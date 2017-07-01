/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Config.hpp                    */
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

#pragma once

#include "russell.dpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

class ConfigOld {
public :
	ConfigOld();
	virtual ~ ConfigOld();

	void setSane();
	void setUi (const Ui*);
	void synchronize();
	void showToCout() const;

	const QString& getSourceRoot() const;
	const QString& getTargetRoot() const;
	const QString& getSourceUniverse() const;
	const QString& getProver() const;
	const QString& getVerifier() const;

	const QString& getProveOption() const;
	const QString& getVerifyOption() const;
	const QString& getTranslateOption() const;
	const QString& getLookupOption() const;
	const QString& getLearnOption() const;
	
	const QString& getServerHost() const;
	const QString& getServerPort() const;

	Indexes& indexes();

	void readSessionConfig (const KConfigGroup&);
	void writeSessionConfig (KConfigGroup&) const;

private :
	friend class Ui;
	friend class Indexes;

	void showStringListToCout (const QStringList&, const char*) const;

	const Ui* ui_;

	QStringList sourceRootList_;
	QStringList targetRootList_;
	QStringList sourceUniverseList_;
	QStringList proverList_;
	QStringList verifierList_;

	QStringList proveOptionList_;
	QStringList translateOptionList_;
	QStringList verifyOptionList_;
	QStringList lookupOptionList_;
	QStringList learnOptionList_;

	QStringList serverHostList_;
	QStringList serverPortList_;

	Indexes* indexes_;

	static const QString defaultSourceRoot_;
	static const QString defaultTargetRoot_;
	static const QString defaultSourceUniverse_;
	static const QString defaultProver_;
	static const QString defaultVerifier_;
	static const QString defaultProveOption_;
	static const QString defaultVerifyOption_;
	static const QString defaultTranslateOption_;
	static const QString defaultLookupOption_;
	static const QString defaultLearnOption_;
	static const QString defaultServerHost_;
	static const QString defaultServerPort_;
};

}
}
}
}

