/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Navigation.hpp                       */
/* Description:     an abstract navigaiton view for a Russell support        */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2003 by Massimo Callegari <massimocallegari@yahoo.it>*/
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_NAVIGATION_HPP_
#define PLUGIN_KATE_RUSSELL_NAVIGATION_HPP_

#include "Kind.hpp"
#include "russell.dpp"
#include "x_include.hpp"

namespace plugin {
namespace kate {
namespace russell {

class Navigation : public QWidget {
Q_OBJECT
public:
	enum Sort {
		OUTLINE,
		STRUCTURE, 
		TYPE_SYSTEM
	};

	Navigation (KTextEditor::MainWindow*, View*, const Sort);
	virtual ~ Navigation();

	void update (const char*);

public Q_SLOTS:
	virtual void refresh() = 0;

	void pushShowAll();
/*	void toggleShowAxioms();
	void toggleShowConstants();
	void toggleShowDefinitions();
	void toggleShowImports();
	void toggleShowProblems();
	void toggleShowRules();
	void toggleShowTheorems();
	void toggleShowTheories();
	void toggleShowTypes();

	void toggleListTreeMode();
	void toggleSortingMode();
*/
	void slotShowContextMenu (const QPoint&);
	void gotoDefinition (QTreeWidgetItem*);

protected :
	void setup (const char*);
	QString getOptions() const;
	void build (QDomNode&, QTreeWidgetItem*&, QTreeWidgetItem* = NULL);
	void buildItem (QDomNode&, QTreeWidgetItem*&, const Kind, QTreeWidgetItem* = NULL);
	void buildTree
	(
		QDomNode&,
		QTreeWidgetItem*&,
		const Kind,
		QTreeWidgetItem*,
		const bool gotoDefinition = false
	);
	//void updateCheckboxes();
		
	KTextEditor::MainWindow* window_;
	View*        view_;
	const Sort   sort_;
	QWidget*     toolView_;
	QTreeWidget* tree_;

	QMenu*       popup_;
	QAction*     refresh_;
	QAction*     showAll_;
	QAction*     showAxioms_;
	QAction*     showConstants_;
	QAction*     showDefinitions_;
	QAction*     showImports_;
	QAction*     showProblems_;
	QAction*     showRules_;
	QAction*     showTheorems_;
	QAction*     showTheories_;
	QAction*     showTypes_;
	QAction*     treeMode_;
	QAction*     sortingMode_;



//	bool treeMode_;
//	bool treeSort_;
/*
	bool showAxioms_;
	bool showConstants_;
	bool showDefinitions_;
	bool showImports_;
	bool showProblems_;
	bool showRules_;
	bool showTheorems_;
	bool showTheories_;
	bool showTypes_;

	int showAllIndex_;
	int showAxiomsIndex_;
	int showConstantsIndex_;
	int showDefinitionsIndex_;
	int showImportsIndex_;
	int showProblemsIndex_;
	int showRulesIndex_;
	int showTheoremsIndex_;
	int showTheoriesIndex_;
	int showTypesIndex_;
*/
//	int listTreeModeIndex_;
//	int sortingModeIndex_;

	bool expandTypes_;
};

}
}
}

#endif /* PLUGIN_KATE_RUSSELL_NAVIGATION_HPP_ */

