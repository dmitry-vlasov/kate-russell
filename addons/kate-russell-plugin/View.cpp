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

#include <KTextEditor/MainWindow>
#include <KTextEditor/View>

#include "ProjectConfigTab.hpp"
#include "View.hpp"
#include "Outline.hpp"
#include "Structure.hpp"
#include "TypeSystem.hpp"
#include "Proof.hpp"
#include "Client.hpp"
#include "Server.hpp"
#include "LatexToUnicode.hpp"
#include "View.moc"
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
			QStringLiteral("kate_private_plugin_katerussellplugin_prover"),
			KTextEditor::MainWindow :: Bottom,
			QIcon::fromTheme(QStringLiteral("application-x-ms-dos-executable")),
			i18n ("Russell")
		)
	),
	bottomUi_ (),

	popupMenu_ (),
	lookupDefinition_ (NULL),
	openDefinition_ (NULL),
	latexToUnicode_ (NULL),
	proveAutomatically_ (NULL),
	proveInteractive_(NULL),

	outline_ (new Outline (mainWindow_, this)),
	structure_ (new Structure (mainWindow_, this)),
	typeSystem_ (new TypeSystem (mainWindow_, this)),
	proof_ (new Proof (mainWindow_, this)),

	client_ (new Client (this)),

	output_ (),
	outputBuffer_ (),
	state_ (WAITING)
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
	}
	View :: ~View() 
	{
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
		delete client_;
	}

	View :: State_
	View :: getState() const {
		return state_;
	}
	Ui :: Bottom&
	View :: getBottomUi() {
		return bottomUi_;
	}
	QWidget*
	View :: toolView() const {
		return toolView_;
	}
	void 
	View :: clearOutput() 
	{
		output_.clear();
		bottomUi_.outputTextEdit->clear();
		if ((state_ == LOOKING_DEFINITION) || (state_ == OPENING_DEFINITION) ||
			(state_ == MINING_OUTLINE) || (state_ == MINING_STRUCTURE) || 
			(state_ == MINING_TYPE_SYSTEM)) {
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

		int endlIndex = location.indexOf (QStringLiteral("\n"));
		// chopping prefix "path: " (6 chars)
		path = location.mid (0, endlIndex);
		path.remove (0, 6);
		location.remove (0, endlIndex + 1);
		if (!location.isEmpty()) {
			endlIndex = location.indexOf (QStringLiteral("\n"));
			// chopping prefix "line: " (6 chars)
			QString lineString = location.mid (0, endlIndex);
			lineString.remove (0, 6);
			location.remove (0, endlIndex + 1);
			line = lineString.toInt();
		}
		if (!location.isEmpty()) {
			endlIndex = location.indexOf (QStringLiteral("\n"));
			// chopping prefix "col: " (5 chars)
			QString columnString = location.mid (0, endlIndex);
			columnString.remove (0, 5);
			location.remove (0, endlIndex + 1);
			column = columnString.toInt();
		}
		gotoLocation (path, line, column);
	}

	Client*
	View :: client() {
		return client_;
	}
	Proof*
	View :: proof() {
		return proof_;
	}

	const QString& 
	View :: getOutput() const {
		return output_;
	}
	void 
	View :: mineOutline (const QString& options) 
	{
		QString file = currentFile();
		if (!file.size()) return;
		state_ = MINING_OUTLINE;
		client_->mine (file, options);
	}
	void 
	View :: mineStructure (const QString& options) 
	{
		QString file = currentFile();
		if (!file.size()) return;
		state_ = MINING_STRUCTURE;
		client_->mine (file, options);
	}
	void 
	View :: mineTypeSystem (const QString& options) 
	{
		QString file = currentFile();
		if (!file.size()) return;
		state_ = MINING_TYPE_SYSTEM;
		client_->mine (file, options);
	}
	void 
	View :: gotoLocation (const QString& path, const int line, const int column)
	{
		QUrl url(QStringLiteral("file://") + path);
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
	bool
	View :: currentIsSmm() const {
		return currentFileType() == Lang::SMM;
	}

	/**********************************
	 *	Private Q_SLOTS members
	 **********************************/

	void
	View::slotRead(KTextEditor::View* view) {
		QString file = currentFile();
		if (!file.size()) return;
		connect(
			view->document(),
			SIGNAL(documentSavedOrUploaded(KTextEditor::Document*, bool)),
			this,
			SLOT(slotDocumentSaved(KTextEditor::Document*, bool))
		);
		state_ = READING;
		client_->open(file);
	}
	void
	View::slotDocumentSaved(KTextEditor::Document* doc, bool) {
		QString file = doc->url().toLocalFile();
		if (!file.size()) return;
		state_ = READING;
		client_->open(file);
	}

	void
	View :: slotCommandCompleted(quint32 code, const QString& msg, const QString& data)
	{
		if (!code) {
			output_ += msg;
			outputBuffer_ += msg;
			showBuffer(true);
			//slotReadOutputStdOut (true);
			//slotReadOutputStdErr (true);

			QApplication :: restoreOverrideCursor();
			switch (state_) {
			case READING:
				outline_->refresh();
				break;
			case LOOKING_DEFINITION :
				if (!data.isEmpty())
					QMessageBox::information(mainWindow_->activeView(), i18n("Definition:"), data);
				break;
			case OPENING_DEFINITION :
				openLocation(data);
				break;
			case MINING_OUTLINE :
				outline_->update(data);
				break;
			case MINING_STRUCTURE :
				structure_->update(data);
				break;
			case MINING_TYPE_SYSTEM :
				typeSystem_->update(data);
				break;
			case PROVING :
				reloadSource();
				break;
			case VERIFYING :
				client_->verifyMm(currentFile());
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
			switch (state_) {
			case READING:
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
		state_ = WAITING;
	}

	void 
	View :: slotRefreshOutline() {
		outline_->refresh();
	}

	// main slots
	void 
	View :: slotProveVerify()
	{
		state_ = PROVING;
		clearOutput();
		outputBuffer_ += QStringLiteral("----- proving -----\n");
		client_->prove (false);
	}
	void 
	View :: slotProve()
	{
		state_ = PROVING;
		client_->prove();
	}
	void 
	View :: slotProveInteractive()
	{
		if (Server::russell().isRunning()) {
			state_ = PROVING;
			client_->execute(QStringLiteral("option --in prop.rus"));
			client_->execute(QStringLiteral("read"));
			client_->execute(QStringLiteral("check"));
			client_->execute(QStringLiteral("exit"));
			state_ = WAITING;
		}
	}
	void
	View :: slotTranslate() {
		QString file = currentFile();
		if (!file.size()) return;
		state_ = TRANSLATING;
		client_->translate(file);
	}
	void 
	View :: slotVerify()
	{
		QString file = currentFile();
		qDebug() << file;
		if (!file.size()) return;
		state_ = VERIFYING;
		client_->verify(file);
	}
	void 
	View :: slotLearn()
	{
		state_ = VERIFYING;
		client_->learn();
	}
	void
	View :: slotConfirmProof(int proofIndex)
	{
		//client_->setupIndex (proofIndex);
		client_->execute(QStringLiteral("confirm"));
		client_->execute(QStringLiteral("write"));
	}

	void
	View :: proveIdAutomatically()
	{
		state_ = PROVING;
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
	void
	View :: proveIdInteractively()
	{
		if (!Server::russell().isRunning()) {
			KMessageBox :: sorry (0, i18n ("Server cound not start."));
			return;
		}

		state_ = PROVING_INTERACTIVELY;
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
		proof_->fell();
		//client_->setupInFile();
		//client_->setupOutFile();
		client_->execute (QStringLiteral("read"));
		client_->execute (QStringLiteral("check"));
		//client_->setupPosition (line, column);
		proof_->plant();
		proof_->show();
	}

	// server slots
	void
	View :: slotManageServer()
	{
		QAction* action = NULL;
		if (Server::russell().isRunning()) {
			Server::russell().stop();
			action = actionCollection()->action (QStringLiteral("start_server"));
			action->setText (i18n ("Start mdl server"));
			action->setIcon (QIcon (QStringLiteral("go-next")));
		} else {
			Server::russell().start();
			action = actionCollection()->action (QStringLiteral("start_server"));
			action->setText (i18n ("Stop mdl server"));
			action->setIcon (QIcon (QStringLiteral("application-exit")));
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
		if (!file.size()) return;
		state_ = LOOKING_DEFINITION;
		client_->lookupDefinition (file, line, column);
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
		if (!file.size()) return;
		state_ = OPENING_DEFINITION;
		client_->lookupLocation (file, line, column);
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

	// process slots
/*
	void 
	View :: slotProcessExited (const int exitCode, QProcess :: ExitStatus exitStatus)
	{	
		//slotReadOutputStdOut (true);
		//slotReadOutputStdErr (true);
		QApplication :: restoreOverrideCursor();

		// did we get any errors?
		if ((errorParser_->numErrors() == 0) &&
		    (errorParser_->numWarnings() == 0) &&
		    (exitCode == 0) && 
		    (exitStatus == QProcess :: NormalExit)) 
		{
			if (chain_) {
				switch (state_) {
				case PROVING :
					state_ = TRANSLATING;
					reloadSource();
					bottomUi_.outputTextEdit->appendPlainText (QStringLiteral("\n"));
					outputBuffer_ += QStringLiteral("----- translating -----\n");
					client_->translate (false);
					return;
				case TRANSLATING :
					state_ = VERIFYING;
					bottomUi_.outputTextEdit->appendPlainText (QStringLiteral("\n"));
					outputBuffer_ += QStringLiteral("----- verifying -----\n");
					client_->verify (false);
					return;
				case VERIFYING:
					state_ = WAITING;
					chain_ = false;
				case WAITING :
				default :
					/*KNotification::event(
						KNotification::Notification,
						i18n ("Success"),
						i18n("Proving and verification completed without problems."),
						toolView_
					);* /
					//QWidget* msg = new QWidget();
					//msg->
					//mainWindow_->activeView()->layout()->addWidget(msg);
					/*KPassivePopup :: message
					(
						i18n ("Success"),
						i18n("Proving and verification completed without problems."),
						toolView_
					);* /;
				}
			} else {
				switch (state_) {
				case LOOKING_DEFINITION :
					if (!output_.isEmpty()) {
						/*KPassivePopup :: message
						(
							i18n ("Definition:"), 
							output_, 
							mainWindow()->activeView()
						);* /
					}
					break;
				case OPENING_DEFINITION :
					openLocation();
					break;
				case MINING_OUTLINE :
					outline_->update();
					break;
				case MINING_STRUCTURE :
					structure_->update();
					break;
				case MINING_TYPE_SYSTEM :
					typeSystem_->update();
					break;
				case PROVING :
					reloadSource();
				default :
					/*KPassivePopup :: message
					(
						i18n ("Success"),
						i18n("Proving and verification completed without problems."),
						toolView_
					);* /;
				}
			}
		} else {
			chain_ = false;
			state_ = WAITING;
			if (exitStatus == QProcess :: NormalExit) {
				outputBuffer_ += QStringLiteral("process ended\n");
			} else {
				outputBuffer_ += QStringLiteral("process crushed\n");
				outputBuffer_ += QStringLiteral("error code: ");
				outputBuffer_ += QString :: number (exitCode);
				outputBuffer_ += QStringLiteral("\n");
			}
		}
	}
	void 
	View :: slotReadOutputStdOut (const bool forceOutput)
	{
		QString newLines = QString :: fromUtf8 (client_->process()->readAllStandardOutput());
		newLines.remove (QLatin1Char('\r'));
		output_ += newLines;
		outputBuffer_ += newLines;
		showBuffer (forceOutput);
	}
	void
	View :: slotReadOutputStdErr (const bool forceOutput)
	{
		QString newLines = QString :: fromUtf8 (client_->process()->readAllStandardError());
		newLines.remove (QLatin1Char('\r'));
		errorParser_->proceed (newLines);
		output_ += newLines;
		outputBuffer_ += newLines;
		showBuffer (forceOutput);
	}
*/

	static void output(const QString& serverStdOut, QListWidget* listWidget)
	{
		QStringList newLines = serverStdOut.split(QLatin1Char ('\n'));
		QListWidgetItem* item = listWidget->currentItem();
		if (item) {
			item->setText(item->text() + newLines.first());
		} else {
			listWidget->insertItem(0, newLines.first());
		}
		newLines.removeFirst();
		int row = listWidget->count();
		listWidget->insertItems (row, newLines);
		row = listWidget->count();
		listWidget->setCurrentRow (row - 1);
	}

	void
	View :: slotReadRussellStdOut()
	{
		QString serverStdOut = QString :: fromUtf8 (Server::russell().process().readAllStandardOutput());
		output(serverStdOut, bottomUi_.russellListWidget);
	}
	void 
	View :: slotReadRussellStdErr()
	{
		QString serverStdOut = QString :: fromUtf8 (Server::russell().process().readAllStandardError());
		output(serverStdOut, bottomUi_.russellListWidget);
	}
	void
	View :: slotReadMetamathStdOut()
	{
		QString serverStdOut = QString :: fromUtf8 (Server::metamath().process().readAllStandardOutput());
		qDebug() << serverStdOut;
		output(serverStdOut, bottomUi_.metamathListWidget);
	}
	void
	View :: slotReadMetamathStdErr()
	{
		QString serverStdOut = QString :: fromUtf8 (Server::russell().process().readAllStandardError());
		output(serverStdOut, bottomUi_.metamathListWidget);
	}
	void
	View :: slotShowServerMessages (QString messages)
	{
		output(messages, bottomUi_.metamathListWidget);
		/*QStringList newLines = messages.split(QLatin1Char ('\n'), QString :: SkipEmptyParts);
		int row = bottomUi_.russellListWidget->count();
		bottomUi_.russellListWidget->insertItems (row, newLines);
		row = bottomUi_.russellListWidget->count();
		bottomUi_.russellListWidget->setCurrentRow (row - 1);*/
	}

	/****************************
	 *	Private members
	 ****************************/

	void 
	View :: showBuffer (const bool forceOutput) 
	{
		if (!outputBuffer_.contains (QLatin1Char('\n')) && !forceOutput) {
			return;
		}
		int end = 0;
		while (true) {
			end = outputBuffer_.indexOf (QLatin1Char('\n'), 0);
			if (end == -1) {
				break;
			}
			QString line = outputBuffer_.mid (0, end);
			outputBuffer_.remove (0, (end == -1) ? -1 : end + 1);
			if (!line.isEmpty()) {
				bottomUi_.outputTextEdit->appendPlainText (line);
			}
		}
	}
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
		action->setIcon (QIcon(QStringLiteral(":/katerussell/icons/hi22-actions-russell-axiom.png")));
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

		/*connect
		(
			bottomUi_.errTreeWidget,
			SIGNAL (itemClicked (QTreeWidgetItem*, int)),
			SLOT (slotItemSelected (QTreeWidgetItem*))
		);*/
		/*
		connect
		(
			bottomUi_.russellExecuteButton,
			SIGNAL (pressed()),
			client_,
			SLOT (executeCommand())
		);*/

		bottomUi_.outputTextEdit->setReadOnly (true);
	}
	void
	View :: initLauncher()
	{
		//connect (client_->process(), SIGNAL (finished (int, QProcess::ExitStatus)), this, SLOT (slotProcessExited (const int, QProcess::ExitStatus)));
		//connect (client_->process(), SIGNAL (readyReadStandardError()), this, SLOT (slotReadOutputStdErr()));
		//connect (client_->process(), SIGNAL (readyReadStandardOutput()), this, SLOT (slotReadOutputStdOut()));

		connect (&Server::russell().process(), SIGNAL (readyReadStandardError()), this, SLOT (slotReadRussellStdErr()));
		connect (&Server::russell().process(), SIGNAL (readyReadStandardOutput()), this, SLOT (slotReadRussellStdOut()));
		connect (&Server::metamath().process(), SIGNAL (readyReadStandardError()), this, SLOT (slotReadMetamathStdErr()));
		connect (&Server::metamath().process(), SIGNAL (readyReadStandardOutput()), this, SLOT (slotReadMetamathStdOut()));

		connect (client_, SIGNAL (showServerMessages(QString)), this, SLOT (slotShowServerMessages(QString)));

		connect (proof_, SIGNAL (proofFound(int)), this, SLOT (slotConfirmProof(int)));

		connect (&Execute::russell(), SIGNAL(dataReceived(quint32, QString, QString)), this, SLOT(slotCommandCompleted(quint32, QString, QString)));

		//connect (mainWindow_->activeView(), SIGNAL (viewChanged()), this, SLOT (refreshOutline()));

		connect (mainWindow_, SIGNAL (viewCreated(KTextEditor::View*)), this, SLOT (slotRead(KTextEditor::View*)));
		connect (mainWindow_, SIGNAL (viewCreated(KTextEditor::View*)), this, SLOT (slotRead(KTextEditor::View*)));
	}

	View* View::instance_ = nullptr;
}
