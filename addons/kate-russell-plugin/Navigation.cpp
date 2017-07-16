/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Navigation.cpp                       */
/* Description:     an abstract navigaiton view for a Russell support        */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2003 by Massimo Callegari <massimocallegari@yahoo.it>*/
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_NAVIGATION_CPP_
#define PLUGIN_KATE_RUSSELL_NAVIGATION_CPP_

#include "Navigation.moc"
#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	Navigation :: Navigation 
	(
		KTextEditor::MainWindow* window,
		View* view,
		const Sort sort
	) :
	//QWidget (window->centralWidget()),
	QWidget (window->activeView()),
	window_ (window),
	view_ (view),
	sort_ (sort),

	toolView_ (NULL),
	tree_ (NULL),
	popup_ (NULL),

	refresh_(nullptr),
	showAll_ (nullptr),
	showAxioms_ (nullptr),
	showConstants_ (nullptr),
	showDefinitions_ (nullptr),
	showImports_ (nullptr),
	showProblems_ (nullptr),
	showRules_ (nullptr),
	showTheorems_ (nullptr),
	showTheories_ (nullptr),
	showTypes_ (nullptr),
	treeMode_ (nullptr),
	sortingMode_ (nullptr),
	expandTypes_ (false) {
	}
	Navigation :: ~ Navigation() {
	}

	void 
	Navigation :: update (const char* header) 
	{
		const QString output = view_->getOutput();
		QDomDocument document (i18n(header));
		if (!document.setContent (output)) {
			return;
		}
		const QDomElement root = document.documentElement();
		if (root.tagName() != QLatin1String(header)) {
			return;
		}
		QDomNode node = root.firstChild();
		
		tree_->clear();
		QTreeWidgetItem* sibling = NULL;
		build (node, sibling);
	}

	/****************************
	 *	Public slots
	 ****************************/

	void 
	Navigation :: pushShowAll() 
	{
		if (sort_ == TYPE_SYSTEM) return;
		showAxioms_->setChecked(true);
		showConstants_->setChecked(true);
		showDefinitions_->setChecked(true);
		if (sort_ == OUTLINE) {
			showImports_->setChecked(true);
			showProblems_->setChecked(true);
		}
		showRules_->setChecked(true);
		if (sort_ == OUTLINE) {
			showTheorems_->setChecked(true);
		}
		showTheories_->setChecked(true);
		showTypes_->setChecked(true);
		//updateCheckboxes();
		refresh();
	}
