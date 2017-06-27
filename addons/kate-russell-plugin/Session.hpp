/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Session.hpp                   */
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

#ifndef PLUGIN_KATE_RUSSELL_CONFIG_SESSION_HPP_
#define PLUGIN_KATE_RUSSELL_CONFIG_SESSION_HPP_

#include "russell.dpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

class Session {
public :
	static void write
	(
		KConfigGroup& configGroup,
		const QString& name,
		const int value
	);
	static void read
	(
		const KConfigGroup& configGroup,
		const QString& name,
		int& value
	);
	static void writeList
	(
		KConfigGroup& configGroup,
		const QString& name,
		const QStringList& list
	);
	static void readList
	(
		const KConfigGroup& configGroup,
		const QString& name,
		QStringList& list,
		const QString& defaultValue
	);
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_CONFIG_SESSION_HPP_ */

