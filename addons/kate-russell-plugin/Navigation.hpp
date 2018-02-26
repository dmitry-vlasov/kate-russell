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

#pragma once

#include <QWidget>
#include <QPoint>
#include <QTreeWidget>
#include <QDomNode>
#include <QMenu>
#include <QAction>

#include <KTextEditor/MainWindow>

#include "Kind.hpp"
#include "View.hpp"

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

	void update (const char*, const QString& data);

public Q_SLOTS:
	virtual void refresh() = 0;

	void pushShowAll();
	void toggleTreeMode();
	void toggleSortingMode();

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

	struct TreeWidget : public QTreeWidget {
		TreeWidget(QWidget*, Navigation*);
		void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
		Navigation* navigation;
	};
		
	KTextEditor::MainWindow* window_;
	View*       view_;
	const Sort  sort_;
	QWidget*    toolView_;
	TreeWidget* tree_;

	QMenu*      popup_;
	QAction*    refresh_;
	QAction*    showAll_;
	QAction*    showAxioms_;
	QAction*    showConstants_;
	QAction*    showDefinitions_;
	QAction*    showImports_;
	QAction*    showProblems_;
	QAction*    showRules_;
	QAction*    showTheorems_;
	QAction*    showTheories_;
	QAction*    showTypes_;
	QAction*    treeMode_;
	QAction*    sortingMode_;

	bool expandTypes_;
};

}
