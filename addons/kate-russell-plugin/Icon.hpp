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
	static const QIcon& get (const Kind);

	// Russell source elements
	static const QIcon& source();
	static const QIcon& import();
	static const QIcon& theory();
	static const QIcon& contents();
	static const QIcon& constant();
	static const QIcon& type();
	static const QIcon& rule();
	static const QIcon& axiom();
	static const QIcon& definition();
	static const QIcon& theorem();
	static const QIcon& problem();
	static const QIcon& proof();

	// Proof tree elements
	static const QIcon& root();
	static const QIcon& hyp();
	static const QIcon& prop();
	static const QIcon& ref();
	static const QIcon& top();
	//static const QIcon& proved();
};

}