/*
	void 
	Navigation :: toggleShowAxioms() 
	{
		showAxioms_ = !showAxioms_;
		popup_->setItemChecked (showAxiomsIndex_, showAxioms_);
		refresh();
	}
	void 
	Navigation :: toggleShowConstants() 
	{
		showConstants_ = !showConstants_;
		popup_->setItemChecked (showConstantsIndex_, showConstants_);
		refresh();
	}
	void 
	Navigation :: toggleShowDefinitions() 
	{
		showDefinitions_ = !showDefinitions_;
		popup_->setItemChecked (showDefinitionsIndex_, showDefinitions_);
		refresh();
	}
	void 
	Navigation :: toggleShowImports() 
	{
		showImports_ = !showImports_;
		popup_->setItemChecked (showImportsIndex_, showImports_);
		refresh();
	}
	void 
	Navigation :: toggleShowProblems() 
	{
		showProblems_ = !showProblems_;
		popup_->setItemChecked (showProblemsIndex_, showProblems_);
		refresh();
	}
	void 
	Navigation :: toggleShowRules() 
	{
		showRules_ = !showRules_;
		popup_->setItemChecked (showRulesIndex_, showRules_);
		refresh();
	}
	void 
	Navigation :: toggleShowTheorems() 
	{
		showTheorems_ = !showTheorems_;
		popup_->setItemChecked (showTheoremsIndex_, showTheorems_);
		refresh();
	}
	void 
	Navigation :: toggleShowTheories() 
	{
		showTheories_ = !showTheories_;
		popup_->setItemChecked (showTheoriesIndex_, showTheories_);
		refresh();
	}
	void 
	Navigation :: toggleShowTypes() 
	{
		showTypes_ = !showTypes_;
		popup_->setItemChecked (showTheoriesIndex_, showTypes_);
		refresh();
	}
	void 
	Navigation :: toggleListTreeMode() 
	{
		treeMode_ = !treeMode_;
		refresh();
	}
	void 
	Navigation :: toggleSortingMode() 
	{
		treeSort_ = !treeSort_;
		popup_->setItemChecked (sortingModeIndex_, treeSort_);
		refresh();
	}
*/
	void 
	Navigation :: slotShowContextMenu (const QPoint& point) {
		popup_-> popup (tree_->mapToGlobal (point));
	}
	void 
	Navigation :: gotoDefinition (QTreeWidgetItem* item)
	{
		const QString& locate = item->text (4);
		if (locate == QStringLiteral("no")) {
			return;
		}
		QString path = item->text (1);
		const int line = item->text (2).toInt ();
		const int column = item->text (3).toInt ();
		view_->gotoLocation (path, line, column);
	}

	/****************************
	 *	Protected members
	 ****************************/

	void
	Navigation :: setup (const char* header)
	{
		/*refresh_ = popup_->insertItem (i18n ("Refresh"), this, SLOT (refresh()));

		if (sort_ != TYPE_SYSTEM) {	
			popup_->addSeparator();
			showAllIndex_ = popup_->insertItem (i18n("Show All"), this, SLOT (pushShowAll()));
			showAxiomsIndex_ = popup_->insertItem (i18n("Show Axioms"), this, SLOT (toggleShowAxioms()));
			showConstantsIndex_ = popup_->insertItem (i18n("Show Constants"), this, SLOT (toggleShowConstants()));
			showDefinitionsIndex_ = popup_->insertItem (i18n("Show Definitions"), this, SLOT (toggleShowDefinitions()));
			if (sort_ == OUTLINE) {
				showImportsIndex_ = popup_->insertItem (i18n("Show Imports"), this, SLOT (toggleShowImports()));
				showProblemsIndex_ = popup_->insertItem (i18n("Show Problems"), this, SLOT (toggleShowProblems()));
			}			
			showRulesIndex_ = popup_->insertItem (i18n("Show Rules"), this, SLOT (toggleShowRules()));
			if (sort_ == OUTLINE) {
				showTheoremsIndex_ = popup_->insertItem (i18n("Show Theorems"), this, SLOT (toggleShowTheorems()));
			}
			showTheoriesIndex_ = popup_->insertItem (i18n("Show Theories"), this, SLOT (toggleShowTheories()));
			showTypesIndex_ = popup_->insertItem (i18n("Show Types"), this, SLOT (toggleShowTypes()));
		}
		
		popup_->addSeparator();
		listTreeModeIndex_ = popup_->insertItem (i18n ("List/Tree Mode"), this, SLOT (toggleListTreeMode()));
		sortingModeIndex_ = popup_->insertItem(i18n("Enable Sorting"), this, SLOT (toggleSortingMode()));
		*/

		refresh_ = popup_->addAction(i18n("Refresh"));
		refresh_->setCheckable(true);
		connect(refresh_, SIGNAL(triggered()), this, SLOT (refresh()));

		if (sort_ != TYPE_SYSTEM) {
			popup_->addSeparator();
			showAll_ = popup_->addAction(i18n("Show All"));
			showAll_->setCheckable(true);
			showAxioms_ = popup_->addAction(i18n("Show Axioms"));
			showAxioms_->setCheckable(true);
			showConstants_ = popup_->addAction(i18n("Show Constants"));
			showConstants_->setCheckable(true);
			showDefinitions_ = popup_->addAction(i18n("Show Definitions"));
			showDefinitions_->setCheckable(true);
			if (sort_ == OUTLINE) {
				showImports_ = popup_->addAction(i18n("Show Imports"));
				showImports_->setCheckable(true);
				showProblems_ = popup_->addAction(i18n("Show Problems"));
				showProblems_->setCheckable(true);
			}
			showRules_ = popup_->addAction(i18n("Show Rules"));
			showRules_->setCheckable(true);
			if (sort_ == OUTLINE) {
				showTheorems_ = popup_->addAction(i18n("Show Theorems"));
				showTheorems_->setCheckable(true);
			}
			showTheories_ = popup_->addAction(i18n("Show Theories"));
			showTheories_->setCheckable(true);
			showTypes_ = popup_->addAction(i18n("Show Types"));
			showTypes_->setCheckable(true);
		}

		popup_->addSeparator();
		treeMode_ = popup_->addAction(i18n ("List/Tree Mode"));
		treeMode_->setCheckable(true);
		sortingMode_ = popup_->addAction(i18n("Enable Sorting"));
		sortingMode_->setCheckable(true);

		//updateCheckboxes();
	
		QStringList titles;
		titles << i18nc ("@title:column", header) << i18nc ("@title:column", "Position");
		tree_->setColumnCount (2);
		tree_->setHeaderLabels (titles);
		tree_->setColumnHidden (1, true);
		tree_->setSortingEnabled (false);
		tree_->setRootIsDecorated (0);
		tree_->setContextMenuPolicy (Qt :: CustomContextMenu);
		tree_->setIndentation (10);
		tree_->setLayoutDirection (Qt :: LeftToRight);

		connect (tree_, SIGNAL (customContextMenuRequested (const QPoint&)), this, SLOT (slotShowContextMenu (const QPoint&)));
		connect (tree_, SIGNAL (itemActivated (QTreeWidgetItem*, int)), this, SLOT (gotoDefinition (QTreeWidgetItem*)));
	}
	QString 
	Navigation :: getOptions() const
	{
		/*
		 {XmlNode::IMPORT,  {"import",  IMPORT_BIT}},
	{XmlNode::CONST,   {"const",   CONST_BIT}},
	{XmlNode::TYPE,    {"type",    TYPE_BIT}},
	{XmlNode::RULE,    {"rule",    RULE_BIT}},
	{XmlNode::AXIOM,   {"axiom",   AXIOM_BIT}},
	{XmlNode::DEF,     {"def",     DEF_BIT}},
	{XmlNode::THEOREM, {"theorem", THEOREM_BIT}},
	{XmlNode::PROOF,   {"proof",   PROOF_BIT}},
	{XmlNode::THEORY,  {"theory",  THEORY_BIT}},
	{XmlNode::PROBLEM, {"problem", PROBLEM_BIT}}
		 */

		QString options;
		if (showAxioms_ && showAxioms_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("axiom");
		}
		if (showConstants_ && showConstants_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("const");
		}
		if (showDefinitions_ && showDefinitions_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("def");
		}
		if (showImports_ && showImports_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("import");
		}
		if (showProblems_ && showProblems_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("problem");
		}
		if (showRules_ && showRules_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("rule");
		}
		if (showTheorems_ && showTheorems_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("theorem");
		}
		if (showTheories_ && showTheories_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("theory");
		}
		if (showTypes_ && showTypes_->isChecked()) {
			if (options.size()) options += QStringLiteral(",");
			options += QStringLiteral("type");
		}
		return options;
	}
	void 
	Navigation :: build (QDomNode& outlineNode, QTreeWidgetItem*& sibling, QTreeWidgetItem* parent) 
	{
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QStringLiteral("import")) {
					buildItem (outlineNode, sibling, IMPORT, parent);
				} else if (outlineElement.tagName() == QStringLiteral("theory")) {
					buildTree (outlineNode, sibling, THEORY, parent);
				} else if (outlineElement.tagName() == QStringLiteral("contents")) {
					buildTree (outlineNode, sibling, CONTENTS, parent);
				} else if (outlineElement.tagName() == QStringLiteral("constant")) {
					buildItem (outlineNode, sibling, CONSTANT, parent);
				} else if (outlineElement.tagName() == QStringLiteral("type")) {
					if (expandTypes_) {
						buildTree (outlineNode, sibling, TYPE, parent, true);
					} else {
						buildItem (outlineNode, sibling, TYPE, parent);
					}
				} else if (outlineElement.tagName() == QStringLiteral("rule")) {
					buildItem (outlineNode, sibling, RULE, parent);
				} else if (outlineElement.tagName() == QStringLiteral("axiom")) {
					buildItem (outlineNode, sibling, AXIOM, parent);
				} else if (outlineElement.tagName() == QStringLiteral("definition")) {
					buildItem (outlineNode, sibling, DEFINITION, parent);
				} else if (outlineElement.tagName() == QStringLiteral("theorem")) {
					buildItem (outlineNode, sibling, THEOREM, parent);
				} else if (outlineElement.tagName() == QStringLiteral("problem")) {
					buildItem (outlineNode, sibling, PROBLEM, parent);
				} else if (outlineElement.tagName() == QStringLiteral("proof")) {
					buildItem (outlineNode, sibling, PROOF, parent);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}
	void 
	Navigation :: buildItem
	(
		QDomNode& outlineNode,
		QTreeWidgetItem*& sibling,
		const Kind kind,
		QTreeWidgetItem* parent
	)
	{
		QTreeWidgetItem* item = 
			(treeMode_ && treeMode_->isChecked() && (parent != NULL)) ?
			new QTreeWidgetItem (parent, sibling) :
			new QTreeWidgetItem (tree_);
		sibling = item;
		item->setIcon (0, Icon :: get (kind));
		QDomElement outlineElement = outlineNode.toElement();
		item->setText (0, outlineElement.attribute (QStringLiteral("name"), QStringLiteral("")));
		item->setText (1, outlineElement.attribute (QStringLiteral("path"), QStringLiteral("")));
		item->setText (2, outlineElement.attribute (QStringLiteral("line"), QStringLiteral("")));
		item->setText (3, outlineElement.attribute (QStringLiteral("col"), QStringLiteral("")));
		item->setText (4, QStringLiteral("yes"));
	}
	void 
	Navigation :: buildTree
	(
		QDomNode& outlineNode,
		QTreeWidgetItem*& sibling,
		const Kind kind,
		QTreeWidgetItem* parent,
		const bool gotoDefinition
	)
	{
		QTreeWidgetItem* item =
			(treeMode_ && treeMode_->isChecked() && (parent != NULL)) ?
			new QTreeWidgetItem (parent, sibling) :
			new QTreeWidgetItem (tree_);
		sibling = item;
		item->setIcon (0, Icon :: get (kind));
		tree_->expandItem (item);
		QDomElement outlineElement = outlineNode.toElement();
		item->setText (0, outlineElement.attribute (QStringLiteral("name"), QStringLiteral("")));
		item->setText (1, outlineElement.attribute (QStringLiteral("path"), QStringLiteral("")));
		item->setText (2, outlineElement.attribute (QStringLiteral("line"), QStringLiteral("")));
		item->setText (3, outlineElement.attribute (QStringLiteral("col"), QStringLiteral("")));
		if (gotoDefinition) {
			item->setText (4, QStringLiteral("yes"));
		} else {
			item->setText (4, QStringLiteral("no"));
		}
		QDomNode childNode = outlineNode.firstChild();
		QTreeWidgetItem* childSibling = NULL;
		build (childNode, childSibling, item);
	}
/*
	void 
	Navigation :: updateCheckboxes() 
	{
		popup_->setItemChecked (showAxiomsIndex_, showAxioms_);
		popup_->setItemChecked (showConstantsIndex_, showConstants_);
		popup_->setItemChecked (showDefinitionsIndex_, showDefinitions_);
		popup_->setItemChecked (showImportsIndex_, showImports_);
		popup_->setItemChecked (showProblemsIndex_, showProblems_);
		popup_->setItemChecked (showRulesIndex_, showRules_);
		popup_->setItemChecked (showTheoremsIndex_, showTheorems_);
		popup_->setItemChecked (showTheoriesIndex_, showTheories_);
		popup_->setItemChecked (showTypesIndex_, showTypes_);

		popup_->setItemChecked (sortingModeIndex_, treeSort_);
	}
*/
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_NAVIGATION_CPP_ */
