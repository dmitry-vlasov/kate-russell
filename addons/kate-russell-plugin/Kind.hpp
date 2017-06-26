/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Kind.dpp                             */
/* Description:     enum for kinds of structures in source files             */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_KIND_HPP_
#define PLUGIN_KATE_RUSSELL_KIND_HPP_

namespace plugin {
namespace kate {
namespace russell {

	enum Kind {
		SOURCE,
		IMPORT,
		THEORY,
		CONTENTS,
		CONSTANT,
		RULE,
		AXIOM,
		DEFINITION,
		THEOREM,
		PROBLEM,
		PROOF,
		TYPE
	};
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_KIND_HPP_ */

