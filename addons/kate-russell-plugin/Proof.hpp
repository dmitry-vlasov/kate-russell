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

#include <memory>

#include <QWidget>
#include <QPoint>
#include <QTreeWidget>
#include <QTableWidget>
#include <QDomNode>
#include <QMenu>
#include <QAction>

#include <ktexteditor/mainwindow.h>

#include "Enums.hpp"
#include "HypNode.hpp"
#include "ProofInfo.hpp"
#include "ui_ProofsView.h"

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
	void slotExpandNode();
	void slotDeleteNode();

	// Proof tree cultivation
	void slotStartProving(const QString& file, int line, int col);
	void slotChooseVariant(QTreeWidgetItem*);
	void stopProving();
	void slotProofsInfo();
	void slotShow();
	void slotHide();
	void slotVisibilityChanged(bool visible);
	void slotUpdateXML(const QString& XMLSource, const Task& task);
	void slotShowAssertionVariant(QTableWidgetItem* item);
	void slotShowProofVariant(QTableWidgetItem* item);

private :
	void setupSlotsAndSignals();
	void setupLayout();

	HypInfo infoHyp(QDomNode&);
	PropInfo infoProp(QDomNode&);

	void processInfo(QDomNode&);
	void processInfoNode(QDomNode);
	void processInfoChildren(QDomNode);
	void processInfoProofs(QDomNode);

	// Building up a tree structure
	void buildTree      (QDomNode&);
	void buildTreeRoot (QDomNode&);
	void buildTreeHyp   (QDomNode&);
	void buildTreeProp  (QDomNode&);
	void buildTreeRef   (QDomNode&);

	// Proofs are found
	void finalizeProofs(QDomNode&);

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

	HypNode        hypNodeView;
	QWidget        proofsView;
	Ui::ProofsView proofsViewUi;
	QVector<QString> proofsVector;
};

}
