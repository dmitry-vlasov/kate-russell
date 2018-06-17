/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Proof.hpp                            */
/* Description:     an proof view for a Russell support                      */
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
#include <QTableWidget>
#include <QDomNode>
#include <QMenu>
#include <QAction>

#include <ktexteditor/mainwindow.h>

namespace russell {

class View;

class Proof : public QWidget {
Q_OBJECT
public:
	Proof (KTextEditor::MainWindow*, View*);
	virtual ~ Proof();

	bool isProved() const;
	int  getProofIndex() const;

Q_SIGNALS:
	void proofFound (int proofIndex) const;

public Q_SLOTS:
	void slotShowContextMenu(const QPoint&);
	void expandTree();
	void doNothing();

	// Proof tree cultivation
	void startProving(const QString& file, int line, int col);
	void growTree(QTreeWidgetItem*);
	void stopProving();
	void info();
	void show();
	void hide();
	void visibilityChanged(bool visible);
	void updateXML(const QString& XMLSource);

private :
	void setupSlotsAndSignals();
	void setupLayout();

	// Building up a tree structure
	void buildTree      (QDomNode&);
	void buildTreeUp    (QDomNode&);
	void buildTreeUpRoot (QDomNode&);
	void buildTreeHyp   (QDomNode&);
	void buildTreeProp  (QDomNode&);
	void buildTreeRef   (QDomNode&);
	void buildTreeTop   (QDomNode&);
	void buildRoot      (QDomNode&);
	void buildRootProof (QDomNode&);

	// Showing a node
	void buildNode     (QDomNode&);
	void buildNodeRoot (QDomNode&);
	void buildNodeHyp  (QDomNode&);
	void buildNodeProp (QDomNode&);
	void buildNodeRef  (QDomNode&);
	void buildNodeTop  (QDomNode&);
	QTableWidget* buildNodeEvidences (QDomNode&);

	typedef
		std :: map<int, QTreeWidgetItem*>
		TreeItemVector_;
		
	KTextEditor::MainWindow* mainWindow_;

	View*        russellView_;
	QWidget*     proofView_;
	QTreeWidget* tree_;
	QMenu*       popup_;

	TreeItemVector_  treeItems_;
	QTreeWidgetItem* root_;
	QStringList  proofs_;
};

}
