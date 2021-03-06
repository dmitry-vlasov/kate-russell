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

#include <QMessageBox>

#include <KTextEditor/View>

#include "Icon.hpp"
#include "Navigation.moc"
#include "Navigation.hpp"

namespace russell {

inline QString de_escape_xml(const QString& s) {
	QString str;
	for (int i = 0; i < s.length(); ++ i) {
		if (s[i] == QLatin1Char('&')) {
			++i;
			if (s.mid(i, 4) == QLatin1String("quot")) {
				i += 4;
				str += QLatin1Char('\"');
			} else if (s.mid(i, 4) == QLatin1String("apos")) {
				i += 4;
				str += QLatin1Char('\'');
			} else if (s.mid(i, 3) == QLatin1String("amp")) {
				i += 3;
				str += QLatin1Char('&');
			} else if (s.mid(i, 2) == QLatin1String("lt")) {
				i += 2;
				str += QLatin1Char('<');
			} else if (s.mid(i, 2) == QLatin1String("gt")) {
				i += 2;
				str += QLatin1Char('>');
			} else {
				str += QLatin1Char('&'); str += s[i];
			}
		} else str += s[i];
	}
	return str;
}

	/****************************
	 *	Public members
	 ****************************/

	Navigation :: Navigation 
	(
		KTextEditor::MainWindow* window,
		View* view,
		const Sort sort
	) :
	QWidget(window->activeView()),
	window_ (window),
	view_ (view),
	sort_ (sort),

	toolView_ (nullptr),
	tree_ (nullptr),
	popup_ (nullptr),

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
	Navigation :: update (const char* header, const QString& output)
	{
		if (!output.size()) return;
		QDomDocument document(i18n(header));
		QString error;

		//QTextStream(stdout) << "DATA: \n" << output << "\n";

		if (!document.setContent (output, &error)) {
			QMessageBox::information(this, QLatin1String("Couldn't parse xml"), error);
			return;
		}
		const QDomElement root = document.documentElement();
		if (root.tagName() != QLatin1String(header)) {
			return;
		}
		QDomNode node = root.firstChild();
		
		tree_->clear();
		QTreeWidgetItem* sibling = nullptr;
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
		refresh();
	}

	void 
	Navigation :: toggleTreeMode() {
		refresh();
	}
	void
	Navigation :: toggleSortingMode() {
		//sortingMode_->setChecked(treeMode_->isChecked());
		refresh();
	}

