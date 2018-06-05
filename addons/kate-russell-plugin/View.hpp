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

#include "Enums.hpp"
#include "ui_BottomTab.h"

namespace russell {

class Outline;
class Structure;
class TypeSystem;
class Proof;
class Server;

class View : public QObject, public KXMLGUIClient {
Q_OBJECT
public:
	View (KTextEditor::MainWindow*, KTextEditor::Plugin*);
	virtual ~ View();

	static View* get() { return instance_; }

	State getState() const;
	Ui :: Bottom& getBottomUi();
	QWidget* toolView() const;
	void clearOutput();
	void openLocation(const QString&);

	Server* server();
	Proof* proof();
	KTextEditor::Plugin* plugin() { return plugin_; }
	KTextEditor::MainWindow* mainWindow() const { return mainWindow_; }

	const QString& getOutput() const;
	void mineOutline (const QString&);
	void mineStructure (const QString&);
	void mineTypeSystem (const QString&);
	void gotoLocation (const QString&, const int, const int);

	QUrl currentFileUrl (const bool save = false) const;
	QString currentFile (const bool save = false) const;
	Lang currentFileType() const;
	bool currentIsRus() const;
	bool currentIsMm() const;

private Q_SLOTS:
	void slotRead(KTextEditor::View*);
	void slotDocumentSaved(KTextEditor::Document*, bool);

	void slotRusCommandCompleted(quint32 code, const QString& msg, const QString& data);
	void slotMmCommandCompleted();
	void slotRefreshOutline();

	// prove slots
	//void slotProveVerify();
	//void slotProve();
	//void slotProveInteractive();
	void slotVerify();
	void slotTranslate();
	//void slotConfirmProof(int);

	//void proveIdAutomatically();
	//void proveIdInteractively();

	// server slots
	void slotManageServer();

	// popup menu
	void showMenu();
	void lookupDefinition();
	void openDefinition();
	void latexToUnicode();

	// server output slots
	void slotReadRussellStdErr();
	void slotReadRussellStdOut();
	void slotReadMetamathStdErr();
	void slotReadMetamathStdOut();

	// execute custom command
	void slotExecuteRussellCommand();
	void slotExecuteMetamathCommand();

Q_SIGNALS:
    void mmCommandFinished();

private:
    void openFile(const QString& file);
	void showBuffer (const bool = false);
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

	static View* instance_;

	struct InternalState {
		InternalState() : state(State::WAITING) { }
		InternalState(State s, const QString& f) : state(s), file(f) { }
		State state;
		QString file;
	};

	struct StateKeeper {
		InternalState get() const {
			return internal_state_;
		}
		bool start(State s, const QString& f) {
			if (internal_state_.state != State::WAITING) return false;
			internal_state_ = InternalState(s, f);
			return true;
		}
		void stop() {
			internal_state_ = InternalState();
		}
	private:
		InternalState internal_state_;
	};
	StateKeeper state_;
};

}

