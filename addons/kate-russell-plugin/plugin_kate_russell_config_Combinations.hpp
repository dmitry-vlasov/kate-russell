/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Combinations.hpp              */
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

#ifndef PLUGIN_KATE_RUSSELL_CONFIG_COMBINATIONS_HPP_
#define PLUGIN_KATE_RUSSELL_CONFIG_COMBINATIONS_HPP_

#include "plugin_kate_russell.dpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

class Combinations {
public :
	Combinations (const Ui* = NULL);
	virtual ~ Combinations();

	void setSane();
	void setUi (const Ui*);
	void synchronize (const bool add);
	void choose (const int);
	void updateName (const int);

	void readSessionConfig (const KConfigGroup&);
	void writeSessionConfig (KConfigGroup&) const;

	Indexes& combination();

	void showToCout() const;

private :
	friend class Ui;
	friend class Config;

	void writeCombinations (KConfigGroup& configGroup) const;
	void readCombinations (const KConfigGroup& configGroup);

	const Ui* ui_;

	QList<Indexes> combinations_;
	QStringList    combNames_;
	int            index_;

	static const QString defaultName_;
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_CONFIG_COMBINATIONS_HPP_ */