	void
	Navigation :: slotShowContextMenu (const QPoint& point) {
		popup_-> popup (tree_->mapToGlobal (point));
	}
	void 
	Navigation :: gotoDefinition (QTreeWidgetItem* item)
	{
		const QString& locate = item->text (4);
		if (locate == QLatin1String("no")) {
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
		refresh_ = popup_->addAction(i18n("Refresh"));
		connect(refresh_, SIGNAL(triggered()), this, SLOT (refresh()));

		if (sort_ != TYPE_SYSTEM) {
			popup_->addSeparator();
			showAll_ = popup_->addAction(i18n("Show All"));
			connect(showAll_, SIGNAL(triggered()), this, SLOT (pushShowAll()));

			showAxioms_ = popup_->addAction(i18n("Show Axioms"));
			showAxioms_->setCheckable(true);
			connect(showAxioms_, SIGNAL(triggered()), this, SLOT (refresh()));
			showConstants_ = popup_->addAction(i18n("Show Constants"));
			showConstants_->setCheckable(true);
			connect(showConstants_, SIGNAL(triggered()), this, SLOT (refresh()));
			showDefinitions_ = popup_->addAction(i18n("Show Definitions"));
			showDefinitions_->setCheckable(true);
			connect(showDefinitions_, SIGNAL(triggered()), this, SLOT (refresh()));
			if (sort_ == OUTLINE) {
				showImports_ = popup_->addAction(i18n("Show Imports"));
				showImports_->setCheckable(true);
				connect(showImports_, SIGNAL(triggered()), this, SLOT (refresh()));
				showProblems_ = popup_->addAction(i18n("Show Problems"));
				showProblems_->setCheckable(true);
				connect(showProblems_, SIGNAL(triggered()), this, SLOT (refresh()));
			}
			showRules_ = popup_->addAction(i18n("Show Rules"));
			showRules_->setCheckable(true);
			connect(showRules_, SIGNAL(triggered()), this, SLOT (refresh()));
			if (sort_ == OUTLINE) {
				showTheorems_ = popup_->addAction(i18n("Show Theorems"));
				showTheorems_->setCheckable(true);
				connect(showTheorems_, SIGNAL(triggered()), this, SLOT (refresh()));
			}
			showTheories_ = popup_->addAction(i18n("Show Theories"));
			showTheories_->setCheckable(true);
			connect(showTheories_, SIGNAL(triggered()), this, SLOT (refresh()));
			showTypes_ = popup_->addAction(i18n("Show Types"));
			showTypes_->setCheckable(true);
			connect(showTypes_, SIGNAL(triggered()), this, SLOT (refresh()));
		}

		popup_->addSeparator();
		treeMode_ = popup_->addAction(i18n ("List/Tree Mode"));
		treeMode_->setCheckable(true);
		connect(treeMode_, SIGNAL(triggered()), this, SLOT (toggleTreeMode()));
		sortingMode_ = popup_->addAction(i18n("Enable Sorting"));
		sortingMode_->setCheckable(true);
		connect(sortingMode_, SIGNAL(triggered()), this, SLOT (toggleSortingMode()));
	
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
		if (sort_ == TYPE_SYSTEM) return QLatin1String("type");
		QString options;
		if (showAxioms_ && showAxioms_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("axiom");
		}
		if (showConstants_ && showConstants_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("const");
		}
		if (showDefinitions_ && showDefinitions_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("def");
		}
		if (showImports_ && showImports_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("import");
		}
		if (showProblems_ && showProblems_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("problem");
		}
		if (showRules_ && showRules_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("rule");
		}
		if (showTheorems_ && showTheorems_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("theorem");
		}
		if (showTheories_ && showTheories_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("theory");
		}
		if (showTypes_ && showTypes_->isChecked()) {
			if (options.size()) options += QLatin1String(",");
			options += QLatin1String("type");
		}
		return options;
	}
	void 
	Navigation :: build (QDomNode& outlineNode, QTreeWidgetItem*& sibling, QTreeWidgetItem* parent) 
	{
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("import")) {
					buildItem (outlineNode, sibling, IMPORT, parent);
				} else if (outlineElement.tagName() == QLatin1String("theory")) {
					buildTree (outlineNode, sibling, THEORY, parent);
				} else if (outlineElement.tagName() == QLatin1String("contents")) {
					buildTree (outlineNode, sibling, CONTENTS, parent);
				} else if (outlineElement.tagName() == QLatin1String("constant")) {
					buildItem (outlineNode, sibling, CONSTANT, parent);
				} else if (outlineElement.tagName() == QLatin1String("type")) {
					if (expandTypes_) {
						buildTree (outlineNode, sibling, TYPE, parent, true);
					} else {
						buildItem (outlineNode, sibling, TYPE, parent);
					}
				} else if (outlineElement.tagName() == QLatin1String("rule")) {
					buildItem (outlineNode, sibling, RULE, parent);
				} else if (outlineElement.tagName() == QLatin1String("axiom")) {
					buildItem (outlineNode, sibling, AXIOM, parent);
				} else if (outlineElement.tagName() == QLatin1String("definition")) {
					buildItem (outlineNode, sibling, DEFINITION, parent);
				} else if (outlineElement.tagName() == QLatin1String("theorem")) {
					buildItem (outlineNode, sibling, THEOREM, parent);
				} else if (outlineElement.tagName() == QLatin1String("problem")) {
					buildItem (outlineNode, sibling, PROBLEM, parent);
				} else if (outlineElement.tagName() == QLatin1String("proof")) {
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
			(treeMode_ && treeMode_->isChecked() && parent) ?
			new QTreeWidgetItem (parent, sibling) :
			new QTreeWidgetItem (tree_);
		sibling = item;
		item->setIcon (0, Icon :: get (kind));
		QDomElement outlineElement = outlineNode.toElement();
		if (kind != CONSTANT) {
			item->setText (0, de_escape_xml(outlineElement.attribute (QLatin1String("name"), QLatin1String(""))));
		} else {
			item->setText (0, de_escape_xml(outlineElement.text().trimmed()));
		}
		item->setText (1, de_escape_xml(outlineElement.attribute (QLatin1String("path"), QLatin1String(""))));
		item->setText (2, outlineElement.attribute (QLatin1String("line"), QLatin1String("")));
		item->setText (3, outlineElement.attribute (QLatin1String("col"), QLatin1String("")));
		item->setText (4, QLatin1String("yes"));
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
			(treeMode_ && treeMode_->isChecked() && parent) ?
			new QTreeWidgetItem (parent, sibling) :
			new QTreeWidgetItem (tree_);
		sibling = item;
		item->setIcon (0, Icon :: get (kind));
		tree_->expandItem (item);
		QDomElement outlineElement = outlineNode.toElement();
		item->setText (0, de_escape_xml(outlineElement.attribute (QLatin1String("name"), QLatin1String(""))));
		item->setText (1, de_escape_xml(outlineElement.attribute (QLatin1String("path"), QLatin1String(""))));
		item->setText (2, outlineElement.attribute (QLatin1String("line"), QLatin1String("")));
		item->setText (3, outlineElement.attribute (QLatin1String("col"), QLatin1String("")));
		if (gotoDefinition) {
			item->setText (4, QLatin1String("yes"));
		} else {
			item->setText (4, QLatin1String("no"));
		}
		QDomNode childNode = outlineNode.firstChild();
		QTreeWidgetItem* childSibling = nullptr;
		build (childNode, childSibling, item);
	}

	Navigation :: TreeWidget :: TreeWidget(QWidget* parent, Navigation* n) :
		QTreeWidget(parent), navigation(n) { }

	void
	Navigation :: TreeWidget :: showEvent(QShowEvent *event) {
		navigation->refresh();
		QTreeView::showEvent(event);
	}
}
