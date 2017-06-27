/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Config.cpp                    */
/* Description:     a config class for a Russell support plugin for Kate     */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_CONFIG_CONFIG_CPP_
#define PLUGIN_KATE_RUSSELL_CONFIG_CONFIG_CPP_

#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

	/****************************
	 *	Public members
	 ****************************/

	Config :: Config() :
	ui_ (NULL),
	sourceRootList_(),
	targetRootList_(),
	sourceUniverseList_(),
	proverList_(),
	verifierList_(),
	proveOptionList_ (),
	translateOptionList_ (),
	verifyOptionList_ (),
	lookupOptionList_ (),
	learnOptionList_ (),
	serverHostList_ (),
	serverPortList_ (),
	indexes_ (new Indexes ()) {
	}
	Config :: ~ Config() {
		delete indexes_;
	}

	void
	Config :: setSane()
	{
		if (sourceRootList_.isEmpty()) {
			sourceRootList_ << defaultSourceRoot_;
		}
		if (targetRootList_.isEmpty()) {
			targetRootList_ << defaultTargetRoot_;
		}
		if (sourceUniverseList_.isEmpty()) {
			sourceUniverseList_ << defaultSourceUniverse_;
		}
		if (proverList_.isEmpty()) {
			proverList_ << defaultProver_;
		}
		if (verifierList_.isEmpty()) {
			verifierList_ << defaultVerifier_;
		}

		if (proveOptionList_.isEmpty()) {
			proveOptionList_ << defaultProveOption_;
		}
		if (translateOptionList_.isEmpty()) {
			translateOptionList_ << defaultTranslateOption_;
		}
		if (verifyOptionList_.isEmpty()) {
			verifyOptionList_ << defaultVerifyOption_;
		}
		if (lookupOptionList_.isEmpty()) {
			lookupOptionList_ << defaultLookupOption_;
		}
		if (learnOptionList_.isEmpty()) {
			learnOptionList_ << defaultLearnOption_;
		}
		if (serverHostList_.isEmpty()) {
			serverHostList_ << defaultServerHost_;
		}
		if (serverPortList_.isEmpty()) {
			serverPortList_ << defaultServerPort_;
		}
		indexes_->setSane (this);
	}
	void
	Config :: setUi (const Ui* ui)
	{
		ui_ = ui;
		indexes_->setUi (ui);
	}
	void
	Config :: synchronize()
	{
		indexes_->synchronize();

		sourceRootList_.clear();
		for (int i = 0; i < ui_->sourceRoots_->count(); ++ i) {
			sourceRootList_ << ui_->sourceRoots_->itemText (i);
		}
		targetRootList_.clear();
		for (int i = 0; i < ui_->targetRoots_->count(); ++ i) {
			targetRootList_ << ui_->targetRoots_->itemText (i);
		}
		sourceUniverseList_.clear();
		for (int i = 0; i < ui_->sourceUniverses_->count(); ++ i) {
			sourceUniverseList_ << ui_->sourceUniverses_->itemText (i);
		}
		proverList_.clear();
		for (int i = 0; i < ui_->provers_->count(); ++ i) {
			proverList_ << ui_->provers_->itemText (i);
		}
		verifierList_.clear();
		for (int i = 0; i < ui_->verifiers_->count(); ++ i) {
			verifierList_ << ui_->verifiers_->itemText (i);
		}

		proveOptionList_.clear();
		for (int i = 0; i < ui_->proveOptions_->count(); ++ i) {
			proveOptionList_ << ui_->proveOptions_->itemText (i);
		}
		translateOptionList_.clear();
		for (int i = 0; i < ui_->translateOptions_->count(); ++ i) {
			translateOptionList_ << ui_->translateOptions_->itemText (i);
		}
		verifyOptionList_.clear();
		for (int i = 0; i < ui_->verifyOptions_->count(); ++ i) {
			verifyOptionList_ << ui_->verifyOptions_->itemText (i);
		}
		lookupOptionList_.clear();
		for (int i = 0; i < ui_->lookupOptions_->count(); ++ i) {
			lookupOptionList_ << ui_->lookupOptions_->itemText (i);
		}
		learnOptionList_.clear();
		for (int i = 0; i < ui_->learnOptions_->count(); ++ i) {
			learnOptionList_ << ui_->learnOptions_->itemText (i);
		}
		serverHostList_.clear();
		for (int i = 0; i < ui_->serverHost_->count(); ++ i) {
			serverHostList_ << ui_->serverHost_->itemText (i);
		}
		serverPortList_.clear();
		for (int i = 0; i < ui_->serverPort_->count(); ++ i) {
			serverPortList_ << ui_->serverPort_->itemText (i);
		}
		setSane();

		#ifdef DEBUG_CONFIG
		std :: cout << "Config :: synchronize()" << std :: endl;
		showToCout();
		#endif
	}
	void
	Config :: showToCout() const
	{
		std :: cout << "Config :: showToCout()" << std :: endl;
		showStringListToCout (sourceRootList_, "SourceRoot");
		showStringListToCout (targetRootList_, "TargetRoot");
		showStringListToCout (sourceUniverseList_, "SourceUniverse");
		showStringListToCout (proverList_, "Prover");
		showStringListToCout (verifierList_, "Verifier");

		showStringListToCout (proveOptionList_, "ProveOption");
		showStringListToCout (translateOptionList_, "TranslateOption");
		showStringListToCout (verifyOptionList_, "VerifyOption");
		showStringListToCout (lookupOptionList_, "LookupOption");
		showStringListToCout (learnOptionList_, "LearnOption");

		showStringListToCout (serverHostList_, "ServerHost");
		showStringListToCout (serverPortList_, "ServerPort");
		indexes_->showToCout();
	}

	const QString&
	Config :: getSourceRoot() const {
		return sourceRootList_ [indexes_->sourceRootIndex_];
	}
	const QString& 
	Config :: getTargetRoot() const {
		return targetRootList_ [indexes_->targetRootIndex_];
	}
	const QString&
	Config :: getSourceUniverse() const {
		return sourceUniverseList_ [indexes_->sourceUniverseIndex_];
	}
	const QString& 
	Config :: getProver() const {
		return proverList_ [indexes_->proverIndex_];
	}
	const QString& 
	Config :: getVerifier() const {
		return verifierList_ [indexes_->verifierIndex_];
	}
	const QString& 
	Config :: getProveOption() const {
		return proveOptionList_ [indexes_->proveOptionIndex_];
	}
	const QString& 
	Config :: getVerifyOption() const {
		return verifyOptionList_ [indexes_->verifyOptionIndex_];
	}
	const QString& 
	Config :: getTranslateOption() const {
		return translateOptionList_ [indexes_->translateOptionIndex_];
	}
	const QString& 
	Config :: getLookupOption() const {
		return lookupOptionList_ [indexes_->lookupOptionIndex_];
	}
	const QString&
	Config :: getLearnOption() const {
		return learnOptionList_ [indexes_->learnOptionIndex_];
	}
	const QString&
	Config :: getServerHost() const {
		return serverHostList_ [indexes_->serverHostIndex_];
	}
	const QString&
	Config :: getServerPort() const {
		return serverPortList_ [indexes_->serverPortIndex_];
	}

	Indexes&
	Config :: indexes() {
		return *indexes_;
	}

	void
	Config :: readSessionConfig (const KConfigGroup& configGroup)
	{
		indexes_->readSessionConfig (configGroup);

		Session :: readList (configGroup, i18n("SourceRoot"), sourceRootList_, defaultSourceRoot_);
		Session :: readList (configGroup, i18n("TargetRoot"), targetRootList_, defaultTargetRoot_);
		Session :: readList (configGroup, i18n("SourceUniverse"), sourceUniverseList_, defaultSourceUniverse_);
		Session :: readList (configGroup, i18n("Prover"), proverList_, defaultProver_);
		Session :: readList (configGroup, i18n("Verifier"), verifierList_, defaultVerifier_);

		Session :: readList (configGroup, i18n("ProveOption"), proveOptionList_, defaultProveOption_);
		Session :: readList (configGroup, i18n("TranslateOption"), translateOptionList_, defaultTranslateOption_);
		Session :: readList (configGroup, i18n("VerifyOption"), verifyOptionList_, defaultVerifyOption_);
		Session :: readList (configGroup, i18n("LookupOption"), lookupOptionList_, defaultLookupOption_);
		Session :: readList (configGroup, i18n("LearnOption"), learnOptionList_, defaultLearnOption_);

		Session :: readList (configGroup, i18n("ServerHost"), serverHostList_, defaultServerHost_);
		Session :: readList (configGroup, i18n("ServerPort"), serverPortList_, defaultServerPort_);

		setSane();

		#ifdef DEBUG_CONFIG
		std :: cout << "Config :: readSessionConfig()" << std :: endl;
		showToCout();
		#endif
	}
	void
	Config :: writeSessionConfig (KConfigGroup& configGroup) const
	{
		indexes_->writeSessionConfig (configGroup);

		Session :: writeList (configGroup, i18n("SourceRoot"), sourceRootList_);
		Session :: writeList (configGroup, i18n("TargetRoot"), targetRootList_);
		Session :: writeList (configGroup, i18n("SourceUniverse"), sourceUniverseList_);
		Session :: writeList (configGroup, i18n("Prover"), proverList_);
		Session :: writeList (configGroup, i18n("Verifier"), verifierList_);

		Session :: writeList (configGroup, i18n("ProveOption"), proveOptionList_);
		Session :: writeList (configGroup, i18n("TranslateOption"), translateOptionList_);
		Session :: writeList (configGroup, i18n("VerifyOption"), verifyOptionList_);
		Session :: writeList (configGroup, i18n("LookupOption"), lookupOptionList_);
		Session :: writeList (configGroup, i18n("LearnOption"), learnOptionList_);

		Session :: writeList (configGroup, i18n("ServerHost"), serverHostList_);
		Session :: writeList (configGroup, i18n("ServerPort"), serverPortList_);

		#ifdef DEBUG_CONFIG
		std :: cout << "Config :: writeSessionConfig()" << std :: endl;
		showToCout();
		#endif
	}

	/****************************
	 *	Private members
	 ****************************/

	void
	Config :: showStringListToCout (const QStringList& stirngList, const char* name) const
	{
		std :: cout << name << ":" << std :: endl;
		for (int i = 0; i < stirngList.count(); ++ i) {
			std :: cout << '\t' << qPrintable (stirngList [i]) << std :: endl;
		}
		std :: cout << std :: endl;
	}

	/*************************************
	 *	Private static attributes
	 *************************************/

	const QString Config :: defaultSourceRoot_ = i18n("");
	const QString Config :: defaultTargetRoot_ = i18n("");
	const QString Config :: defaultSourceUniverse_ = i18n("");
	const QString Config :: defaultProver_     = i18n("mdl");
	const QString Config :: defaultVerifier_   = i18n("smm");

	const QString Config :: defaultProveOption_     = i18n("-v -w %f");
	const QString Config :: defaultTranslateOption_ = i18n("-v -w -o %t %s");
	const QString Config :: defaultVerifyOption_    = i18n("-v %f");
	const QString Config :: defaultLookupOption_    = i18n("--line %l --column %c %f");
	const QString Config :: defaultLearnOption_     = i18n("-v -w --universe %u %f");

	const QString Config :: defaultServerHost_ = i18n("localhost");
	const QString Config :: defaultServerPort_ = i18n("1872");
}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_CONFIG_CONFIG_CPP_ */

