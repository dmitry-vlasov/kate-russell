/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Outline.cpp                          */
/* Description:     an outline view for a Russell support plugin for Kate    */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2003 by Massimo Callegari <massimocallegari@yahoo.it>*/
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_OUTLINE_CPP_
#define PLUGIN_KATE_RUSSELL_OUTLINE_CPP_

#include "plugin_kate_russell.hpp"
#include "plugin_kate_russell_Outline.moc"

namespace plugin {
namespace kate {
namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	Outline :: Outline 
	(
		KTextEditor::MainWindow* window,
		View* proveView
	) :
	Navigation (window, proveView, OUTLINE)
	{
		toolView_ = window_->createToolView 
		(
			proveView->plugin(),
			QStringLiteral("kate_private_plugin_kateproveplugin_outline"),
			KTextEditor::MainWindow :: Left,
			QIcon (QStringLiteral("application-x-ms-dos-executable")),
			i18n ("Outline")
		);
		tree_ = new QTreeWidget (toolView_);
		popup_ = new QMenu(tree_);
/*
		showAxioms_ = true;
		showConstants_ = true;
		showDefinitions_ = true;
		showImports_ = true;
		showProblems_ = true;
		showRules_ = true;
		showTheorems_ = true;
		showTheories_ = true;
		showTypes_ = true;
*/
		setup ("Outline");

		showAxioms_->setChecked(true);
		showConstants_->setChecked(true);
		showDefinitions_->setChecked(true);
		showImports_->setChecked(true);
		showProblems_->setChecked(true);
		showRules_->setChecked(true);
		showTheorems_->setChecked(true);
		showTheories_->setChecked(true);
		showTypes_->setChecked(true);
	}
	Outline :: ~ Outline() {
	}

	void 
	Outline :: update() {
		Navigation :: update ("outline");
	}

	/****************************
	 *	Public slots
	 ****************************/

	void 
	Outline :: refresh() 
	{
		if (window_->activeView() == NULL) {
			return;
		}
		QString options = getOptions();
		view_->mineOutline (options);
	}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_OUTLINE_CPP_ */

