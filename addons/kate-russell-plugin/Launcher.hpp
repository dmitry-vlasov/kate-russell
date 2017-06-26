/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Launcher.hpp                     */
/* Description:     a launcher class for a Russell support plugin for Kate   */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_MDL_LAUNCHER_HPP_
#define PLUGIN_KATE_RUSSELL_MDL_LAUNCHER_HPP_

#include "plugin_kate_russell.dpp"

namespace plugin {
namespace kate {
namespace russell {
namespace mdl {

class Launcher { 
public :
	Launcher (View*, const Config*);
	virtual ~ Launcher();
	
	bool prove (const bool clearOutput = true);
	bool translate (const bool clearOutput = true);
	bool verify    (const bool clearOutput = true);
	bool learn     (const bool clearOutput = true);
	bool lookupDefinition (const int line, const int column);
	bool lookupLocation   (const int line, const int column);
	bool mine (const QString&);

	bool prove (
		const int line,
		const int column,
		const bool clearOutput = true
	);

	bool stop();

	KProcess* process();

	static bool checkLocal (const QString&);
	static bool checkLocal (const QUrl&);

private :
	bool startProcess (const QString&, const QString&);

	View*         view_;
	const Config* config_;
	KProcess*     process_;
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_LAUNCHER_HPP_ */

