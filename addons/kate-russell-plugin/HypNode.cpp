/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_ProofNode.cpp                            */
/* Description:     an proof view for a Russell support                      */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2003 by Massimo Callegari <massimocallegari@yahoo.it>*/
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#include <QLineEdit>
#include <QWindow>

#include "View.hpp"
#include "Icon.hpp"

#include "HypNode.moc"

#include "Execute.hpp"
#include "HypNode.hpp"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	HypNode::HypNode() :
	QWidget (nullptr, Qt::SubWindow) {
		ui_.setupUi(this);
	}
	HypNode::~ HypNode() { }
}
