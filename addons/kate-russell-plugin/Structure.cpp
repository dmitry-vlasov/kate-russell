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

#include "Structure.moc"
#include "Structure.hpp"

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
			QLatin1String("kate_private_plugin_kateproveplugin_structure"),
			KTextEditor::MainWindow :: Left,
			QIcon (QLatin1String("application-x-ms-dos-executable")),
			i18n ("Structure")
		);
		tree_ = new TreeWidget (toolView_, this);
		popup_ = new QMenu(tree_);

		setup ("Structure");

		showAxioms_->setChecked(true);
		showTypes_->setChecked(true);
	}
	Structure :: ~ Structure() {
	}

	void 
	Structure :: update(const QString& data) {
		Navigation :: update ("structure", data);
	}

	/****************************
	 *	Public slots
	 ****************************/

	void 
	Structure :: refresh() 
	{
		//if (!window_->activeView()) return;
		QString options = getOptions();
		view_->mineStructure (options);
	}
}
