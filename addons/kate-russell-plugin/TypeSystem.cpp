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

#ifndef PLUGIN_KATE_RUSSELL_TYPE_SYSTEM_CPP_
#define PLUGIN_KATE_RUSSELL_TYPE_SYSTEM_CPP_

#include "TypeSystem.moc"
#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	TypeSystem :: TypeSystem 
	(
		KTextEditor::MainWindow* window,
		View* proveView
	) :
	Navigation (window, proveView, TYPE_SYSTEM)
	{
		toolView_ = window_->createToolView 
		(
			proveView->plugin(),
			QStringLiteral("kate_private_plugin_kateproveplugin_type_system"),
			KTextEditor::MainWindow :: Left,
			QIcon (QStringLiteral("application-x-ms-dos-executable")),
			i18n ("Type system")
		);
		tree_ = new QTreeWidget (toolView_);
		popup_ = new QMenu(tree_);

		//showAxioms_ = true;
		//showTheories_ = true;
		//showTypes_ = true;
		//expandTypes_ = true;
		//treeMode_ = true;
		//updateCheckboxes();

		setup ("Type system");

		if (showAxioms_)   showAxioms_->setChecked(true);
		if (showTheories_) showTheories_->setChecked(true);
		if (showTypes_)    showTypes_->setChecked(true);
		if (treeMode_)     treeMode_->setChecked(true);
		expandTypes_ = true;
	}
	TypeSystem :: ~ TypeSystem() {
	}

	void 
	TypeSystem :: update() {
		Navigation :: update ("type_system");
	}

	/****************************
	 *	Public slots
	 ****************************/

	void 
	TypeSystem :: refresh() 
	{
		if (window_->activeView() == NULL) {
			return;
		}
		QString options = getOptions();
		view_->mineTypeSystem (options);
	}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_TYPE_SYSTEM_CPP_ */

