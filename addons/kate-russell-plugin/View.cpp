/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_View.cpp                             */
/* Description:     a view for a Russell support plugin for Kate             */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#include <QScrollBar>

#include <KXMLGUIFactory>
#include <KStringHandler>
#include <KMessageBox>
#include <KActionCollection>
#include <KConfigGroup>
#include <KSharedConfig>

#include <KTextEditor/MainWindow>
#include <KTextEditor/View>

#include "ProjectConfigTab.hpp"
#include "View.hpp"
#include "Outline.hpp"
#include "Structure.hpp"
#include "TypeSystem.hpp"
#include "Proof.hpp"
#include "Server.hpp"
#include "LatexToUnicode.hpp"
#include "View.moc"

#include "commands.hpp"
#include "Execute.hpp"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	View :: View (KTextEditor::MainWindow* mw, KTextEditor::Plugin* plugin) :
	QObject(mw),
	plugin_(plugin),
	mainWindow_ (mw),
	toolView_ 
	(
		mainWindow_->createToolView
		(
			plugin_,
			QLatin1String("kate_private_plugin_katerussellplugin_prover"),
			KTextEditor::MainWindow :: Bottom,
			QIcon::fromTheme(QLatin1String("application-x-ms-dos-executable")),
			i18n ("Russell")
		)
	),
	lookupDefinition_ (nullptr),
	openDefinition_ (nullptr),
	latexToUnicode_ (nullptr),
	proveAutomatically_ (nullptr),
	proveInteractive_(nullptr),

	outline_ (new Outline (mainWindow_, this)),
	structure_ (new Structure (mainWindow_, this)),
	typeSystem_ (new TypeSystem (mainWindow_, this)),
	proof_ (new Proof (mainWindow_, this))
	{
		if (instance_)
			KMessageBox::error(0, i18n ("Russell plugin view is already initialized"));
		instance_ = this;
		#ifdef DEBUG_CREATE_DESTROY
		std :: cout << "View :: View (KTextEditor::MainWindow* mw)" << std :: endl;
		#endif

		KXMLGUIClient::setComponentName(QLatin1String("katerussell"), i18n("Russell plugin view"));
		setXMLFile(QLatin1String("ui.rc"));

		initPopupMenu();
		initActions();
		initBottomUi();
		initLauncher();

		mainWindow()->guiFactory()->addClient (this);

		KConfigGroup config(KSharedConfig::openConfig(), QLatin1String("Russell"));
		int count = config.readEntry(QLatin1String("Opened files number"), 0);
		while (count) {
			QUrl file = config.readEntry(QStringLiteral("Opened file %1 name").arg(--count), QUrl());
			mainWindow_->openUrl(file);
		}
	}
	View :: ~View() 
	{
		KConfigGroup config(KSharedConfig::openConfig(), QLatin1String("Russell"));
		int count = 0;
		for (KTextEditor::View* view : mainWindow_->views()) {
			QUrl file = view->document()->url();
			config.writeEntry(QStringLiteral("Opened file %1 name").arg(count++), file);
		}
		config.writeEntry(QLatin1String("Opened files number"), count);

		#ifdef DEBUG_CREATE_DESTROY
		std :: cout << "View :: ~View()" << std :: endl;
		#endif
		Server::russell().stop();
		mainWindow()->guiFactory()->removeClient (this);
		delete toolView_;
		delete outline_;
		delete structure_;
		delete typeSystem_;
		delete proof_;
	}

	State View::getState() const {
		return state_.get().state;
	}
	Ui::Bottom& View::getBottomUi() {
		return bottomUi_;
	}
	QWidget* View::toolView() const {
		return toolView_;
	}
	void 
	View :: clearOutput() 
	{
		bottomUi_.russellTextEdit->clear();
		bottomUi_.metamathTextEdit->clear();
		State s = state_.get().state;
		if ((s == State::LOOKING_DEFINITION) || (s == State::OPENING_DEFINITION) ||
			(s == State::MINING_OUTLINE) || (s == State::MINING_STRUCTURE) ||
			(s == State::MINING_TYPE_SYSTEM)) {
			return;
		}
		bottomUi_.qtabwidget->setCurrentIndex (1);
	}
	void
	View :: openLocation(const QString& loc)
	{
		if (loc.isEmpty()) return;
		QString location = loc;
		QString path;
		int line = 1;
		int column = 1;

		int endlIndex = location.indexOf (QLatin1String("\n"));
		// chopping prefix "path: " (6 chars)
		path = location.mid (0, endlIndex);
		path.remove (0, 6);
		location.remove (0, endlIndex + 1);
		if (!location.isEmpty()) {
			endlIndex = location.indexOf (QLatin1String("\n"));
			// chopping prefix "line: " (6 chars)
			QString lineString = location.mid (0, endlIndex);
			lineString.remove (0, 6);
			location.remove (0, endlIndex + 1);
			line = lineString.toInt();
		}
		if (!location.isEmpty()) {
			endlIndex = location.indexOf (QLatin1String("\n"));
			// chopping prefix "col: " (5 chars)
			QString columnString = location.mid (0, endlIndex);
			columnString.remove (0, 5);
			location.remove (0, endlIndex + 1);
			column = columnString.toInt();
		}
		gotoLocation (path, line, column);
	}

	Proof* View::proof() { return proof_; }

	void 
	View :: mineOutline (const QString& options) 
	{
		QString file = currentFile();
		if (file.size() && state_.start(State::MINING_OUTLINE, file)) {
			Execute::russellClient().execute(command::mine(file, State::MINING_OUTLINE, options));
		}
	}
	void 
	View :: mineStructure (const QString& options) 
	{
		QString file = currentFile();
		if (file.size() && state_.start(State::MINING_STRUCTURE, file)) {
			Execute::russellClient().execute(command::mine(file, State::MINING_STRUCTURE, options));
		}
	}
	void 
	View :: mineTypeSystem (const QString& options) 
	{
		QString file = currentFile();
		if (file.size() && state_.start(State::MINING_TYPE_SYSTEM, file)) {
			Execute::russellClient().execute(command::mine(file, State::MINING_TYPE_SYSTEM, options));
		}
	}
	void 
	View :: gotoLocation (const QString& path, const int line, const int column)
	{
		QUrl url(QLatin1String("file://") + path);
		if (KTextEditor::View* view = mainWindow_->openUrl (url)) {
			mainWindow_->activateView(view->document());
			mainWindow_->activeView()->setCursorPosition (KTextEditor :: Cursor (line, column));
			mainWindow_->activeView()->setFocus();
		} else {
			KMessageBox :: sorry (0, i18n ("Cannot open ") + path);
		}
	}

	QUrl
	View :: currentFileUrl (const bool save) const
	{
		KTextEditor::View* activeView = mainWindow()->activeView();
		if (activeView == NULL) {
			return QUrl();
		}
		if (save && activeView->document()->isModified()) {
			activeView->document()->documentSave();
		}
		return activeView->document()->url();
	}
	QString
	View :: currentFile (const bool save) const
	{
		QUrl url = currentFileUrl (save);
		if (url.isEmpty()) {
			//KMessageBox :: sorry (0, i18n ("There's no active window."));
			return QString();
		}
		QString file = url.toLocalFile();
		if (file_type(file) == Lang::OTHER) {
			//KMessageBox :: sorry (0, i18n ("Current file type is not supported."));
			return QString();
		}
		return file;
	}
	Lang
	View :: currentFileType() const {
		return file_type(currentFile());
	}
	bool
	View :: currentIsRus() const {
		return currentFileType() == Lang::RUS;
	}
	bool
	View :: currentIsMm() const {
		return currentFileType() == Lang::MM;
	}

	/**********************************
	 *	Private Q_SLOTS members
	 **********************************/

	void
	View::slotRead(KTextEditor::View* view) {
		QString file = currentFile();
		if (file.size() && fileChanged(file) && state_.start(State::READING, file)) {
			connect(
				view->document(),
				SIGNAL(documentSavedOrUploaded(KTextEditor::Document*, bool)),
				this,
				SLOT(slotDocumentSaved(KTextEditor::Document*, bool))
			);
			if (Execute::russellClient().execute(command::read(file, ActionScope::FILE))) {
				registerFileRead(file);
			}
		}
	}
	void
	View::slotDocumentSaved(KTextEditor::Document* doc, bool) {
		QString file = doc->url().toLocalFile();
		if (file.size() && fileChanged(file) && state_.start(State::READING, file)) {
			if (Execute::russellClient().execute(command::read(file, ActionScope::FILE))) {
				registerFileRead(file);
			}
		}
	}

	void
	View :: slotRusCommandCompleted(quint32 code, const QString& msg, const QString& data)
	{
		InternalState internal_state = state_.get();
		state_.stop();
		if (!code) {
			QApplication :: restoreOverrideCursor();
			switch (internal_state.state) {
			case State::READING:
				outline_->refresh();
				break;
			case State::LOOKING_DEFINITION :
				if (!data.isEmpty())
					QMessageBox::information(mainWindow_->activeView(), i18n("Definition:"), data);
				break;
			case State::OPENING_DEFINITION :
				openLocation(data);
				break;
			case State::MINING_OUTLINE :
				outline_->update(data);
				break;
			case State::MINING_STRUCTURE :
				structure_->update(data);
				break;
			case State::MINING_TYPE_SYSTEM :
				typeSystem_->update(data);
				break;
			case State::PROVING :
				reloadSource();
				break;
			case State::VERIFYING_RUS :
				//client_->verifyMm(currentFile());
				break;
			case State::TRANSLATING:
				if (internal_state.file.size() && state_.start(State::VERIFYING_MM, internal_state.file)) {
					Execute::metamath().execute(command::verifyMm(internal_state.file, ActionScope::FILE));
				}
				break;
			default :
				/*KPassivePopup :: message
				(
					i18n ("Success"),
					i18n("Proving and verification completed without problems."),
					toolView_
				);*/;
			}
		} else {
			switch (internal_state.state) {
			case State::READING:
				QMessageBox::warning(mainWindow_->activeView(), i18n("Reading failed:"), msg);
				break;
			default :
				QMessageBox::warning(mainWindow_->activeView(), i18n("Some error:"), msg);
			}

			//outputBuffer_ += QStringLiteral("process crushed\n");
			//outputBuffer_ += QStringLiteral("error code: ");
			//outputBuffer_ += QString :: number (code);
			//outputBuffer_ += QStringLiteral("\n");
		}
	}
	void
	View :: slotMmCommandCompleted()
	{
		InternalState internal_state = state_.get();
		state_.stop();
		QApplication :: restoreOverrideCursor();
		switch (internal_state.state) {
		case State::VERIFYING_MM:
			if (internal_state.file.size() && state_.start(State::ERASING_MM, internal_state.file)) {
				Execute::metamath().execute(command::eraseMm(internal_state.file, ActionScope::FILE));
			}
			break;
		default:
			break;
		}

	}

	void 
	View :: slotRefreshOutline() {
		outline_->refresh();
	}

	// main slots
