/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_TypeSystem.hpp                       */
/* Description:     a type system view for a Russell support plugin for Kate */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2003 by Massimo Callegari <massimocallegari@yahoo.it>*/
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_TYPE_SYSTEM_HPP_
#define PLUGIN_KATE_RUSSELL_TYPE_SYSTEM_HPP_

#include "Navigation.hpp"

namespace plugin {
namespace kate {
namespace russell {

class TypeSystem : public Navigation {
Q_OBJECT
public:
	TypeSystem (KTextEditor::MainWindow*, View*);
	virtual ~ TypeSystem();

	void update();

public Q_SLOTS:
	virtual void refresh();
};

}
}
}

#endif /* PLUGIN_KATE_RUSSELL_TYPE_SYSTEM_HPP_ */

