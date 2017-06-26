/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Ui.cpp                        */
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

#ifndef PLUGIN_KATE_PROVE_CONFIG_UI_CPP_
#define PLUGIN_KATE_PROVE_CONFIG_UI_CPP_

#include "Ui.moc"
#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

	/****************************
	 *	Public members
	 ****************************/

	Ui :: Ui
	(
		View* view,
		QWidget* parent,
		Config* config,
		Combinations* combs
	):
	QWidget (parent),
	view_ (view),
	config_ (config),
	combs_ (combs),

	// KComboBox section
	combinations_ (new KComboBox(this)),
	sourceRoots_ (new KComboBox(this)),
	targetRoots_ (new KComboBox(this)),
	sourceUniverses_ (new KComboBox(this)),
	provers_ (new KComboBox(this)),
	verifiers_ (new KComboBox(this)),
	proveOptions_ (new KComboBox(this)),
	translateOptions_ (new KComboBox(this)),
	verifyOptions_ (new KComboBox(this)),
	lookupOptions_ (new KComboBox(this)),
	learnOptions_ (new KComboBox(this)),
	serverHost_ (new KComboBox(this)),
	serverPort_ (new KComboBox(this)),

	// Browse section
	sourceRootBrowse_ (new QToolButton(this)),
	targetRootBrowse_ (new QToolButton(this)),
	sourceUniverseBrowse_ (new QToolButton(this)),
	proverBrowse_ (new QToolButton(this)),
	verifyBrowse_ (new QToolButton(this)),

	// Label section
	combinationLabel_ (new QLabel (i18n ("Combination"), this)),
	sourceRootLabel_ (new QLabel (i18n ("Source dir"), this)),
	targetRootLabel_ (new QLabel (i18n ("Target dir"), this)),
	sourceUniverseLabel_ (new QLabel (i18n ("Source universe"), this)),
	proverLabel_ (new QLabel (i18n ("Prover"), this)),
	verifierLabel_ (new QLabel (i18n ("Verifier"), this)),
	proveOptionLabel_ (new QLabel (i18n ("Prove"), this)),
	translateOptionLabel_ (new QLabel (i18n ("Translate"), this)),
	verifyOptionLabel_ (new QLabel (i18n ("Verify"), this)),
	lookupOptionLabel_ (new QLabel (i18n ("Lookup"), this)),
	learnOptionLabel_ (new QLabel (i18n ("Learn"), this)),
	serverHostLabel_ (new QLabel (i18n ("Server host"), this)),
	serverPortLabel_ (new QLabel (i18n ("Server port"), this)),

	// add / del Button section
	addCombination_ (new QToolButton(this)),
	delCombination_ (new QToolButton(this)),

	addSourceRoot_ (new QToolButton(this)),
	delSourceRoot_ (new QToolButton(this)),
	addTargetRoot_ (new QToolButton(this)),
	delTargetRoot_ (new QToolButton(this)),
	addSourceUniverse_ (new QToolButton(this)),
	delSourceUniverse_ (new QToolButton(this)),
	addProver_ (new QToolButton(this)),
	delProver_ (new QToolButton(this)),
	addVerifier_ (new QToolButton(this)),
	delVerifier_ (new QToolButton(this)),

	addProveOption_ (new QToolButton(this)),
	delProveOption_ (new QToolButton(this)),
	addTranslateOption_ (new QToolButton(this)),
	delTranslateOption_ (new QToolButton(this)),
	addVerifyOption_ (new QToolButton(this)),
	delVerifyOption_ (new QToolButton(this)),
	addLookupOption_ (new QToolButton(this)),
	delLookupOption_ (new QToolButton(this)),
	addLearnOption_ (new QToolButton(this)),
	delLearnOption_ (new QToolButton(this)),

	addServerHost_ (new QToolButton(this)),
	delServerHost_ (new QToolButton(this)),
	addServerPort_ (new QToolButton(this)),
	delServerPort_ (new QToolButton(this)),

	// Lines section
	verticalLine1_ (new QFrame (this)),
	horizontalLine1_ (new QFrame (this)),
	verticalLine2_ (new QFrame (this)),
	horizontalLine2_ (new QFrame (this)),

	widgetsHeight_ (sizeHint().height()),
	useBottomLayout_ (true)
	{
		#ifdef DEBUG_CREATE_DESTROY
		std :: cout << "Ui :: Ui (...)" << std :: endl;
		#endif

		setIcons();
		connectSignals();
		setTooltips();
		setLabels();
		setEditable();
		setInsertPolicy();

		QCompleter* dirCompleter = new QCompleter (this);
		QStringList filter;
		dirCompleter->setModel (new QDirModel (filter, QDir :: AllDirs | QDir :: NoDotAndDotDot, QDir :: Name, this));
		sourceRoots_->setCompleter (dirCompleter);
		targetRoots_->setCompleter (dirCompleter);
		sourceUniverses_->setCompleter (dirCompleter);
		provers_->setCompleter (dirCompleter);
		verifiers_->setCompleter (dirCompleter);

		// calculate the approximate height to exceed before going to "Side Layout"
		//setSideLayout();
		/*widgetsHeight_ = sizeHint().height();
		delete layout();*/
		setBottomLayout();
	}
	Ui :: ~ Ui()
	{
		#ifdef DEBUG_CREATE_DESTROY
		std :: cout << "Ui :: ~ Ui()" << std :: endl;
		#endif

		// Combo boxes section
		delete combinations_;

		delete sourceRoots_;
		delete targetRoots_;
		delete sourceUniverses_;
		delete provers_;
		delete verifiers_;

		delete proveOptions_;
		delete translateOptions_;
		delete verifyOptions_;
		delete lookupOptions_;
		delete learnOptions_;

		// browse buttnos section
		delete sourceRootBrowse_;
		delete targetRootBrowse_;
		delete sourceUniverseBrowse_;
		delete proverBrowse_;
		delete verifyBrowse_;

		// label section
		delete combinationLabel_;
		delete sourceRootLabel_;
		delete targetRootLabel_;
		delete sourceUniverseLabel_;
		delete proverLabel_;
		delete verifierLabel_;
		delete proveOptionLabel_;
		delete translateOptionLabel_;
		delete verifyOptionLabel_;
		delete lookupOptionLabel_;

		// add / del buttons
		delete addCombination_;
		delete delCombination_;

		delete addSourceRoot_;
		delete delSourceRoot_;
		delete addTargetRoot_;
		delete delTargetRoot_;
		delete addSourceUniverse_;
		delete delSourceUniverse_;
		delete addProver_;
		delete delProver_;
		delete addVerifier_;
		delete delVerifier_;

		delete addProveOption_;
		delete delProveOption_;
		delete addTranslateOption_;
		delete delTranslateOption_;
		delete addVerifyOption_;
		delete delVerifyOption_;
		delete addLookupOption_;
		delete delLookupOption_;
		delete addLearnOption_;
		delete delLearnOption_;

		delete addServerHost_;
		delete delServerHost_;
		delete addServerPort_;
		delete delServerPort_;

		// Lines section
		delete verticalLine1_;
		delete horizontalLine1_;
		delete verticalLine2_;
		delete horizontalLine2_;
	}

	void 
	Ui :: synchronize()
	{
		blockSignals (true);

		// Combo boxes
		combinations_->clear();
		combinations_->addItems (combs_->combNames_);
		combinations_->setCurrentIndex (combs_->index_);

		sourceRoots_->clear();
		sourceRoots_->addItems (config_->sourceRootList_);
		sourceRoots_->setCurrentIndex (config_->indexes_->sourceRootIndex_);
		targetRoots_->clear();
		targetRoots_->addItems (config_->targetRootList_);
		targetRoots_->setCurrentIndex (config_->indexes_->targetRootIndex_);
		sourceUniverses_->clear();
		sourceUniverses_->addItems (config_->sourceUniverseList_);
		sourceUniverses_->setCurrentIndex (config_->indexes_->sourceUniverseIndex_);
		provers_->clear();
		provers_->addItems (config_->proverList_);
		provers_->setCurrentIndex (config_->indexes_->proverIndex_);
		verifiers_->clear();
		verifiers_->addItems (config_->verifierList_);
		verifiers_->setCurrentIndex (config_->indexes_->verifierIndex_);

		proveOptions_->clear();
		proveOptions_->addItems (config_->proveOptionList_);
		proveOptions_->setCurrentIndex (config_->indexes_->proveOptionIndex_);
		translateOptions_->clear();
		translateOptions_->addItems (config_->translateOptionList_);
		translateOptions_->setCurrentIndex (config_->indexes_->translateOptionIndex_);
		verifyOptions_->clear();
		verifyOptions_->addItems (config_->verifyOptionList_);
		verifyOptions_->setCurrentIndex (config_->indexes_->verifyOptionIndex_);
		lookupOptions_->clear();
		lookupOptions_->addItems (config_->lookupOptionList_);
		lookupOptions_->setCurrentIndex (config_->indexes_->lookupOptionIndex_);
		learnOptions_->clear();
		learnOptions_->addItems (config_->learnOptionList_);
		learnOptions_->setCurrentIndex (config_->indexes_->learnOptionIndex_);

		serverHost_->clear();
		serverHost_->addItems (config_->serverHostList_);
		serverHost_->setCurrentIndex (config_->indexes_->serverHostIndex_);
		serverPort_->clear();
		serverPort_->addItems (config_->serverPortList_);
		serverPort_->setCurrentIndex (config_->indexes_->serverPortIndex_);

		delCombination_->setDisabled (combinations_->count() <= 1);
		delSourceRoot_->setDisabled (sourceRoots_->count() <= 1);
		delTargetRoot_->setDisabled (targetRoots_->count() <= 1);
		delSourceUniverse_->setDisabled (sourceUniverses_->count() <= 1);
		delProver_->setDisabled (provers_->count() <= 1);
		delVerifier_->setDisabled (verifiers_->count() <= 1);
		delProveOption_->setDisabled (proveOptions_->count() <= 1);
		delTranslateOption_->setDisabled (translateOptions_->count() <= 1);
		delVerifyOption_->setDisabled (verifyOptions_->count() <= 1);
		delLookupOption_->setDisabled (lookupOptions_->count() <= 1);
		delLearnOption_->setDisabled (learnOptions_->count() <= 1);

		delServerHost_->setDisabled (serverHost_->count() <= 1);
		delServerPort_->setDisabled (serverPort_->count() <= 1);

		blockSignals (false);
	}

	/****************************
	 *	Protected members
	 ****************************/

	void 
	Ui :: resizeEvent (QResizeEvent *)
	{
		// check if the widgets fit in a VBox layout
		/*if (useBottomLayout_ && (size().height() > widgetsHeight_)) {
			delete layout();
			setSideLayout();
			useBottomLayout_ = false;
		} else if (!useBottomLayout_ && (size().height() < widgetsHeight_)) {
			delete layout();
			setBottomLayout();
			useBottomLayout_ = true;
		}*/
	}

	/*******************************
	 *	Private slot members
	 *******************************/

	// Add item slots
	void
	Ui :: combinationActivated (int index)
	{
		#ifdef DEBUG_CONFIG
		std :: cout << "Ui :: combinationActivated()" << std :: endl;
		#endif
		combs_->choose (index);
		config_->indexes() = combs_->combination();
		synchronize();
	}
	void
	Ui :: optionsActivated (int)
	{
		#ifdef DEBUG_CONFIG
		std :: cout << "Ui :: optionsActivated()" << std :: endl;
		#endif
		combs_->combination() = config_->indexes();
	}

	void
	Ui :: addCombination()
	{
		combinations_->addItem (i18n("new combination"));
		combinations_->setCurrentIndex (sourceRoots_->count() - 1);
		delCombination_->setDisabled (false);
		combs_->synchronize (true);
	}
	void
	Ui :: addSourceRoot()
	{
		const int index = sourceRoots_->currentIndex();
		const QString option = sourceRoots_->itemText (index);
		sourceRoots_->addItem (option);
		sourceRoots_->setCurrentIndex (sourceRoots_->count() - 1);
		delSourceRoot_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addTargetRoot()
	{
		const int index = targetRoots_->currentIndex();
		const QString option = targetRoots_->itemText (index);
		targetRoots_->addItem (option);
		targetRoots_->setCurrentIndex (targetRoots_->count() - 1);
		delTargetRoot_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addSourceUniverse()
	{
		const int index = sourceUniverses_->currentIndex();
		const QString option = sourceUniverses_->itemText (index);
		sourceUniverses_->addItem (option);
		sourceUniverses_->setCurrentIndex (sourceUniverses_->count() - 1);
		delSourceUniverse_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addProver()
	{
		const int index = provers_->currentIndex();
		const QString option = provers_->itemText (index);
		provers_->addItem (option);
		provers_->setCurrentIndex (provers_->count() - 1);
		delProver_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addVerifier()
	{
		const int index = verifiers_->currentIndex();
		const QString option = verifiers_->itemText (index);
		verifiers_->addItem (option);
		verifiers_->setCurrentIndex (verifiers_->count() - 1);
		delVerifier_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addProveOption()
	{
		const int index = proveOptions_->currentIndex();
		const QString option = proveOptions_->itemText (index);
		proveOptions_->addItem (option);
		proveOptions_->setCurrentIndex (proveOptions_->count() - 1);
		delProveOption_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addTranslateOption()
	{
		const int index = translateOptions_->currentIndex();
		const QString option = translateOptions_->itemText (index);
		translateOptions_->addItem (option);
		translateOptions_->setCurrentIndex (translateOptions_->count() - 1);
		delTranslateOption_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addVerifyOption()
	{
		const int index = verifyOptions_->currentIndex();
		const QString option = verifyOptions_->itemText (index);
		verifyOptions_->addItem (option);
		verifyOptions_->setCurrentIndex (verifyOptions_->count() - 1);
		delVerifyOption_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addLookupOption()
	{
		const int index = lookupOptions_->currentIndex();
		const QString option = lookupOptions_->itemText (index);
		lookupOptions_->addItem (option);
		lookupOptions_->setCurrentIndex (lookupOptions_->count() - 1);
		delLookupOption_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addLearnOption()
	{
		const int index = learnOptions_->currentIndex();
		const QString option = learnOptions_->itemText (index);
		learnOptions_->addItem (option);
		learnOptions_->setCurrentIndex (learnOptions_->count() - 1);
		delLearnOption_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addServerHost()
	{
		const int index = serverHost_->currentIndex();
		const QString option = serverHost_->itemText (index);
		serverHost_->addItem (option);
		serverHost_->setCurrentIndex (serverHost_->count() - 1);
		delServerHost_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: addServerPort()
	{
		const int index = serverPort_->currentIndex();
		const QString option = serverPort_->itemText (index);
		serverPort_->addItem (option);
		serverPort_->setCurrentIndex (serverPort_->count() - 1);
		delServerPort_->setDisabled (false);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}

	// Del item slots
	void
	Ui :: delCombination()
	{
		combinations_->removeItem (combinations_->currentIndex());
		delCombination_->setDisabled (combinations_->count() == 1);
		combs_->synchronize (false);
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delSourceRoot()
	{
		sourceRoots_->removeItem (sourceRoots_->currentIndex());
		delSourceRoot_->setDisabled (sourceRoots_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delTargetRoot()
	{
		targetRoots_->removeItem (targetRoots_->currentIndex());
		delTargetRoot_->setDisabled (targetRoots_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delSourceUniverse()
	{
		sourceUniverses_->removeItem (sourceUniverses_->currentIndex());
		delSourceUniverse_->setDisabled (sourceUniverses_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delProver()
	{
		provers_->removeItem(provers_->currentIndex());
		delProver_->setDisabled (provers_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delVerifier()
	{
		verifiers_->removeItem (verifiers_->currentIndex());
		delVerifier_->setDisabled (verifiers_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delProveOption()
	{
		proveOptions_->removeItem (proveOptions_->currentIndex());
		delProveOption_->setDisabled (proveOptions_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delTranslateOption()
	{
		translateOptions_->removeItem (translateOptions_->currentIndex());
		delTranslateOption_->setDisabled (translateOptions_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delVerifyOption()
	{
		verifyOptions_->removeItem(verifyOptions_->currentIndex());
		delVerifyOption_->setDisabled (verifyOptions_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delLookupOption()
	{
		lookupOptions_->removeItem (lookupOptions_->currentIndex());
		delLookupOption_->setDisabled (lookupOptions_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delLearnOption()
	{
		learnOptions_->removeItem (learnOptions_->currentIndex());
		delLearnOption_->setDisabled (learnOptions_->count() == 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delServerHost()
	{
		serverHost_->removeItem (serverHost_->currentIndex());
		delServerHost_->setDisabled (serverHost_->count() <= 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: delServerPort()
	{
		serverPort_->removeItem (serverPort_->currentIndex());
		delServerPort_->setDisabled (serverPort_->count() <= 1);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}

	// Edit text slots
	void
	Ui :: editCombination (const QString& text)
	{
		const int currentPos = combinations_->lineEdit()->cursorPosition();
		combinations_->setItemText (combinations_->currentIndex(), text);
		combinations_->lineEdit()->setCursorPosition (currentPos);
		combs_->updateName (combinations_->currentIndex());
	}
	void
	Ui :: editSourceRoot (const QString& text)
	{
		const int currentPos = sourceRoots_->lineEdit()->cursorPosition();
		const int currentIndex = sourceRoots_->currentIndex();
		sourceRoots_->setItemText (currentIndex, text);
		sourceRoots_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: editTargetRoot (const QString& text)
	{
		const int currentPos = targetRoots_->lineEdit()->cursorPosition();
		const int currentIndex = targetRoots_->currentIndex();
		targetRoots_->setItemText (currentIndex, text);
		targetRoots_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: editSourceUniverse (const QString& text)
	{
		const int currentPos = sourceUniverses_->lineEdit()->cursorPosition();
		const int currentIndex = sourceUniverses_->currentIndex();
		sourceUniverses_->setItemText (currentIndex, text);
		sourceUniverses_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: editProver (const QString& text)
	{
		const int currentPos = provers_->lineEdit()->cursorPosition();
		const int currentIndex = provers_->currentIndex();
		provers_->setItemText (currentIndex, text);
		provers_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: editVerifier (const QString& text)
	{
		const int currentPos = verifiers_->lineEdit()->cursorPosition();
		verifiers_->setItemText (verifiers_->currentIndex(), text);
		verifiers_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void 
	Ui :: editProveOption (const QString& text)
	{
		const int currentPos = proveOptions_->lineEdit()->cursorPosition();
		proveOptions_->setItemText (proveOptions_->currentIndex(), text);
		proveOptions_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void 
	Ui :: editTranslateOption (const QString& text)
	{
		const int currentPos = translateOptions_->lineEdit()->cursorPosition();
		translateOptions_->setItemText (translateOptions_->currentIndex(), text);
		translateOptions_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void 
	Ui :: editVerifyOption (const QString& text)
	{
		const int currentPos = verifyOptions_->lineEdit()->cursorPosition();
		verifyOptions_->setItemText (verifyOptions_->currentIndex(), text);
		verifyOptions_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void 
	Ui :: editLookupOption (const QString& text)
	{
		const int currentPos = lookupOptions_->lineEdit()->cursorPosition();
		lookupOptions_->setItemText (lookupOptions_->currentIndex(), text);
		lookupOptions_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: editLearnOption (const QString& text)
	{
		const int currentPos = learnOptions_->lineEdit()->cursorPosition();
		learnOptions_->setItemText (learnOptions_->currentIndex(), text);
		learnOptions_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: editServerHost (const QString& text)
	{
		const int currentPos = serverHost_->lineEdit()->cursorPosition();
		serverHost_->setItemText (serverHost_->currentIndex(), text);
		serverHost_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: editServerPort (const QString& text)
	{
		const int currentPos = serverPort_->lineEdit()->cursorPosition();
		serverPort_->setItemText (serverPort_->currentIndex(), text);
		serverPort_->lineEdit()->setCursorPosition (currentPos);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}

	// browse slots
	void
	Ui :: browseSourceRoot()
	{
		const int currentIndex = sourceRoots_->currentIndex();
		const QString currentDirectory = sourceRoots_->itemText (currentIndex);
		QUrl url (currentDirectory);
		if (currentDirectory.isEmpty()) {
			KTextEditor :: View* activeView = view_->mainWindow()->activeView();
			if (activeView != NULL) {
				url = activeView->document()->url();
			}
		}
		const QString newDirectory = QFileDialog :: getExistingDirectory (this, i18n("Source Root"), url.toString());
		sourceRoots_->setItemText (currentIndex, newDirectory);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: browseTargetRoot()
	{
		const int currentIndex = targetRoots_->currentIndex();
		const QString currentDirectory = targetRoots_->itemText (currentIndex);
		QUrl url (currentDirectory);
		if (currentDirectory.isEmpty()) {
			KTextEditor :: View* activeView = view_->mainWindow()->activeView();
			if (activeView != NULL) {
				url = activeView->document()->url();
			}
		}
		const QString newDirectory = QFileDialog :: getExistingDirectory (this, i18n("Target Root"), url.toString());
		targetRoots_->setItemText (currentIndex, newDirectory);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: browseSourceUniverse()
	{
		const int currentIndex = sourceUniverses_->currentIndex();
		const QString currentUniverse = sourceUniverses_->itemText (currentIndex);
		const QString newUniverse
		(
			currentUniverse.isEmpty() ?
			QFileDialog :: getOpenFileName (this, i18n("Source Universe"), Config :: defaultSourceUniverse_) :
			QFileDialog :: getOpenFileName (this, i18n("Source Universe"), currentUniverse)
		);
		sourceUniverses_->setItemText (currentIndex, newUniverse);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: browseProver()
	{
		const int currentIndex = provers_->currentIndex();
		const QString currentProver = provers_->itemText (currentIndex);
		const QString newProver
		(
			currentProver.isEmpty() ?
			QFileDialog :: getOpenFileName (this, i18n("Prover"), Config :: defaultProver_) :
			QFileDialog :: getOpenFileName (this, i18n("Prover"), currentProver)
		);
		provers_->setItemText (currentIndex, newProver);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}
	void
	Ui :: browseVerify()
	{
		const int currentIndex = verifiers_->currentIndex();
		const QString currentVerifier = verifiers_->itemText (currentIndex);
		const QString newVerifier
		(
			currentVerifier.isEmpty() ?
			QFileDialog :: getOpenFileName (this, i18n("Verifier"), Config :: defaultVerifier_) :
			QFileDialog :: getOpenFileName (this, i18n("Verifier"), currentVerifier)
		);
		verifiers_->setItemText (currentIndex, newVerifier);
		config_->synchronize();
		combs_->combination() = config_->indexes();
	}

	/****************************
	 *	Private members
	 ****************************/

	void
	Ui :: blockSignals (const bool value)
	{
		// Combo boxes
		combinations_->blockSignals (value);

		sourceRoots_->blockSignals (value);
		targetRoots_->blockSignals (value);
		sourceUniverses_->blockSignals (value);
		provers_->blockSignals (value);
		verifiers_->blockSignals (value);

		proveOptions_->blockSignals (value);
		translateOptions_->blockSignals (value);
		verifyOptions_->blockSignals (value);
		lookupOptions_->blockSignals (value);
		learnOptions_->blockSignals (value);

		serverHost_->blockSignals (value);
		serverPort_->blockSignals (value);

		// Browse buttons
		sourceRootBrowse_->blockSignals (value);
		targetRootBrowse_->blockSignals (value);
		proverBrowse_->blockSignals (value);
		verifyBrowse_->blockSignals (value);

		// add / del buttons
		addCombination_->blockSignals (value);
		delCombination_->blockSignals (value);

		addSourceRoot_->blockSignals (value);
		delSourceRoot_->blockSignals (value);
		addTargetRoot_->blockSignals (value);
		delTargetRoot_->blockSignals (value);
		addSourceUniverse_->blockSignals (value);
		delSourceUniverse_->blockSignals (value);
		addProver_->blockSignals (value);
		delProver_->blockSignals (value);
		addVerifier_->blockSignals (value);
		delVerifier_->blockSignals (value);

		addProveOption_->blockSignals (value);
		delProveOption_->blockSignals (value);
		addTranslateOption_->blockSignals (value);
		delTranslateOption_->blockSignals (value);
		addVerifyOption_->blockSignals (value);
		delVerifyOption_->blockSignals (value);
		addLookupOption_->blockSignals (value);
		delLookupOption_->blockSignals (value);
		addLearnOption_->blockSignals (value);
		delLearnOption_->blockSignals (value);

		addServerHost_->blockSignals (value);
		delServerHost_->blockSignals (value);
		addServerPort_->blockSignals (value);
		delServerPort_->blockSignals (value);
	}
	void
	Ui :: setIcons()
	{
		// Browse buttons
		sourceRootBrowse_->setIcon(QIcon(QStringLiteral("inode-directory")));
		targetRootBrowse_->setIcon(QIcon(QStringLiteral("inode-directory")));
		sourceUniverseBrowse_->setIcon(QIcon(QStringLiteral("inode-directory")));
		proverBrowse_->setIcon(QIcon(QStringLiteral("inode-directory")));
		verifyBrowse_->setIcon(QIcon(QStringLiteral("inode-directory")));

		// add / del buttons
		addCombination_->setIcon(QIcon(QStringLiteral("document-new")));
		delCombination_->setIcon(QIcon(QStringLiteral("edit-delete")));

		addSourceRoot_->setIcon(QIcon(QStringLiteral("document-new")));
		delSourceRoot_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addTargetRoot_->setIcon(QIcon(QStringLiteral("document-new")));
		delTargetRoot_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addSourceUniverse_->setIcon(QIcon(QStringLiteral("document-new")));
		delSourceUniverse_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addProver_->setIcon(QIcon(QStringLiteral("document-new")));
		delProver_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addVerifier_->setIcon(QIcon(QStringLiteral("document-new")));
		delVerifier_->setIcon(QIcon(QStringLiteral("edit-delete")));

		addProveOption_->setIcon(QIcon(QStringLiteral("document-new")));
		delProveOption_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addTranslateOption_->setIcon(QIcon(QStringLiteral("document-new")));
		delTranslateOption_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addVerifyOption_->setIcon(QIcon(QStringLiteral("document-new")));
		delVerifyOption_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addLookupOption_->setIcon(QIcon(QStringLiteral("document-new")));
		delLookupOption_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addLearnOption_->setIcon(QIcon(QStringLiteral("document-new")));
		delLearnOption_->setIcon(QIcon(QStringLiteral("edit-delete")));

		addServerHost_->setIcon(QIcon(QStringLiteral("document-new")));
		delServerHost_->setIcon(QIcon(QStringLiteral("edit-delete")));
		addServerPort_->setIcon(QIcon(QStringLiteral("document-new")));
		delServerPort_->setIcon(QIcon(QStringLiteral("edit-delete")));
	}
	void
	Ui :: setTooltips()
	{
		combinations_->setToolTip(i18n("Combination of options."));

		sourceRoots_->setToolTip(i18n("Enter the root directory for the concerned library."));
		targetRoots_->setToolTip(i18n("Enter the root directory for the concerned library."));
		sourceRoots_->setToolTip(i18n("Enter the universe file for the concerned library."));
		provers_->setToolTip(i18n("Enter a path to the mdl prover or leave 'mdl' for a system wide prover."));
		verifiers_->setToolTip(i18n("Enter a path to the mdl verifier or leave 'smm' for a system wide verifier."));

		proveOptions_->setToolTip (i18n("Use:\n\"%f\" for proved file"));
		translateOptions_->setToolTip (i18n("Use:\n\"%s\" for source file\n\"%t\" for target file"));
		verifyOptions_->setToolTip (i18n("Use:\n\"%f\" for verified file"));
		lookupOptions_->setToolTip (i18n("Use:\n\"%l\" for lookup line\n\"%c\" for lookup column"));
		learnOptions_->setToolTip (i18n("Use:\n\"%f\" for learning file\n\"%u\" for universe file"));

		serverHost_->setToolTip (i18n("server host"));
		serverPort_->setToolTip (i18n("server port"));
	}
	void
	Ui :: setLabels()
	{
		combinationLabel_->setBuddy (combinations_);

		sourceRootLabel_->setBuddy (sourceRoots_);
		targetRootLabel_->setBuddy (targetRoots_);
		sourceUniverseLabel_->setBuddy (sourceUniverses_);
		proverLabel_->setBuddy (provers_);
		verifierLabel_->setBuddy (verifiers_);

		proveOptionLabel_->setBuddy (proveOptions_);
		translateOptionLabel_->setBuddy (translateOptions_);
		verifyOptionLabel_->setBuddy (verifyOptions_);
		lookupOptionLabel_->setBuddy (lookupOptions_);
		learnOptionLabel_->setBuddy (learnOptions_);

		serverHostLabel_->setBuddy (serverHost_);
		serverPortLabel_->setBuddy (serverPort_);
	}
	void
	Ui :: setEditable()
	{
		combinations_->setEditable (true);

		sourceRoots_->setEditable (true);
		targetRoots_->setEditable (true);
		sourceUniverses_->setEditable (true);
		provers_->setEditable (true);
		verifiers_->setEditable (true);

		proveOptions_->setEditable (true);
		translateOptions_->setEditable (true);
		verifyOptions_->setEditable (true);
		lookupOptions_->setEditable (true);
		learnOptions_->setEditable (true);

		serverHost_->setEditable (true);
		serverPort_->setEditable (true);
	}
	void
	Ui :: setInsertPolicy()
	{
		sourceRoots_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		targetRoots_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		sourceUniverses_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		provers_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		verifiers_->setInsertPolicy (QComboBox :: InsertAtCurrent);

		proveOptions_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		translateOptions_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		verifyOptions_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		lookupOptions_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		learnOptions_->setInsertPolicy (QComboBox :: InsertAtCurrent);

		serverHost_->setInsertPolicy (QComboBox :: InsertAtCurrent);
		serverPort_->setInsertPolicy (QComboBox :: InsertAtCurrent);
	}
	void
	Ui :: connectSignals()
	{
		connect (addCombination_, SIGNAL(clicked()), this, SLOT (addCombination()));
		connect (delCombination_, SIGNAL(clicked()), this, SLOT (delCombination()));
		connect (combinations_, SIGNAL(editTextChanged(QString)), this, SLOT(editCombination(QString)));
		connect (combinations_, SIGNAL(activated(int)), this, SLOT(combinationActivated(int)));

		connect (addSourceRoot_, SIGNAL(clicked()), this, SLOT (addSourceRoot()));
		connect (delSourceRoot_, SIGNAL(clicked()), this, SLOT (delSourceRoot()));
		connect (sourceRoots_, SIGNAL(editTextChanged(QString)), this, SLOT(editSourceRoot(QString)));
		connect (sourceRoots_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addTargetRoot_, SIGNAL(clicked()), this, SLOT(addTargetRoot()));
		connect (delTargetRoot_, SIGNAL(clicked()), this, SLOT(delTargetRoot()));
		connect (targetRoots_, SIGNAL(editTextChanged(QString)), this, SLOT(editTargetRoot(QString)));
		connect (targetRoots_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addSourceUniverse_, SIGNAL(clicked()), this, SLOT (addSourceUniverse()));
		connect (delSourceUniverse_, SIGNAL(clicked()), this, SLOT (delSourceUniverse()));
		connect (sourceUniverses_, SIGNAL(editTextChanged(QString)), this, SLOT(editSourceUniverse(QString)));
		connect (sourceUniverses_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addProver_, SIGNAL(clicked()), this, SLOT(addProver()));
		connect (delProver_, SIGNAL(clicked()), this, SLOT(delProver()));
		connect (provers_, SIGNAL(editTextChanged(QString)), this, SLOT(editProver(QString)));
		connect (provers_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addVerifier_, SIGNAL(clicked()), this, SLOT (addVerifier()));
		connect (delVerifier_, SIGNAL(clicked()), this, SLOT (delVerifier()));
		connect (verifiers_, SIGNAL(editTextChanged(QString)), this, SLOT(editVerifier(QString)));
		connect (verifiers_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addProveOption_, SIGNAL(clicked()), this, SLOT (addProveOption()));
		connect (delProveOption_, SIGNAL(clicked()), this, SLOT (delProveOption()));
		connect (proveOptions_, SIGNAL(editTextChanged(QString)), this, SLOT(editProveOption(QString)));
		connect (proveOptions_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addTranslateOption_, SIGNAL(clicked()), this, SLOT (addTranslateOption()));
		connect (delTranslateOption_, SIGNAL(clicked()), this, SLOT (delTranslateOption()));
		connect (translateOptions_, SIGNAL(editTextChanged(QString)), this, SLOT(editTranslateOption(QString)));
		connect (translateOptions_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addVerifyOption_, SIGNAL(clicked()), this, SLOT (addVerifyOption()));
		connect (delVerifyOption_, SIGNAL(clicked()), this, SLOT (delVerifyOption()));
		connect (verifyOptions_, SIGNAL(editTextChanged(QString)), this, SLOT(editVerifyOption(QString)));
		connect (verifyOptions_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addLookupOption_, SIGNAL(clicked()), this, SLOT (addLookupOption()));
		connect (delLookupOption_, SIGNAL(clicked()), this, SLOT (delLookupOption()));
		connect (lookupOptions_, SIGNAL(editTextChanged(QString)), this, SLOT(editLookupOption(QString)));
		connect (lookupOptions_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addLearnOption_, SIGNAL(clicked()), this, SLOT (addLearnOption()));
		connect (delLearnOption_, SIGNAL(clicked()), this, SLOT (delLearnOption()));
		connect (learnOptions_, SIGNAL(editTextChanged(QString)), this, SLOT(editLearnOption(QString)));
		connect (learnOptions_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addServerHost_, SIGNAL(clicked()), this, SLOT (addServerHost()));
		connect (delServerHost_, SIGNAL(clicked()), this, SLOT (delServerHost()));
		connect (serverHost_, SIGNAL(editTextChanged(QString)), this, SLOT(editServerHost(QString)));
		connect (serverHost_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (addServerPort_, SIGNAL(clicked()), this, SLOT (addServerPort()));
		connect (delServerPort_, SIGNAL(clicked()), this, SLOT (delServerPort()));
		connect (serverPort_, SIGNAL(editTextChanged(QString)), this, SLOT(editServerPort(QString)));
		connect (serverPort_, SIGNAL(activated(int)), this, SLOT(optionsActivated(int)));

		connect (sourceRootBrowse_, SIGNAL (clicked()), this, SLOT (browseSourceRoot()));
		connect (targetRootBrowse_, SIGNAL (clicked()), this, SLOT (browseTargetRoot()));
		connect (sourceUniverseBrowse_, SIGNAL (clicked()), this, SLOT (browseSourceUniverse()));
		connect (proverBrowse_, SIGNAL (clicked()), this, SLOT (browseProver()));
		connect (verifyBrowse_, SIGNAL (clicked()), this, SLOT (browseVerify()));
	}

	/*void
	Ui :: setSideLayout()
	{
		QGridLayout* layout = new QGridLayout (this);
		horizontalLine_->setFrameShape (QFrame::HLine);

		layout->addWidget (sourceLabel_,  2, 0, 1, 1, Qt :: AlignLeft);
		layout->addWidget (sourceEdit_,   2, 1, 1, 3);
		layout->addWidget (sourceBrowse_, 2, 4, 1, 1);

		layout->addWidget (targetLabel_,  3, 0, 1, 1, Qt :: AlignLeft);
		layout->addWidget (targetEdit_,   3, 1, 1, 3);
		layout->addWidget (targetBrowse_, 3, 4, 1, 1);
	    
		layout->addWidget (proverLabel_,  4, 0, 1, 1, Qt :: AlignLeft);
		layout->addWidget (proverEdit_,   4, 1, 1, 3);
		layout->addWidget (proverBrowse_, 4, 4, 1, 1);

		layout->addWidget (verifierLabel_, 5, 0, 1, 1, Qt :: AlignLeft);
		layout->addWidget (verifyEdit_,    5, 1, 1, 3);
		layout->addWidget (verifyBrowse_,  5, 4, 1, 1);

		layout->addWidget (line_, 6, 0, 1, 5);

		layout->addWidget (proveLabel_,     7, 0, 1, 1, Qt :: AlignLeft);
		layout->addWidget (proveOptions_,   7, 1, 1, 2);
		layout->addWidget (addProveOption_, 7, 3, 1, 1);
		layout->addWidget (delProveOption_, 7, 4, 1, 1);
	    
		layout->addWidget (translateLabel_,     8, 0, 1, 1, Qt :: AlignLeft);
		layout->addWidget (translateOptions_,   8, 1, 1, 2);
		layout->addWidget (addTranslateOption_, 8, 3, 1, 1);
		layout->addWidget (delTranslateOption_, 8, 4, 1, 1);
	    
		layout->addWidget (verifyLabel_,     9, 0, 1, 1, Qt :: AlignLeft);
		layout->addWidget (verifyOptions_,   9, 1, 1, 2);
		layout->addWidget (addVerifyOption_, 9, 3, 1, 1);
		layout->addWidget (delVerifyOption_, 9, 4, 1, 1);

		layout->addWidget (lookupLabel_,     10, 0, Qt :: AlignLeft);
		layout->addWidget (lookupOptions_,   10, 1, 1, 2);
		layout->addWidget (addLookupOption_, 10, 3);
		layout->addWidget (delLookupOption_, 10, 4);

		//layout->addItem (new QSpacerItem (1, 1), 12, 0);
		layout->setColumnStretch (0, 0);
		layout->setRowStretch (6, 1);
	}*/
	void 
	Ui :: setBottomLayout()
	{
		QGridLayout* layout = new QGridLayout (this);

		//QHBoxLayout* tLayout = new QHBoxLayout();

		// zero column : combinations
		/*tLayout->addWidget (addCombination_, 0);
		tLayout->addWidget (delCombination_, 0);
		tLayout->setContentsMargins (0,0,0,0);
		layout->addLayout (tLayout, 1, 0);

		verticalLine1_->setFrameShape (QFrame::HLine);
		verticalLine1_->setFrameShadow (QFrame::Sunken);
		layout->addWidget (verticalLine1_, 0, 1, 5, 1);*/

		layout->addWidget (combinationLabel_, 0, 0, 1, 1, Qt :: AlignRight);
		layout->addWidget (combinations_,     0, 1, 1, 1);
		layout->addWidget (addCombination_,   0, 2, 1, 1);
		layout->addWidget (delCombination_,   0, 3, 1, 1);

		horizontalLine1_->setFrameShape (QFrame::HLine);
		horizontalLine1_->setFrameShadow (QFrame::Sunken);
		layout->addWidget (horizontalLine1_, 1, 0, 1, 11);

		// first column : browsers
		layout->addWidget (sourceRootLabel_,  2, 0, 1, 1, Qt :: AlignRight);
		layout->addWidget (sourceRoots_,      2, 1, 1, 1);
		layout->addWidget (sourceRootBrowse_, 2, 2, 1, 1);
		layout->addWidget (addSourceRoot_,    2, 3, 1, 1);
		layout->addWidget (delSourceRoot_,    2, 4, 1, 1);

		layout->addWidget (targetRootLabel_,  3, 0, 1, 1, Qt :: AlignRight);
		layout->addWidget (targetRoots_,      3, 1, 1, 1);
		layout->addWidget (targetRootBrowse_, 3, 2, 1, 1);
		layout->addWidget (addTargetRoot_,    3, 3, 1, 1);
		layout->addWidget (delTargetRoot_,    3, 4, 1, 1);

		layout->addWidget (sourceUniverseLabel_,  4, 0, 1, 1, Qt :: AlignRight);
		layout->addWidget (sourceUniverses_,      4, 1, 1, 1);
		layout->addWidget (sourceUniverseBrowse_, 4, 2, 1, 1);
		layout->addWidget (addSourceUniverse_,    4, 3, 1, 1);
		layout->addWidget (delSourceUniverse_,    4, 4, 1, 1);

		layout->addWidget (proverLabel_,  5, 0, 1, 1, Qt :: AlignRight);
		layout->addWidget (provers_,      5, 1, 1, 1);
		layout->addWidget (proverBrowse_, 5, 2, 1, 1);
		layout->addWidget (addProver_,    5, 3, 1, 1);
		layout->addWidget (delProver_,    5, 4, 1, 1);

		layout->addWidget (verifierLabel_, 6, 0, 1, 1, Qt :: AlignRight);
		layout->addWidget (verifiers_,     6, 1, 1, 1);
		layout->addWidget (verifyBrowse_,  6, 2, 1, 1);
		layout->addWidget (addVerifier_,   6, 3, 1, 1);
		layout->addWidget (delVerifier_,   6, 4, 1, 1);
	    
		// second column : options
		verticalLine1_->setFrameShape (QFrame::VLine);
		verticalLine1_->setFrameShadow (QFrame::Sunken);
		layout->addWidget (verticalLine1_, 2, 5, 5, 1);

		layout->addWidget (proveOptionLabel_, 2, 6, 1, 1, Qt :: AlignRight);
		layout->addWidget (proveOptions_,     2, 7, 1, 1);
		layout->addWidget (addProveOption_,   2, 8, 1, 1);
		layout->addWidget (delProveOption_,   2, 9, 1, 1);
	    
		layout->addWidget (translateOptionLabel_, 3, 6, 1, 1, Qt :: AlignRight);
		layout->addWidget (translateOptions_,     3, 7, 1, 1);
		layout->addWidget (addTranslateOption_,   3, 8, 1, 1);
		layout->addWidget (delTranslateOption_,   3, 9, 1, 1);
	    
		layout->addWidget (verifyOptionLabel_, 4, 6, 1, 1, Qt :: AlignRight);
		layout->addWidget (verifyOptions_,     4, 7, 1, 1);
		layout->addWidget (addVerifyOption_,   4, 8, 1, 1);
		layout->addWidget (delVerifyOption_,   4, 9, 1, 1);

		layout->addWidget (lookupOptionLabel_, 5, 6, 1, 1, Qt :: AlignRight);
		layout->addWidget (lookupOptions_,     5, 7, 1, 1);
		layout->addWidget (addLookupOption_,   5, 8, 1, 1);
		layout->addWidget (delLookupOption_,   5, 9, 1, 1);

		layout->addWidget (learnOptionLabel_, 6, 6, 1, 1, Qt :: AlignRight);
		layout->addWidget (learnOptions_,     6, 7, 1, 1);
		layout->addWidget (addLearnOption_,   6, 8, 1, 1);
		layout->addWidget (delLearnOption_,   6, 9, 1, 1);
	    
		//layout->addItem (new QSpacerItem (1, 1), 5, 0 );
		//layout->setColumnStretch (3, 1);
		//layout->setColumnStretch (7, 1);
		//layout->setRowStretch (7, 2);

		horizontalLine2_->setFrameShape (QFrame::HLine);
		horizontalLine2_->setFrameShadow (QFrame::Sunken);
		layout->addWidget (horizontalLine2_, 7, 0, 1, 11);

		layout->addWidget (serverHostLabel_,  8, 0, 1, 1, Qt :: AlignRight);
		layout->addWidget (serverHost_,       8, 1, 1, 1);
		layout->addWidget (addServerHost_,    8, 2, 1, 1);
		layout->addWidget (delServerHost_,    8, 3, 1, 1);

		verticalLine2_->setFrameShape (QFrame::VLine);
		verticalLine2_->setFrameShadow (QFrame::Sunken);
		layout->addWidget (verticalLine2_, 8, 5, 1, 1);

		layout->addWidget (serverPortLabel_,  8, 6, 1, 1, Qt :: AlignRight);
		layout->addWidget (serverPort_,       8, 7, 1, 1);
		layout->addWidget (addServerPort_,    8, 8, 1, 1);
		layout->addWidget (delServerPort_,    8, 9, 1, 1);
	}
}
}
}
}

#endif /* PLUGIN_KATE_PROVE_CONFIG_UI_CPP_ */