/*	void
	View :: slotProveVerify()
	{
		if (state_.start(State::PROVING)) {
			//clearOutput();
			//client_->prove (false);
		}
	}
	void 
	View :: slotProve()
	{
		if (state_.start(State::PROVING)) {
			//client_->prove();
		}
	}
	void 
	View :: slotProveInteractive()
	{
		if (Server::russell().isRunning() && state_.start(State::PROVING)) {
			//client_->execute(QStringLiteral("option --in prop.rus"));
			//client_->execute(QStringLiteral("read"));
			//client_->execute(QStringLiteral("check"));
			//client_->execute(QStringLiteral("exit"));
			//state_.stop();
		}
	}
*/
	void
	View :: slotTranslate() {
		QString file = currentFile();
		if (file.size() && state_.start(State::TRANSLATING, file)) {
			if (FileConf fc = chooseFileConf(file, ActionScope::FILE)) {
				QString comm;
				comm += command::translate(fc.file, ActionScope::FILE, Lang::MM);
				//comm += command::translate(fc.conf->smmTarget(fc.file, true), ActionScope::FILE, Lang::MM);
				comm += command::merge(fc.conf->mmTarget(fc.file, true), ActionScope::FILE);
				Execute::russellClient().execute(comm);
			} else {
				state_.stop();
			}
		}
	}
	void 
	View :: slotVerify()
	{
		QString file = currentFile();
		//qDebug() << file;
		if (file.size() && state_.start(State::VERIFYING_RUS, file)) {
			Execute::russellClient().execute(command::verifyRus(file, ActionScope::FILE));
		}
	}

	/*void
	View :: slotLearn()
	{
		if (state_.start(VERIFYING)) {
			//client_->learn();
		}
	}
	void
	View :: slotConfirmProof(int proofIndex)
	{
		//client_->setupIndex (proofIndex);
		//client_->execute(QStringLiteral("confirm"));
		//client_->execute(QStringLiteral("write"));
	}

	void
	View :: proveIdAutomatically()
	{
		if (state_.start(State::PROVING)) {
			KTextEditor :: View* activeView = mainWindow()->activeView();
			if (!activeView) {
				qDebug() << "no KTextEditor::View" << endl;
				return;
			}
			if (!activeView->cursorPosition().isValid()) {
				qDebug() << "cursor not valid!" << endl;
				return;
			}
			const int line = activeView->cursorPosition().line();
			const int column = activeView->cursorPosition().column();

			client_->prove (line, column, true);
		}
	}
	void
	View :: proveIdInteractively()
	{
		if (!Server::russell().isRunning()) {
			KMessageBox :: sorry (0, i18n ("Server could not start."));
			return;
		}
		if (state_.start(State::PROVING_INTERACTIVELY)) {
			KTextEditor :: View* activeView = mainWindow()->activeView();
			if (!activeView) {
				qDebug() << "no KTextEditor::View" << endl;
				return;
			}
			if (!activeView->cursorPosition().isValid()) {
				qDebug() << "cursor not valid!" << endl;
				return;
			}
			const int line = activeView->cursorPosition().line();
			const int column = activeView->cursorPosition().column();

			if (!Server::russell().isRunning()) {
				KMessageBox :: sorry (0, i18n ("Server could not start."));
				return;
			}
			proof_->stopProving();
			//client_->setupInFile();
			//client_->setupOutFile();
			client_->execute (QStringLiteral("read"));
			client_->execute (QStringLiteral("check"));
			//client_->setupPosition (line, column);
			proof_->startProving();
			proof_->show();
		}
	}*/

	// server slots
	void
	View :: slotManageServer()
	{
		QAction* action = nullptr;
		if (Server::russell().isRunning()) {
			Server::russell().stop();
			action = actionCollection()->action (QLatin1String("start_server"));
			action->setText (i18n ("Start mdl server"));
			action->setIcon (QIcon (QLatin1String("go-next")));
		} else {
			Server::russell().start();
			action = actionCollection()->action (QLatin1String("start_server"));
			action->setText (i18n ("Stop mdl server"));
			action->setIcon (QIcon (QLatin1String("application-exit")));
		}
	}

	// popup menu
	void 
	View :: showMenu() 
	{
		const QString identifier = currentIdentifier();
		if (identifier.isEmpty()) {
			lookupDefinition_->setText (i18n ("Nothing to lookup"));
			openDefinition_->setText (i18n ("Nothing to open"));
			proveAutomatically_->setText (i18n ("Nothing to prove"));
			proveInteractive_->setText (i18n ("Nothing to prove"));
		} else {
			const QString squeezedIdentifier = KStringHandler :: csqueeze (identifier, 30);
			lookupDefinition_->setText (i18n ("Lookup definition: %1", squeezedIdentifier));
			openDefinition_->setText (i18n ("Open definition: %1", squeezedIdentifier));
			proveAutomatically_->setText (i18n ("Prove automatically: %1", squeezedIdentifier));
			proveInteractive_->setText (i18n ("Prove interactively: %1", squeezedIdentifier));
		}
		int line = -1; int start = -1; int length = -1;
		const QString latexExpression = currentLatexExpression (line, start, length);
		if (latexExpression.isEmpty()) {
			latexToUnicode_->setText (i18n ("No latex expression"));
		} else {
			const QString squeezedExpression = KStringHandler :: csqueeze (latexExpression, 30);
			latexToUnicode_->setText (i18n("Latex to unicode: %1", squeezedExpression));
		}
	}
	void 
	View :: lookupDefinition() 
	{		
		if (!currentIsRus()) return;
		KTextEditor :: View* activeView = mainWindow()->activeView();
		if (!activeView) {
			qDebug() << "no KTextEditor::View" << endl;
			return;
		}
		if (!activeView->cursorPosition().isValid()) {
			qDebug() << "cursor not valid!" << endl;
			return;
		}
		const int line = activeView->cursorPosition().line();
		const int column = activeView->cursorPosition().column();

		QString file = currentFile();
		if (file.size() && state_.start(State::LOOKING_DEFINITION, file)) {
			Execute::russellClient().execute(command::lookupDefinition(file, line, column));
		}
	}
	void 
	View :: openDefinition() 
	{
		KTextEditor :: View* activeView = mainWindow()->activeView();
		if (!activeView) {
			qDebug() << "no KTextEditor::View" << endl;
			return;
		}
		if (!activeView->cursorPosition().isValid()) {
			qDebug() << "cursor not valid!" << endl;
			return;
		}
		const int line = activeView->cursorPosition().line();
		const int column = activeView->cursorPosition().column();

		QString file = currentFile();
		if (file.size() && state_.start(State::OPENING_DEFINITION, file)) {
			Execute::russellClient().execute(command::lookupLocation (file, line, column));
		}
	}
	void 
	View :: latexToUnicode() 
	{
		KTextEditor :: View* activeView = mainWindow()->activeView();
		if (!activeView) {
			qDebug() << "no KTextEditor::View" << endl;
			return;
		}
		if (!activeView->cursorPosition().isValid()) {
			qDebug() << "cursor not valid!" << endl;
			return;
		}
		int line = -1; int start = -1; int length = -1;
		const QString latexWord = currentLatexExpression (line, start, length);
		LatexToUnicode translator (latexWord);
		const QString& unicodeWord = translator.translate();
		const KTextEditor :: Range range (line, start, line, start + length);
		activeView->document()->replaceText (range, unicodeWord, true);
	}

	static void appendText(QPlainTextEdit* textEdit, const QString& text) {
		textEdit->moveCursor (QTextCursor::End);
		textEdit->insertPlainText (text);
		textEdit->moveCursor (QTextCursor::End);
	}

	void
	View :: slotReadRussellStdOut()
	{
		QString serverStdOut = QString :: fromUtf8 (Server::russell().process().readAllStandardOutput());
		appendText(bottomUi_.russellTextEdit, serverStdOut);
		bottomUi_.qtabwidget->setCurrentIndex(1);
	}
	void 
	View :: slotReadRussellStdErr()
	{
		QString serverStdOut = QString :: fromUtf8 (Server::russell().process().readAllStandardError());
		appendText(bottomUi_.russellTextEdit, serverStdOut);
		bottomUi_.qtabwidget->setCurrentIndex(1);
	}
	void
	View :: slotReadMetamathStdOut()
	{
		QString serverStdOut = QString :: fromUtf8 (Server::metamath().process().readAllStandardOutput());
		appendText(bottomUi_.metamathTextEdit, serverStdOut);
		bottomUi_.qtabwidget->setCurrentIndex(0);
		if (serverStdOut.mid(serverStdOut.length() - 4, 4) == QLatin1String("MM> ")) {
			emit mmCommandFinished();
		}
	}
	void
	View :: slotReadMetamathStdErr()
	{
		QString serverStdOut = QString :: fromUtf8 (Server::russell().process().readAllStandardError());
		appendText(bottomUi_.metamathTextEdit, serverStdOut);
		bottomUi_.qtabwidget->setCurrentIndex(0);
	}

	void View::slotExecuteRussellCommand() {
		QString command = bottomUi_.russellCommandComboBox->currentText();
		Execute::russellClient().execute(command + QLatin1String("\n"));
	}
	void View::slotExecuteMetamathCommand() {
		QString command = bottomUi_.metamathCommandComboBox->currentText();
		Execute::metamath().execute(command + QLatin1String("\n"));
	}

	/****************************
	 *	Private members
	 ****************************/

	void 
	View :: reloadSource()
	{
		KTextEditor :: View* activeView = mainWindow()->activeView();
		if (activeView == NULL) {
			return;
		}    
		activeView->document()->documentReload();
	}
	QString 
	View :: currentIdentifier() const
	{
		KTextEditor :: View* activeView = mainWindow()->activeView();
		if (!activeView) {
			//qDebug() << "no KTextEditor::View" << endl;
			return QString();
		}
		if (!activeView->cursorPosition().isValid()) {
			//qDebug() << "cursor not valid!" << endl;
			return QString();
		}
		const int line = activeView->cursorPosition().line();
		const int col = activeView->cursorPosition().column();

		QString linestr = activeView->document()->line (line);

		int startPos = qMax (qMin (col, linestr.length() - 1), 0);
		int endPos = startPos;
		while (startPos >= 0) {
			bool inId = linestr[startPos].isLetterOrNumber();
			inId = inId || (linestr[startPos] == QLatin1Char('_'));
			inId = inId || (linestr[startPos] == QLatin1Char('.'));
			inId = inId || (linestr[startPos] == QLatin1Char('-'));
			if (!inId) {
				break;
			}
			-- startPos;
		}
		while (endPos < linestr.length()) {
			bool inId = linestr[endPos].isLetterOrNumber();
			inId = inId || (linestr[endPos] == QLatin1Char('_'));
			inId = inId || (linestr[endPos] == QLatin1Char('.'));
			inId = inId || (linestr[endPos] == QLatin1Char('-'));
			if (!inId) {
				break;
			}
			++ endPos;
		}
		if  (startPos == endPos) {
			//qDebug() << "no word found!" << endl;
			return QString();
		}
		//qDebug() << linestr.mid(startPos+1, endPos-startPos-1);
		return linestr.mid (startPos + 1, endPos - startPos - 1);
	}
	QString 
	View :: currentLatexExpression (int& line, int& begin, int& length) const
	{
		KTextEditor :: View* activeView = mainWindow()->activeView();
		if (!activeView) {
			//qDebug() << "no KTextEditor::View" << endl;
			return QString();
		}
		if (!activeView->cursorPosition().isValid()) {
			//qDebug() << "cursor not valid!" << endl;
			return QString();
		}
		line = activeView->cursorPosition().line();
		const int col = activeView->cursorPosition().column();

		QString linestr = activeView->document()->line (line);

		int startPos = qMax (qMin (col, linestr.length() - 1), 0);
		int endPos = startPos;
		while (startPos >= 0) {
			bool out =   (linestr [startPos] == QLatin1Char(' '));
			out = out || (linestr [startPos] == QLatin1Char('\t'));
			out = out || (linestr [startPos] == QLatin1Char('\n'));
			out = out || (linestr [startPos] == QLatin1Char('\r'));
			if (out) {
				break;
			}
			-- startPos;
		}
		while (endPos < linestr.length()) {
			bool out =   (linestr [endPos] == QLatin1Char(' '));
			out = out || (linestr [endPos] == QLatin1Char('\t'));
			out = out || (linestr [endPos] == QLatin1Char('\n'));
			out = out || (linestr [endPos] == QLatin1Char('\r'));
			if (out) {
				break;
			}
			++ endPos;
		}
		if  (startPos == endPos) {
			//qDebug() << "no word found!" << endl;
			return QString();
		}
		begin = startPos + 1;
		length = endPos - startPos - 1;
		return linestr.mid (begin, length);
	}
	bool
	View :: checkLocal (const QUrl &url) const
	{
		if (url.path().isEmpty()) {
			KMessageBox :: sorry(0, i18n ("There is no file or directory specified for proving/translating/verifying."));
			return false;
		} else if (!url.isLocalFile()) {
			KMessageBox :: sorry (0, i18n
			(
				"The file \"%1\" is not a local file."
				"Non-local files cannot be processed.",
				url.path()
			));
			return false;
		}
		return true;
	}

	void
	View :: initPopupMenu()
	{
		// popup menu
		popupMenu_ = new KActionMenu (i18n ("Russell"), this);
		actionCollection()->addAction(QLatin1String("popup_russell"), popupMenu_);

		lookupDefinition_ = popupMenu_->menu()->addAction (i18n ("Lookup definition: %1", QString()), this, SLOT(lookupDefinition()));
		openDefinition_   = popupMenu_->menu()->addAction(i18n("Open definition: %1", QString()), this, SLOT(openDefinition()));
		latexToUnicode_   = popupMenu_->menu()->addAction(i18n("Latex to unicode: %1", QString()), this, SLOT(latexToUnicode()));
		proveAutomatically_ = popupMenu_->menu()->addAction(i18n("Prove automatically: %1", QString()), this, SLOT(proveIdAutomatically()));
		proveInteractive_   = popupMenu_->menu()->addAction(i18n("Prove interactively: %1", QString()), this, SLOT(proveIdInteractively()));

		latexToUnicode_->setShortcut (QKeySequence (Qt :: CTRL + Qt :: SHIFT + Qt :: Key_R));
		connect (popupMenu_->menu(), SIGNAL (aboutToShow()), this, SLOT (showMenu()));
	}
	void
	View :: initActions()
	{
		QAction* action = NULL;

		action = actionCollection()->addAction (QLatin1String("prove_verify"));
		action->setText (i18n ("Prove && Verify"));
		actionCollection()->setDefaultShortcut(action, QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_F));
		connect (action, SIGNAL (triggered (bool)), this, SLOT (slotProveVerify()));

		action = actionCollection()->addAction (QLatin1String("prove"));
		action->setText (i18n ("Prove"));
		actionCollection()->setDefaultShortcut(action, QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_P));
		connect (action, SIGNAL (triggered (bool)), this, SLOT (slotProve()));

		action = actionCollection()->addAction (QLatin1String("translate"));
		action->setText (i18n ("Translate"));
		actionCollection()->setDefaultShortcut(action, QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_T));
		connect (action, SIGNAL (triggered (bool)), this, SLOT (slotTranslate()));

		action = actionCollection()->addAction (QLatin1String("verify"));
		action->setText (i18n ("Verify"));
		actionCollection()->setDefaultShortcut(action, QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_V));
		connect (action, SIGNAL (triggered (bool)), this, SLOT (slotVerify()));

		action = actionCollection()->addAction (QLatin1String("learn"));
		action->setText (i18n ("Learn"));
		actionCollection()->setDefaultShortcut(action, QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_L));
		connect (action, SIGNAL (triggered (bool)), this, SLOT (slotLearn()));

		//action = actionCollection()->addAction (QLatin1String("stop"));
		//action->setText (i18n ("Stop"));
		//+connect (action, SIGNAL (triggered (bool)), this, SLOT (slotStop()));

		//action = actionCollection()->addAction (QLatin1String("goto_next"));
		//action->setText (i18n ("Next Error"));
		//a->setShortcut (QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_Right));
		//connect (action, SIGNAL (triggered (bool)), this, SLOT (slotNext()));

		//action = actionCollection()->addAction (QLatin1String("goto_prev"));
		//action->setText (i18n ("Previous Error"));
		//a->setShortcut (QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_Left));
		//connect (action, SIGNAL (triggered (bool)), this, SLOT (slotPrev()));

		/*a = actionCollection()->addAction ("target_next");
		a->setText (i18n ("Next Target"));
		connect (a, SIGNAL (triggered (bool)), this, SLOT (configNext()));*/

		action = actionCollection()->addAction (QLatin1String("prove_verify_quick"));
		action->setText (i18n ("Prove/Verify"));

		/*static const QPixmap provePixmap (prove_verify_xpm_);
		QIcon (provePixmap)
		a->setIcon (QIcon (provePixmap));*/
		//a->setIcon (QIcon ("media-playback-start"));
		//action->setIcon (QIcon ("preferences-kcalc-constants"));
		action->setIcon (QIcon (QLatin1String("arrow-right")));
		connect (action, SIGNAL (triggered(bool)), this, SLOT (slotProveVerify()));

		action = actionCollection()->addAction (QLatin1String("prove_interactive"));
		action->setText (i18n ("Prove Interactive"));

		/*static const QPixmap provePixmap (prove_verify_xpm_);
		QIcon (provePixmap)
		a->setIcon (QIcon (provePixmap));*/
		//a->setIcon (QIcon ("media-playback-start"));
		action->setIcon (QIcon(QLatin1String("arrow-right-double")));
		action->setIcon (QIcon(QLatin1String(":/katerussell/icons/hi22-actions-russell-axiom.png")));
		connect (action, SIGNAL (triggered(bool)), this, SLOT (slotProveInteractive()));

		action = actionCollection()->addAction (QLatin1String("start_server"));
		action->setText (i18n ("Start mdl server"));

		/*static const QPixmap provePixmap (prove_verify_xpm_);
		QIcon (provePixmap)
		a->setIcon (QIcon (provePixmap));*/
		//a->setIcon (QIcon ("media-playback-start"));
		action->setIcon (QIcon (QLatin1String("go-next")));
		connect (action, SIGNAL (triggered(bool)), this, SLOT (slotManageServer()));
	}
	void
	View :: initBottomUi()
	{
		QWidget* configOutputWidget = new QWidget (toolView_);
		bottomUi_.setupUi (configOutputWidget);

		ProjectConfigTab* projectsTab = new ProjectConfigTab(toolView_);
		bottomUi_.qtabwidget->addTab (projectsTab, i18nc ("Tab label", "Projects"));
		bottomUi_.qtabwidget->setCurrentWidget (projectsTab);

		connect(bottomUi_.russellClearButton, SIGNAL(clicked()), bottomUi_.russellTextEdit, SLOT(clear()));
		connect(bottomUi_.metamathClearButton, SIGNAL(clicked()), bottomUi_.metamathTextEdit, SLOT(clear()));
		connect(bottomUi_.russellExecuteButton, SIGNAL(clicked()), SLOT(slotExecuteRussellCommand()));
		connect(bottomUi_.metamathExecuteButton, SIGNAL(clicked()), SLOT(slotExecuteMetamathCommand()));
	}
	void
	View :: initLauncher()
	{
		connect (&Server::russell().process(), SIGNAL (readyReadStandardError()), this, SLOT (slotReadRussellStdErr()));
		connect (&Server::russell().process(), SIGNAL (readyReadStandardOutput()), this, SLOT (slotReadRussellStdOut()));
		connect (&Server::metamath().process(), SIGNAL (readyReadStandardError()), this, SLOT (slotReadMetamathStdErr()));
		connect (&Server::metamath().process(), SIGNAL (readyReadStandardOutput()), this, SLOT (slotReadMetamathStdOut()));
		connect (proof_, SIGNAL (proofFound(int)), this, SLOT (slotConfirmProof(int)));
		connect (&Execute::russellClient(), SIGNAL (dataReceived(quint32, QString, QString)), this, SLOT(slotRusCommandCompleted(quint32, QString, QString)));
		connect (this, SIGNAL (mmCommandFinished()), this, SLOT(slotMmCommandCompleted()));
		connect (mainWindow_, SIGNAL (viewChanged(KTextEditor::View*)), this, SLOT (slotRefreshOutline()));
		connect (mainWindow_, SIGNAL (viewCreated(KTextEditor::View*)), this, SLOT (slotRead(KTextEditor::View*)));
		connect (mainWindow_, SIGNAL (viewCreated(KTextEditor::View*)), this, SLOT (slotRead(KTextEditor::View*)));
	}

	View* View::instance_ = nullptr;
}
