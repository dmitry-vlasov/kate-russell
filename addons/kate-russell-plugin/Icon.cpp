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
		static QIcon theory(QStringLiteral(":/katerussell/icons/hi16-actions-russell-theory.png"));
		return theory;
	}
	const QIcon&
	Icon :: import() {
		static QIcon import(QStringLiteral(":/katerussell/icons/hi16-actions-russell-import.png"));
		return import;
	}
	const QIcon&
	Icon :: theory() {
		static QIcon theory(QStringLiteral(":/katerussell/icons/hi16-actions-russell-theory.png"));
		return theory;
	}
	const QIcon&
	Icon :: contents() {
		static QIcon theory(QStringLiteral(":/katerussell/icons/hi16-actions-russell-theory.png"));
		return theory;
	}
	const QIcon&
	Icon :: constant() {
		static QIcon constant(QStringLiteral(":/katerussell/icons/hi16-actions-russell-constant.png"));
		return constant;
	}
	const QIcon&
	Icon :: rule() {
		static QIcon rule(QStringLiteral(":/katerussell/icons/hi16-actions-russell-rule.png"));
		return rule;
	}
	const QIcon&
	Icon :: type() {
		static QIcon type(QStringLiteral(":/katerussell/icons/hi16-actions-russell-type.png"));
		return type;
	}
	const QIcon&
	Icon :: axiom() {
		static QIcon axiom(QStringLiteral(":/katerussell/icons/hi16-actions-russell-axiom.png"));
		return axiom;
	}
	const QIcon&
	Icon :: definition() {
		static QIcon definition(QStringLiteral(":/katerussell/icons/hi16-actions-russell-definition.png"));
		return definition;
	}
	const QIcon&
	Icon :: theorem() {
		static QIcon theorem(QStringLiteral(":/katerussell/icons/hi16-actions-russell-theorem.png"));
		return theorem;
	}
	const QIcon&
	Icon :: problem() {
		static QIcon problem(QStringLiteral(":/katerussell/icons/hi16-actions-russell-problem.png"));
		return problem;
	}
	const QIcon&
	Icon :: proof() {
		static QIcon proof(QStringLiteral(":/katerussell/icons/hi16-actions-russell-proof.png"));
		return proof;
	}

	// Proof tree elements
	const QIcon&
	Icon :: root() {
		static QIcon root(QStringLiteral(":/katerussell/icons/hi16-actions-russell-root.png"));
		return root;
	}
	const QIcon&
	Icon :: hyp() {
		static QIcon hyp(QStringLiteral(":/katerussell/icons/hi16-actions-russell-hyp.png"));
		return hyp;
	}
	const QIcon&
	Icon :: prop() {
		static QIcon prop(QStringLiteral(":/katerussell/icons/hi16-actions-russell-prop.png"));
		return prop;
	}
	const QIcon&
	Icon :: ref() {
		static QIcon ref(QStringLiteral(":/katerussell/icons/hi16-actions-russell-ref.png"));
		return ref;
	}
	const QIcon&
	Icon :: top() {
		static QIcon top(QStringLiteral(":/katerussell/icons/hi16-actions-russell-top.png"));
		return top;
	}
	/*const QIcon&
	Icon :: proved() {
		return proved_;
	}*/
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_ICON_CPP_ */

