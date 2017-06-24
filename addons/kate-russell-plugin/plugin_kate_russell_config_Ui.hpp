/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Ui.hpp                        */
/* Description:     a user interface for a Russell support plugin for Kate   */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_CONFIG_UI_HPP_
#define PLUGIN_KATE_RUSSELL_CONFIG_UI_HPP_

#include "plugin_kate_russell.dpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

class Ui : public QWidget {
Q_OBJECT
public:
	Ui
	(
		View* view,
		QWidget* parent,
		Config* config,
		Combinations* combinations
	);
	virtual ~ Ui();

	void synchronize();

protected:
	void resizeEvent (QResizeEvent*);

private Q_SLOTS:
	void combinationActivated (int);
	void optionsActivated (int);

	void addCombination();
	void addSourceRoot();
	void addTargetRoot();
	void addSourceUniverse();
	void addProver();
	void addVerifier();
	void addProveOption();
	void addTranslateOption();
	void addVerifyOption();
	void addLookupOption();
	void addLearnOption();
	void addServerHost();
	void addServerPort();

	void delCombination();
	void delSourceRoot();
	void delTargetRoot();
	void delSourceUniverse();
	void delProver();
	void delVerifier();
	void delProveOption();
	void delTranslateOption();
	void delVerifyOption();
	void delLookupOption();
	void delLearnOption();
	void delServerHost();
	void delServerPort();
    
	void editCombination (const QString&);
	void editSourceRoot (const QString&);
	void editTargetRoot (const QString&);
	void editSourceUniverse (const QString&);
	void editProver (const QString&);
	void editVerifier (const QString&);
	void editProveOption (const QString&);
	void editTranslateOption (const QString&);
	void editVerifyOption (const QString&);
	void editLookupOption (const QString&);
	void editLearnOption (const QString&);
	void editServerHost (const QString&);
	void editServerPort (const QString&);

	void browseSourceRoot();
	void browseTargetRoot();
	void browseSourceUniverse();
	void browseProver();
	void browseVerify();
    
private:
	friend class Combinations;
	friend class Config;
	friend class Indexes;

	void blockSignals (const bool value);
	void setIcons();
	void setTooltips();
	void setLabels();
	void setEditable();
	void setInsertPolicy();
	void connectSignals();

	void setBottomLayout();
	//void setSideLayout();

	View*   view_;
	Config* config_;
	Combinations* combs_;

	// Combo boxes
	KComboBox* combinations_;

	KComboBox* sourceRoots_;
	KComboBox* targetRoots_;
	KComboBox* sourceUniverses_;
	KComboBox* provers_;
	KComboBox* verifiers_;

	KComboBox* proveOptions_;
	KComboBox* translateOptions_;
	KComboBox* verifyOptions_;
	KComboBox* lookupOptions_;
	KComboBox* learnOptions_;

	KComboBox* serverHost_;
	KComboBox* serverPort_;

	// browse buttons
	QToolButton* sourceRootBrowse_;
	QToolButton* targetRootBrowse_;
	QToolButton* sourceUniverseBrowse_;
	QToolButton* proverBrowse_;
	QToolButton* verifyBrowse_;

	// Labels
	QLabel *combinationLabel_;

	QLabel *sourceRootLabel_;
	QLabel *targetRootLabel_;
	QLabel *sourceUniverseLabel_;
	QLabel *proverLabel_;
	QLabel *verifierLabel_;

	QLabel *proveOptionLabel_;
	QLabel *translateOptionLabel_;
	QLabel *verifyOptionLabel_;
	QLabel *lookupOptionLabel_;
	QLabel *learnOptionLabel_;

	QLabel *serverHostLabel_;
	QLabel *serverPortLabel_;

	// Buttons
	QToolButton* addCombination_;
	QToolButton* delCombination_;

	QToolButton* addSourceRoot_;
	QToolButton* delSourceRoot_;
	QToolButton* addTargetRoot_;
	QToolButton* delTargetRoot_;
	QToolButton* addSourceUniverse_;
	QToolButton* delSourceUniverse_;
	QToolButton* addProver_;
	QToolButton* delProver_;
	QToolButton* addVerifier_;
	QToolButton* delVerifier_;

	QToolButton* addProveOption_;
	QToolButton* delProveOption_;
	QToolButton* addTranslateOption_;
	QToolButton* delTranslateOption_;
	QToolButton* addVerifyOption_;
	QToolButton* delVerifyOption_;
	QToolButton* addLookupOption_;
	QToolButton* delLookupOption_;
	QToolButton* addLearnOption_;
	QToolButton* delLearnOption_;
	
	QToolButton* addServerHost_;
	QToolButton* delServerHost_;
	QToolButton* addServerPort_;
	QToolButton* delServerPort_;

	// Lines
	QFrame* verticalLine1_;
	QFrame* horizontalLine1_;
	QFrame* verticalLine2_;
	QFrame* horizontalLine2_;

	int  widgetsHeight_;
	bool useBottomLayout_;
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_CONFIG_UI_HPP_ */

