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

#include "TypeSystem.moc"
#include "TypeSystem.hpp"

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

		setup ("Type system");

		treeMode_->setChecked(true);
		expandTypes_ = true;
	}
	TypeSystem :: ~ TypeSystem() {
	}

	void 
	TypeSystem :: update(const QString& data) {
		Navigation :: update ("structure", data);
	}

	/****************************
	 *	Public slots
	 ****************************/

	void 
	TypeSystem :: refresh() 
	{
		if (!window_->activeView()) return;
		QString options = getOptions();
		view_->mineTypeSystem (options);
	}
}
