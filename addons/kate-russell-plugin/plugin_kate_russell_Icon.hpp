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

#ifndef PLUGIN_KATE_RUSSELL_ICON_HPP_
#define PLUGIN_KATE_RUSSELL_ICON_HPP_

#include "plugin_kate_russell.dpp"

namespace plugin {
namespace kate {
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

private :
	static const QIcon import_;
	static const QIcon theory_;
	static const QIcon constant_;
	static const QIcon type_;
	static const QIcon rule_;
	static const QIcon axiom_;
	static const QIcon definition_;
	static const QIcon theorem_;
	static const QIcon problem_;
	static const QIcon proof_;

	static const QIcon root_;
	static const QIcon hyp_;
	static const QIcon prop_;
	static const QIcon ref_;
	static const QIcon top_;
	//static const QIcon proved_;
};

}
}
}

#endif /* PLUGIN_KATE_RUSSELL_ICON_HPP_ */

