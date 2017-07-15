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

#ifndef PLUGIN_KATE_RUSSELL_VIEW_CPP_
#define PLUGIN_KATE_RUSSELL_VIEW_CPP_

#include <KXMLGUIFactory>
#include "View.moc"
#include "russell.hpp"
#include "ProjectConfigTab.hpp"

namespace plugin {
namespace kate {
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

	client_ (new mdl :: Client (this)),

	errorParser_ (new ErrorParser (this)),
	output_ (),
	outputBuffer_ (),
	state_ (WAITING),
	chain_ (false)
	{
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
		mdl::Server::stop();
		mainWindow()->guiFactory()->removeClient (this);
		delete toolView_;
		delete outline_;
		delete structure_;
		delete typeSystem_;
		delete proof_;
		delete client_;
		delete errorParser_;
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
		bottomUi_.errTreeWidget->clear();
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
		QString pathUrl(QStringLiteral("file://") + path);
		gotoLocation (pathUrl, line, column);
	}

	mdl :: Client*
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
		state_ = MINING_OUTLINE;
		client_->mine (options);
	}
	void 
	View :: mineStructure (const QString& options) 
	{
		state_ = MINING_STRUCTURE;
		client_->mine (options);
	}
	void 
	View :: mineTypeSystem (const QString& options) 
	{
		state_ = MINING_TYPE_SYSTEM;
		client_->mine (options);
	}
	void 
	View :: gotoLocation (const QString& path, const int line, const int column)
	{
		QUrl url(path);
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
	bool
	View :: currentIsRussell() const
	{
		QUrl url = currentFileUrl();
		QString path (url.toLocalFile());
		return (path.endsWith (QStringLiteral(".mdl")) || path.endsWith (QStringLiteral(".rus")));
	}
	bool
	View :: currentIsMetamath() const
	{
		QUrl url = currentFileUrl();
		QString path (url.toLocalFile());
		return (path.endsWith (QStringLiteral(".smm")) || path.endsWith (QStringLiteral(".mm")));
	}

	void
	View :: update()
	{
		//slotReadOutputStdOut (true);
		//slotReadOutputStdErr (true);
		QApplication :: restoreOverrideCursor();
		switch (state_) {
		case LOOKING_DEFINITION :
			if (!output_.isEmpty()) {
				/*KPassivePopup :: message
				(
					i18n ("Definition:"),
					output_,
					mainWindow()->activeView()
				);*/
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
			break;
		default :
			/*KPassivePopup :: message
			(
				i18n ("Success"),
				i18n("Proving and verification completed without problems."),
				toolView_
			);*/;
		}
	}

	/**********************************
	 *	Private Q_SLOTS members
	 **********************************/

	void 
	View :: refreshOutline() {
		outline_->refresh();
	}

	// selecting warnings
	void 
	View :: slotItemSelected (QTreeWidgetItem* item)
	{
		// get stuff
		const QString fileconfigName_ = item->data(0, Qt::UserRole).toString();
		if (fileconfigName_.isEmpty()) {
			return;
		}
		const int line = item->data (1, Qt::UserRole).toInt();
		const int column = item->data (2, Qt::UserRole).toInt();

		// open file (if needed, otherwise, this will activate only the right view...)
		mainWindow_->openUrl (QUrl (fileconfigName_));

		// any view active?
		if (!mainWindow_->activeView()) {
			return;	
		}
		// do it ;)
		mainWindow_->activeView()->setCursorPosition (KTextEditor::Cursor (line - 1, column));
		mainWindow_->activeView()->setFocus();
	}
	void 
	View :: slotNext()
	{
		const int itemCount = bottomUi_.errTreeWidget->topLevelItemCount();
		if (itemCount == 0) {
			return;
		}
		QTreeWidgetItem *item = bottomUi_.errTreeWidget->currentItem();

		int i = 
			(item == 0) ? 
			-1 : 
			bottomUi_.errTreeWidget->indexOfTopLevelItem (item);
		while (++ i < itemCount) {
			item = bottomUi_.errTreeWidget->topLevelItem (i);
			if (!item->text(1).isEmpty()) {
				bottomUi_.errTreeWidget->setCurrentItem (item);
				slotItemSelected (item);
				return;
			}
		}
	}
	void 
	View :: slotPrev()
	{
		const int itemCount = bottomUi_.errTreeWidget->topLevelItemCount();
		if (itemCount == 0) {
			return;
		}
		QTreeWidgetItem *item = bottomUi_.errTreeWidget->currentItem();
		int i = 
			(item == 0) ? 
			itemCount : 
			bottomUi_.errTreeWidget->indexOfTopLevelItem(item);
		while (--i >= 0) {
			item = bottomUi_.errTreeWidget->topLevelItem(i);
			if (!item->text(1).isEmpty()) {
				bottomUi_.errTreeWidget->setCurrentItem(item);
				slotItemSelected(item);
				return;
			}
		}
	}

	// main slots
	void 
	View :: slotProveVerify()
	{
		state_ = PROVING;
		chain_ = true;
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
		if (mdl::Server::is_running()) {
			state_ = PROVING;
			client_->execute(QStringLiteral("option --in prop.rus"));
			client_->execute(QStringLiteral("read"));
			client_->execute(QStringLiteral("check"));
			client_->execute(QStringLiteral("exit"));
			state_ = WAITING;
		}
	}
	void
	View :: slotTranslate()
	{
		state_ = TRANSLATING;
		client_->translate();
	}
	void 
	View :: slotVerify()
	{
		state_ = VERIFYING;
		client_->verify();
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
		client_->setupIndex (proofIndex);
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
		if (!mdl::Server::is_running()) {
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

		if (!mdl::Server::is_running()) {
			KMessageBox :: sorry (0, i18n ("Server could not start."));
			return;
		}
		proof_->fell();
		client_->setupInFile();
		client_->setupOutFile();
		client_->execute (QStringLiteral("read"));
		client_->execute (QStringLiteral("check"));
		client_->setupPosition (line, column);
		proof_->plant();
		proof_->show();
	}

	// server slots
	void
	View :: slotManageServer()
	{
		QAction* action = NULL;
		if (mdl::Server::is_running()) {
			mdl::Server::stop();
			action = actionCollection()->action (QStringLiteral("start_server"));
			action->setText (i18n ("Start mdl server"));
			action->setIcon (QIcon (QStringLiteral("go-next")));
		} else {
			mdl::Server::start();
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
		state_ = LOOKING_DEFINITION;
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

		client_->lookupDefinition (line, column);
	}
	void 
	View :: openDefinition() 
	{
		state_ = OPENING_DEFINITION;
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

		client_->lookupLocation (line, column);
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

	void
	View :: slotReadServerStdOut()
	{
		QString serverStdOut = QString :: fromUtf8 (mdl::Server::process().readAllStandardOutput());
		QStringList newLines = serverStdOut.split(QLatin1Char ('\n'), QString :: SkipEmptyParts);
		int row = bottomUi_.serverListWidget->count();
		bottomUi_.serverListWidget->insertItems (row, newLines);
		row = bottomUi_.serverListWidget->count();
		bottomUi_.serverListWidget->setCurrentRow (row - 1);
	}
	void 
	View :: slotReadServerStdErr()
	{
		QString serverStdOut = QString :: fromUtf8 (mdl::Server::process().readAllStandardError());
		QStringList newLines = serverStdOut.split(QLatin1Char ('\n'), QString :: SkipEmptyParts);
		int row = bottomUi_.serverListWidget->count();
		bottomUi_.serverListWidget->insertItems (row - 1, newLines);
		row = bottomUi_.serverListWidget->count();
		bottomUi_.serverListWidget->setCurrentRow (row);
	}
	void
	View :: slotShowServerMessages (QString messages)
	{
		QStringList newLines = messages.split(QLatin1Char ('\n'), QString :: SkipEmptyParts);
		int row = bottomUi_.serverListWidget->count();
		bottomUi_.serverListWidget->insertItems (row, newLines);
		row = bottomUi_.serverListWidget->count();
		bottomUi_.serverListWidget->setCurrentRow (row - 1);
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
	View :: showErrors (const int exitCode, QProcess :: ExitStatus exitStatus)
	{	
		bottomUi_.qtabwidget->setCurrentIndex (0);
		bottomUi_.errTreeWidget->resizeColumnToContents (0);
		bottomUi_.errTreeWidget->resizeColumnToContents (1);
		bottomUi_.errTreeWidget->resizeColumnToContents (2);
		bottomUi_.errTreeWidget->horizontalScrollBar()->setValue (0);
		//bottomUi_.errTreeWidget->setSortingEnabled(true);
		mainWindow_->showToolView(toolView_);

		QStringList messsages;
		if ((errorParser_->numErrors() > 0) || (errorParser_->numWarnings() > 0)) {
			if (errorParser_->numErrors() > 0) {
				messsages << i18np ("Found one error.", "Found %1 errors.", errorParser_->numErrors());
			}
			if (errorParser_->numWarnings()) {
				messsages << i18np ("Found one warning.", "Found %1 warnings.", errorParser_->numWarnings());
			}
		} else {
			if (exitStatus == QProcess :: NormalExit) {
				messsages << i18n ("Exited with exitCode = %1", exitCode);
			} else {
				messsages << i18n ("Exited with crash status and exitCode = %1", exitCode);
			}
		}
		//KPassivePopup :: message (i18n ("Failure"), messsages.join (QStringLiteral("\n")), toolView_);
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
	void 
	View :: openLocation()
	{
		openLocation(output_);
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

		action = actionCollection()->addAction (QLatin1String("goto_next"));
		action->setText (i18n ("Next Error"));
		//a->setShortcut (QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_Right));
		connect (action, SIGNAL (triggered (bool)), this, SLOT (slotNext()));

		action = actionCollection()->addAction (QLatin1String("goto_prev"));
		action->setText (i18n ("Previous Error"));
		//a->setShortcut (QKeySequence (Qt :: CTRL + Qt :: ALT + Qt :: Key_Left));
		connect (action, SIGNAL (triggered (bool)), this, SLOT (slotPrev()));

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
		action->setIcon (QIcon (QLatin1String("arrow-right-double")));
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
		connect
		(
			bottomUi_.executeButton,
			SIGNAL (pressed()),
			client_,
			SLOT (executeCommand())
		);

		bottomUi_.outputTextEdit->setReadOnly (true);
	}
	void
	View :: initLauncher()
	{
		//connect (client_->process(), SIGNAL (finished (int, QProcess::ExitStatus)), this, SLOT (slotProcessExited (const int, QProcess::ExitStatus)));
		//connect (client_->process(), SIGNAL (readyReadStandardError()), this, SLOT (slotReadOutputStdErr()));
		//connect (client_->process(), SIGNAL (readyReadStandardOutput()), this, SLOT (slotReadOutputStdOut()));

		connect (&mdl::Server::process(), SIGNAL (readyReadStandardError()), this, SLOT (slotReadServerStdErr()));
		connect (&mdl::Server::process(), SIGNAL (readyReadStandardOutput()), this, SLOT (slotReadServerStdOut()));
		connect (client_, SIGNAL (showServerMessages(QString)), this, SLOT (slotShowServerMessages(QString)));

		connect (proof_, SIGNAL (proofFound(int)), this, SLOT (slotConfirmProof(int)));

		//connect (mainWindow_->activeView(), SIGNAL (viewChanged()), this, SLOT (refreshOutline()));
	}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_VIEW_CPP_ */


