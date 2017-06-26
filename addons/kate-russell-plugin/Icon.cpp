/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Icon.cpp                             */
/* Description:     icons for Kate Russell plugin                            */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_ICON_CPP_
#define PLUGIN_KATE_RUSSELL_ICON_CPP_

#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	const QIcon&
	Icon :: get (const Kind kind)
	{
		switch (kind) {
		case SOURCE     : return source();
		case IMPORT     : return import();
		case THEORY     : return theory();
		case CONTENTS   : return contents();
		case CONSTANT   : return constant();
		case TYPE       : return type();
		case RULE       : return rule();
		case AXIOM      : return axiom();
		case DEFINITION : return definition();
		case THEOREM    : return theorem();
		case PROBLEM    : return problem();
		case PROOF      : return proof();
		default         : return theory();
		}
	}

	// Russell source elements
	const QIcon&
	Icon :: source() {
		return theory_;
	}
	const QIcon&
	Icon :: import() {
		return import_;
	}
	const QIcon&
	Icon :: theory() {
		return theory_;
	}
	const QIcon&
	Icon :: contents() {
		return theory_;
	}
	const QIcon&
	Icon :: constant() {
		return constant_;
	}
	const QIcon&
	Icon :: rule() {
		return rule_;
	}
	const QIcon&
	Icon :: type() {
		return type_;
	}
	const QIcon&
	Icon :: axiom() {
		return axiom_;
	}
	const QIcon&
	Icon :: definition() {
		return definition_;
	}
	const QIcon&
	Icon :: theorem() {
		return theorem_;
	}
	const QIcon&
	Icon :: problem() {
		return problem_;
	}
	const QIcon&
	Icon :: proof() {
		return proof_;
	}

	// Proof tree elements
	const QIcon&
	Icon :: root() {
		return root_;
	}
	const QIcon&
	Icon :: hyp() {
		return hyp_;
	}
	const QIcon&
	Icon :: prop() {
		return prop_;
	}
	const QIcon&
	Icon :: ref() {
		return ref_;
	}
	const QIcon&
	Icon :: top() {
		return top_;
	}
	/*const QIcon&
	Icon :: proved() {
		return proved_;
	}*/

	/****************************
	 *	Private members
	 ****************************/

	const QIcon Icon :: import_ (QStringLiteral("russell-import"));
	const QIcon Icon :: theory_ (QStringLiteral("russell-theory"));
	const QIcon Icon :: constant_(QStringLiteral("russell-constant"));
	const QIcon Icon :: type_ (QStringLiteral("russell-type"));
	const QIcon Icon :: rule_ (QStringLiteral("russell-rule"));
	const QIcon Icon :: axiom_(QStringLiteral("russell-axiom"));
	const QIcon Icon :: definition_(QStringLiteral("russell-definition"));
	const QIcon Icon :: theorem_(QStringLiteral("russell-theorem"));
	const QIcon Icon :: problem_(QStringLiteral("russell-problem"));
	const QIcon Icon :: proof_(QStringLiteral("russell-proof"));

	const QIcon Icon :: root_(QStringLiteral("russell-root"));
	const QIcon Icon :: hyp_ (QStringLiteral("russell-hyp"));
	const QIcon Icon :: prop_(QStringLiteral("russell-prop"));
	const QIcon Icon :: ref_(QStringLiteral("russell-ref"));
	const QIcon Icon :: top_(QStringLiteral("russell-top"));
	//const QIcon Icon :: proved_("russell-proved");

}
}
}

#endif /* PLUGIN_KATE_RUSSELL_ICON_CPP_ */

