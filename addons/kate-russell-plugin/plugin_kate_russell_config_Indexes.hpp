/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Indexes.hpp                   */
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

#ifndef PLUGIN_KATE_RUSSELL_CONFIG_INDEXES_HPP_
#define PLUGIN_KATE_RUSSELL_CONFIG_INDEXES_HPP_

#include "plugin_kate_russell.dpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

class Indexes {
public :
	Indexes (const Ui* = NULL);
	Indexes (const Indexes&, const Ui* = NULL);
	Indexes (const KConfigGroup&, const int, const Ui* = NULL);
	virtual ~ Indexes();

	void setSane (const Config*);
	void setUi (const Ui*);
	void synchronize();
	void readSessionConfig (const KConfigGroup&, const int i = -1);
	void writeSessionConfig (KConfigGroup&, const int i = -1) const;
	void operator = (const Indexes&);
	void showToCout() const;

	int sourceRootIndex_;
	int targetRootIndex_;
	int sourceUniverseIndex_;
	int proverIndex_;
	int verifierIndex_;

	int proveOptionIndex_;
	int translateOptionIndex_;
	int verifyOptionIndex_;
	int lookupOptionIndex_;
	int learnOptionIndex_;

	int serverHostIndex_;
	int serverPortIndex_;

private :
	friend class Ui;

	void setSane (int&, const QStringList&);

	const Ui* ui_;
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_CONFIG_INDEXES_HPP_ */

