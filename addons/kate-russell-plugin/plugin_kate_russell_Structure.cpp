/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Structure.cpp                        */
/* Description:     a structure view for a Russell support plugin for Kate   */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2003 by Massimo Callegari <massimocallegari@yahoo.it>*/
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_STRUCTURE_CPP_
#define PLUGIN_KATE_RUSSELL_STRUCTURE_CPP_

#include "plugin_kate_russell.hpp"
#include "plugin_kate_russell_Structure.moc"

namespace plugin {
namespace kate {
namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	Structure :: Structure 
	(
		KTextEditor::MainWindow* window,
		View* proveView
	) :
	Navigation (window, proveView, STRUCTURE)
	{
		toolView_ = window_->createToolView 
		(
			proveView->plugin(),
			QStringLiteral("kate_private_plugin_kateproveplugin_structure"),
			KTextEditor::MainWindow :: Left,
			QIcon (QStringLiteral("application-x-ms-dos-executable")),
			i18n ("Structure")
		);
		tree_ = new QTreeWidget (toolView_);
		popup_ = new QMenu(tree_);

		//showAxioms_ = true;
		//showTheories_ = true;
		//updateCheckboxes();

		setup ("Structure");

		showAxioms_->setChecked(true);
		showTheories_->setChecked(true);
	}
	Structure :: ~ Structure() {
	}

	void 
	Structure :: update() {
		Navigation :: update ("structure");
	}

	/****************************
	 *	Public slots
	 ****************************/

	void 
	Structure :: refresh() 
	{
		if (window_->activeView() == NULL) {
			return;
		}
		QString options = getOptions();
		view_->mineStructure (options);
	}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_STRUCTURE_CPP_ */

