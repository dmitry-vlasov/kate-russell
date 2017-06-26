/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Indexes.cpp                   */
/* Description:     config index array for a Russell support plugin for Kate */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_CONFIG_INDEXES_CPP_
#define PLUGIN_KATE_RUSSELL_CONFIG_INDEXES_CPP_

#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

	/****************************
	 *	Public members
	 ****************************/

	Indexes :: Indexes (const Ui* ui) :
	sourceRootIndex_(0),
	targetRootIndex_ (0),
	sourceUniverseIndex_(0),
	proverIndex_ (0),
	verifierIndex_ (0),
	proveOptionIndex_ (0),
	translateOptionIndex_ (0),
	verifyOptionIndex_ (0),
	lookupOptionIndex_ (0),
	learnOptionIndex_ (0),
	serverHostIndex_ (0),
	serverPortIndex_ (0),
	ui_ (ui) {
	}
	Indexes :: Indexes (const Indexes& indexes, const Ui* ui) :
	sourceRootIndex_ (indexes.sourceRootIndex_),
	targetRootIndex_ (indexes.targetRootIndex_),
	sourceUniverseIndex_ (indexes.sourceRootIndex_),
	proverIndex_ (indexes.proverIndex_),
	verifierIndex_ (indexes.verifierIndex_),
	proveOptionIndex_ (indexes.proveOptionIndex_),
	translateOptionIndex_ (indexes.translateOptionIndex_),
	verifyOptionIndex_ (indexes.verifyOptionIndex_),
	lookupOptionIndex_ (indexes.lookupOptionIndex_),
	learnOptionIndex_ (indexes.learnOptionIndex_),
	serverHostIndex_ (indexes.serverHostIndex_),
	serverPortIndex_ (indexes.serverPortIndex_),
	ui_ (ui) {
	}
	Indexes :: Indexes
	(
		const KConfigGroup& configGroup,
		const int index,
		const Ui* ui
	) :
	sourceRootIndex_ (0),
	targetRootIndex_ (0),
	sourceUniverseIndex_ (0),
	proverIndex_ (0),
	verifierIndex_ (0),
	proveOptionIndex_ (0),
	translateOptionIndex_ (0),
	verifyOptionIndex_ (0),
	lookupOptionIndex_ (0),
	learnOptionIndex_ (0),
	serverHostIndex_ (0),
	serverPortIndex_ (0),
	ui_ (ui)
	{
		readSessionConfig (configGroup, index);
		setSane (ui_->config_);
	}
	Indexes :: ~ Indexes() {
	}

	void
	Indexes :: setSane (const Config* config)
	{
		setSane (sourceRootIndex_, config->sourceRootList_);
		setSane (targetRootIndex_, config->targetRootList_);
		setSane (sourceUniverseIndex_, config->sourceUniverseList_);
		setSane (proverIndex_, config->proverList_);
		setSane (verifierIndex_, config->verifierList_);

		setSane (proveOptionIndex_, config->proveOptionList_);
		setSane (translateOptionIndex_, config->translateOptionList_);
		setSane (verifyOptionIndex_, config->verifyOptionList_);
		setSane (lookupOptionIndex_, config->lookupOptionList_);
		setSane (learnOptionIndex_, config->learnOptionList_);

		setSane (serverHostIndex_, config->serverHostList_);
		setSane (serverPortIndex_, config->serverPortList_);
	}
	void
	Indexes :: setUi (const Ui* ui) {
		ui_ = ui;
	}
	void
	Indexes :: synchronize()
	{
		sourceRootIndex_ = ui_->sourceRoots_->currentIndex();
		targetRootIndex_ = ui_->targetRoots_->currentIndex();
		sourceUniverseIndex_ = ui_->sourceUniverses_->currentIndex();
		proverIndex_ = ui_->provers_->currentIndex();
		verifierIndex_ = ui_->verifiers_->currentIndex();

		proveOptionIndex_ = ui_->proveOptions_->currentIndex();
		translateOptionIndex_ = ui_->translateOptions_->currentIndex();
		verifyOptionIndex_ = ui_->verifyOptions_->currentIndex();
		lookupOptionIndex_ = ui_->lookupOptions_->currentIndex();
		learnOptionIndex_ = ui_->learnOptions_->currentIndex();

		serverHostIndex_ = ui_->serverHost_->currentIndex();
		serverPortIndex_ = ui_->serverPort_->currentIndex();

		#ifdef DEBUG_CONFIG
		std :: cout << "Indexes :: synchronize (...)" << std :: endl;
		showToCout();
		#endif
	}
	void
	Indexes :: readSessionConfig (const KConfigGroup& configGroup, const int i)
	{
		if (i < 0) {
			Session :: read (configGroup, i18n("SourceRoot"), sourceRootIndex_);
			Session :: read (configGroup, i18n("TargetRoot"), targetRootIndex_);
			Session :: read (configGroup, i18n("SourceUniverse"), sourceUniverseIndex_);
			Session :: read (configGroup, i18n("Prover"), proverIndex_);
			Session :: read (configGroup, i18n("Verifier"), verifierIndex_);

			Session :: read (configGroup, i18n("ProveOption"), proveOptionIndex_);
			Session :: read (configGroup, i18n("TranslateOption"), translateOptionIndex_);
			Session :: read (configGroup, i18n("VerifyOption"), verifyOptionIndex_);
			Session :: read (configGroup, i18n("LookupOption"), lookupOptionIndex_);
			Session :: read (configGroup, i18n("LearnOption"), learnOptionIndex_);

			Session :: read (configGroup, i18n("ServerHost"), serverHostIndex_);
			Session :: read (configGroup, i18n("ServerPort"), serverPortIndex_);
		} else {
			Session :: read (configGroup, i18n("SourceRoot %1").arg (i), sourceRootIndex_);
			Session :: read (configGroup, i18n("TargetRoot %1").arg (i), targetRootIndex_);
			Session :: read (configGroup, i18n("SourceUniverse %1").arg (i), sourceUniverseIndex_);
			Session :: read (configGroup, i18n("Prover %1").arg (i), proverIndex_);
			Session :: read (configGroup, i18n("Verifier %1").arg (i), verifierIndex_);

			Session :: read (configGroup, i18n("ProveOption %1").arg (i), proveOptionIndex_);
			Session :: read (configGroup, i18n("TranslateOption %1").arg (i), translateOptionIndex_);
			Session :: read (configGroup, i18n("VerifyOption %1").arg (i), verifyOptionIndex_);
			Session :: read (configGroup, i18n("LookupOption %1").arg (i), lookupOptionIndex_);
			Session :: read (configGroup, i18n("LearnOption %1").arg (i), learnOptionIndex_);

			Session :: read (configGroup, i18n("ServerHost %1").arg (i), serverHostIndex_);
			Session :: read (configGroup, i18n("ServerPort %1").arg (i), serverPortIndex_);
		}

		#ifdef DEBUG_CONFIG
		std :: cout << "Indexes :: readSessionConfig (...)" << std :: endl;
		showToCout();
		#endif
	}
	void
	Indexes :: writeSessionConfig (KConfigGroup& configGroup, const int i) const
	{
		if (i < 0) {
			Session :: write (configGroup, i18n("SourceRoot"), sourceRootIndex_);
			Session :: write (configGroup, i18n("TargetRoot"), targetRootIndex_);
			Session :: write (configGroup, i18n("SourceUniverse"), sourceUniverseIndex_);
			Session :: write (configGroup, i18n("Prover"), proverIndex_);
			Session :: write (configGroup, i18n("Verifier"), verifierIndex_);

			Session :: write (configGroup, i18n("ProveOption"), proveOptionIndex_);
			Session :: write (configGroup, i18n("TranslateOption"), translateOptionIndex_);
			Session :: write (configGroup, i18n("VerifyOption"), verifyOptionIndex_);
			Session :: write (configGroup, i18n("LookupOption"), lookupOptionIndex_);
			Session :: write (configGroup, i18n("LearnOption"), learnOptionIndex_);

			Session :: write (configGroup, i18n("ServerHost"), serverHostIndex_);
			Session :: write (configGroup, i18n("ServerPort"), serverPortIndex_);
		} else {
			Session :: write (configGroup, i18n("SourceRoot %1").arg (i), sourceRootIndex_);
			Session :: write (configGroup, i18n("TargetRoot %1").arg (i), targetRootIndex_);
			Session :: write (configGroup, i18n("SourceUniverse %1").arg (i), sourceUniverseIndex_);
			Session :: write (configGroup, i18n("Prover %1").arg (i), proverIndex_);
			Session :: write (configGroup, i18n("Verifier %1").arg (i), verifierIndex_);

			Session :: write (configGroup, i18n("ProveOption %1").arg (i), proveOptionIndex_);
			Session :: write (configGroup, i18n("TranslateOption %1").arg (i), translateOptionIndex_);
			Session :: write (configGroup, i18n("VerifyOption %1").arg (i), verifyOptionIndex_);
			Session :: write (configGroup, i18n("LookupOption %1").arg (i), lookupOptionIndex_);
			Session :: write (configGroup, i18n("LearnOption %1").arg (i), learnOptionIndex_);

			Session :: write (configGroup, i18n("ServerHost %1").arg (i), serverHostIndex_);
			Session :: write (configGroup, i18n("ServerPort %1").arg (i), serverPortIndex_);
		}
		#ifdef DEBUG_CONFIG
		std :: cout << "Indexes :: writeSessionConfig (...)" << std :: endl;
		showToCout();
		#endif
	}
	void
	Indexes :: operator = (const Indexes& indexes)
	{
		sourceRootIndex_ = indexes.sourceRootIndex_;
		targetRootIndex_ = indexes.targetRootIndex_;
		sourceUniverseIndex_ = indexes.sourceUniverseIndex_;
		proverIndex_ = indexes.proverIndex_;
		verifierIndex_ = indexes.verifierIndex_;

		proveOptionIndex_ = indexes.proveOptionIndex_;
		translateOptionIndex_ = indexes.translateOptionIndex_;
		verifyOptionIndex_ = indexes.verifyOptionIndex_;
		lookupOptionIndex_ = indexes.lookupOptionIndex_;
		learnOptionIndex_ = indexes.learnOptionIndex_;

		serverHostIndex_ = indexes.serverHostIndex_;
		serverPortIndex_ = indexes.serverPortIndex_;
	}
	void
	Indexes :: showToCout() const
	{
		std :: cout << "Indexes :: showToCout()" << std :: endl;
		std :: cout << "\tsourceRootIndex_ = " << sourceRootIndex_ << std :: endl;
		std :: cout << "\ttargetRootIndex_ = " << targetRootIndex_ << std :: endl;
		std :: cout << "\tsourceUniverseIndex_ = " << sourceUniverseIndex_ << std :: endl;
		std :: cout << "\tproverIndex_ = " << proverIndex_ << std :: endl;
		std :: cout << "\tverifierIndex_ = " << verifierIndex_ << std :: endl;
		std :: cout << std :: endl;
		std :: cout << "\tproveOptionIndex_ = " << proveOptionIndex_ << std :: endl;
		std :: cout << "\ttranslateOptionIndex_ = " << translateOptionIndex_ << std :: endl;
		std :: cout << "\tverifyOptionIndex_ = " << verifyOptionIndex_ << std :: endl;
		std :: cout << "\tlookupOptionIndex_ = " << lookupOptionIndex_ << std :: endl;
		std :: cout << "\tlearnOptionIndex_ = " << learnOptionIndex_ << std :: endl;
		std :: cout << std :: endl;
		std :: cout << "\tserverHostIndex_ = " << serverHostIndex_ << std :: endl;
		std :: cout << "\tserverPortIndex_ = " << serverPortIndex_ << std :: endl;
	}

	/****************************
	 *	Private members
	 ****************************/

	void
	Indexes :: setSane (int& index, const QStringList& stringList)
	{
		if (
			(index < 0) ||
			(index + 1 > stringList.count())
		) {
			index = 0;
		}
	}
}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_CONFIG_INDEXES_CPP_ */

