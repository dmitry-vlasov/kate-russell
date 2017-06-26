/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Session.cpp                   */
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

#ifndef PLUGIN_KATE_RUSSELL_CONFIG_SESSION_CPP_
#define PLUGIN_KATE_RUSSELL_CONFIG_SESSION_CPP_

#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace config {

	/****************************
	 *	Public members
	 ****************************/

	void
	Session :: write
	(
		KConfigGroup& configGroup,
		const QString& name,
		const int value
	)
	{
		QString string (name);
		string += i18n(" value");
		configGroup.writeEntry (string, value);
	}
	void
	Session :: read
	(
		const KConfigGroup& configGroup,
		const QString& name,
		int& value
	)
	{
		QString string (name);
		string += i18n(" value");
		value = configGroup.readEntry (string, -1);
	}
	void
	Session :: writeList
	(
		KConfigGroup& configGroup,
		const QString& name,
		const QStringList& list
	)
	{
		for (int i = 0; i < list.size(); ++ i) {
			QString entryName (i18n("%1 "));
			entryName += name;
			configGroup.writeEntry
			(
				entryName.arg(i),
				list [i]
			);
		}
		QString optionCount (name);
		optionCount += i18n(" count");
		configGroup.writeEntry (optionCount, list.size());
	}
	void
	Session :: readList
	(
		const KConfigGroup& configGroup,
		const QString& name,
		QStringList& list,
		const QString& defaultValue
	)
	{
		QString optionCount (name);
		optionCount += i18n(" count");
		const int count = configGroup.readEntry (optionCount, -1);

		list.clear();
		for (int i = 0; i < count; ++ i) {
			QString entryName (i18n("%1 "));
			entryName += name;
			list << configGroup.readEntry
			(
				entryName.arg (i),
				defaultValue
			);
		}
	}
}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_CONFIG_SESSION_CPP_ */

