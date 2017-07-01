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

#ifndef PLUGIN_KATE_RUSSELL_VIEW_HPP_
#define PLUGIN_KATE_RUSSELL_VIEW_HPP_

#include "ui_BottomTab.h"
#include <ktexteditor/sessionconfiginterface.h>

#include "russell.dpp"

namespace plugin {
namespace kate {
namespace russell {

class View : public QObject, public KXMLGUIClient, public KTextEditor::SessionConfigInterface /*public KTextEditor::View , public KXMLGUIClient */ {
Q_OBJECT
Q_INTERFACES(KTextEditor::SessionConfigInterface)
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

	mdl :: Launcher* launcher();
	mdl :: Client*   client();
	mdl :: Server*   server();
	Proof* proof();
	KTextEditor::Plugin* plugin() { return plugin_; }
	KTextEditor::MainWindow* mainWindow() const { return mainWindow_; }

	const QString& getOutput() const;
	const Config* getConfig() const;
	void mineOutline (const QString&);
	void mineStructure (const QString&);
	void mineTypeSystem (const QString&);
	void gotoLocation (const QString&, const int, const int);

	QUrl currentFileUrl (const bool save = false) const;
	bool currentIsRussell() const;
	bool currentIsMetamath() const;

	// overwritten: read and write session config
	void readSessionConfig (const KConfigGroup& config) Q_DECL_OVERRIDE;
	void writeSessionConfig (KConfigGroup& config) Q_DECL_OVERRIDE;

private Q_SLOTS:
	void refreshOutline();
	
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
	void slotStop();
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
	void slotProcessExited (const int, QProcess :: ExitStatus);
	void slotReadOutputStdErr (const bool = false);
	void slotReadOutputStdOut (const bool = false);

	// server output slots
	void slotReadServerStdErr();
	void slotReadServerStdOut();

	// client output slots
	void slotShowServerMessages(QString);

private:
	void showBuffer (const bool = false);
	void showErrors (const int, QProcess :: ExitStatus);
	void reloadSource();
	void openLocation();
	QString currentIdentifier() const;
	QString currentLatexExpression (int&, int&, int&) const;
	bool checkLocal (const QUrl &url) const;

	void initPopupMenu();
	void initActions();
	void initBottomUi();
	void initLauncher();

	KTextEditor::Plugin*     plugin_;
	KTextEditor::MainWindow* mainWindow_;
	QWidget*            toolView_;
	Ui :: Bottom        bottomUi_;

	QPointer<KActionMenu> popupMenu_;
	QAction* lookupDefinition_;
	QAction* openDefinition_;
	QAction* latexToUnicode_;
	QAction* proveAutomatically_;
	QAction* proveInteractive_;

	Config* config_;
	config :: Combinations* combinations_;
	config :: Ui* configUi_;

	Outline*     outline_;
	Structure*   structure_;
	TypeSystem*  typeSystem_;
	Proof*       proof_;

	mdl :: Launcher* launcher_;
	mdl :: Client*   client_;

	ErrorParser* errorParser_;
	QString      output_;
	QString      outputBuffer_;
	State_       state_;
	bool         chain_;
};

}
}
}

#endif /* PLUGIN_KATE_RUSSELL_VIEW_HPP_ */

