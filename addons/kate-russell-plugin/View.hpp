/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_View.hpp                             */
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

#pragma once

#include <QProcess>
#include <QPointer>

#include <KXMLGUIClient>
#include <KActionMenu>

#include <KTextEditor/MainWindow>
#include <KTextEditor/SessionConfigInterface>

#include "ui_BottomTab.h"
#include "ErrorParser.hpp"

namespace russell {

class Outline;
class Structure;
class TypeSystem;
class Proof;
class Client;
class Server;

class View : public QObject, public KXMLGUIClient {
Q_OBJECT
public:
	enum State_ {
		WAITING,
		PROVING,
		PROVING_INTERACTIVELY,
		TRANSLATING,
		VERIFYING,
		LEARNING,
		LOOKING_DEFINITION,
		OPENING_DEFINITION,
		MINING_OUTLINE,
		MINING_STRUCTURE,
		MINING_TYPE_SYSTEM
	};

	View (KTextEditor::MainWindow*, KTextEditor::Plugin*);
	virtual ~ View();

	State_ getState() const;
	Ui :: Bottom& getBottomUi();
	QWidget* toolView() const;
	void clearOutput();
	void openLocation(const QString&);

	Client* client();
	Server* server();
	Proof* proof();
	KTextEditor::Plugin* plugin() { return plugin_; }
	KTextEditor::MainWindow* mainWindow() const { return mainWindow_; }

	void setOutput(const QString& output) { output_ = output; }
	const QString& getOutput() const;
	void mineOutline (const QString&);
	void mineStructure (const QString&);
	void mineTypeSystem (const QString&);
	void gotoLocation (const QString&, const int, const int);

	QUrl currentFileUrl (const bool save = false) const;
	bool currentIsRussell() const;
	bool currentIsMetamath() const;

private Q_SLOTS:
	void slotCommandCompleted(quint32 code, const QString& msg, const QString& data);
	void slotRefreshOutline();
	
	// selecting warnings
	void slotItemSelected (QTreeWidgetItem* item);
	void slotNext();
	void slotPrev();

	// prove slots
	void slotProveVerify();
	void slotProve();
	void slotProveInteractive();
	void slotTranslate();
	void slotVerify();
	void slotLearn();
	void slotConfirmProof(int);

	void proveIdAutomatically();
	void proveIdInteractively();

	// server slots
	void slotManageServer();

	// popup menu
	void showMenu();
	void lookupDefinition();
	void openDefinition();
	void latexToUnicode();

	// launcher output slots
//	void slotProcessExited (const int, QProcess :: ExitStatus);
//	void slotReadOutputStdErr (const bool = false);
//	void slotReadOutputStdOut (const bool = false);

	// server output slots
	void slotReadRussellStdErr();
	void slotReadRussellStdOut();
	void slotReadMetamathStdErr();
	void slotReadMetamathStdOut();

	// client output slots
	void slotShowServerMessages(QString);

private:
	void showBuffer (const bool = false);
	void showErrors (const int, QProcess :: ExitStatus);
	void reloadSource();
	QString currentIdentifier() const;
	QString currentLatexExpression (int&, int&, int&) const;
	bool checkLocal (const QUrl &url) const;

	void initPopupMenu();
	void initActions();
	void initBottomUi();
	void initLauncher();

	KTextEditor::Plugin*     plugin_;
	KTextEditor::MainWindow* mainWindow_;
	QWidget*     toolView_;
	Ui :: Bottom bottomUi_;

	QPointer<KActionMenu> popupMenu_;
	QAction* lookupDefinition_;
	QAction* openDefinition_;
	QAction* latexToUnicode_;
	QAction* proveAutomatically_;
	QAction* proveInteractive_;

	Outline*    outline_;
	Structure*  structure_;
	TypeSystem* typeSystem_;
	Proof*      proof_;
	Client*     client_;

	ErrorParser* errorParser_;
	QString      output_;
	QString      outputBuffer_;
	State_       state_;
};

}
