/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Icon.hpp                             */
/* Description:     icons for Kate Russell plugin                            */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#pragma once

#include <QIcon>
#include "Kind.hpp"

namespace russell {

class Icon {
public :  
	static const QIcon& get (const Kind kind) {
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
	static const QIcon& source() {
		static QIcon theory(QStringLiteral(":/katerussell/icons/hi16-actions-russell-theory.png"));
		return theory;
	}
	static const QIcon& import() {
		static QIcon import(QStringLiteral(":/katerussell/icons/hi16-actions-russell-import.png"));
		return import;
	}
	static const QIcon& theory() {
		static QIcon theory(QStringLiteral(":/katerussell/icons/hi16-actions-russell-theory.png"));
		return theory;
	}
	static const QIcon& contents() {
		static QIcon theory(QStringLiteral(":/katerussell/icons/hi16-actions-russell-theory.png"));
		return theory;
	}
	static const QIcon& constant() {
		static QIcon constant(QStringLiteral(":/katerussell/icons/hi16-actions-russell-constant.png"));
		return constant;
	}
	static const QIcon& rule() {
		static QIcon rule(QStringLiteral(":/katerussell/icons/hi16-actions-russell-rule.png"));
		return rule;
	}
	static const QIcon& type() {
		static QIcon type(QStringLiteral(":/katerussell/icons/hi16-actions-russell-type.png"));
		return type;
	}
	static const QIcon& axiom() {
		static QIcon axiom(QStringLiteral(":/katerussell/icons/hi16-actions-russell-axiom.png"));
		return axiom;
	}
	static const QIcon& definition() {
		static QIcon definition(QStringLiteral(":/katerussell/icons/hi16-actions-russell-definition.png"));
		return definition;
	}
	static const QIcon& theorem() {
		static QIcon theorem(QStringLiteral(":/katerussell/icons/hi16-actions-russell-theorem.png"));
		return theorem;
	}
	static const QIcon& problem() {
		static QIcon problem(QStringLiteral(":/katerussell/icons/hi16-actions-russell-problem.png"));
		return problem;
	}
	static const QIcon& proof() {
		static QIcon proof(QStringLiteral(":/katerussell/icons/hi16-actions-russell-proof.png"));
		return proof;
	}

	// Proof tree elements
	static const QIcon& root() {
		static QIcon root(QStringLiteral(":/katerussell/icons/hi16-actions-russell-root.png"));
		return root;
	}
	static const QIcon& hyp() {
		static QIcon hyp(QStringLiteral(":/katerussell/icons/hi16-actions-russell-hyp.png"));
		return hyp;
	}
	static const QIcon& prop() {
		static QIcon prop(QStringLiteral(":/katerussell/icons/hi16-actions-russell-prop.png"));
		return prop;
	}
	static const QIcon& ref() {
		static QIcon ref(QStringLiteral(":/katerussell/icons/hi16-actions-russell-ref.png"));
		return ref;
	}
	static const QIcon& top() {
		static QIcon top(QStringLiteral(":/katerussell/icons/hi16-actions-russell-top.png"));
		return top;
	}
};

}
