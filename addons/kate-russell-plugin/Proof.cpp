/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Proof.cpp                            */
/* Description:     an proof view for a Russell support                      */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2003 by Massimo Callegari <massimocallegari@yahoo.it>*/
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#include <QLineEdit>
#include <QDialog>
#include <KMessageBox>

#include "Proof.hpp"
#include "View.hpp"
#include "Icon.hpp"

#include "Proof.moc"

#include "commands.hpp"
#include "Execute.hpp"
#include "HypNode.hpp"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	Proof::Proof
	(
		KTextEditor::MainWindow* mainWindow,
		View* russellView
	) :
	QWidget (),
	mainWindow_ (mainWindow),
	russellView_ (russellView),
	proofView_
	(
		mainWindow_->createToolView
		(
			russellView->plugin(),
			QLatin1String("kate_private_plugin_kateproveplugin_proof"),
			KTextEditor::MainWindow :: Left,
			QIcon (QLatin1String("application-x-ms-dos-executable")),
			i18n ("Proof")
		)
	),
	tree_ (new QTreeWidget (proofView_)),
	popup_ (new QMenu (tree_)),

	treeItems_ (),
	root_ (nullptr),
	proofs_ ()
	{
		setupSlotsAndSignals();
		setupLayout();
	}
	Proof::~ Proof() { }

	bool Proof::isProved() const {
		return !proofs_.empty();
	}
	int Proof::getProofIndex() const {
		return 0;
	}

	/****************************
	 *	Public slots
	 ****************************/

	void Proof::slotShowContextMenu(const QPoint& point) {
		if (popup_ != nullptr) {
			popup_->popup(tree_->mapToGlobal(point));
		}
	}
	void Proof::slotShowAssertionVariant(QTableWidgetItem* item) {
		hypNodeView.ui_.assertionTextEdit->setPlainText(
			align_assertion(hypNodeView.propInfoVector[item->row()].assertion)
		);
	}
	void Proof::slotExpandNode() {
		if (QTableWidgetItem* item = hypNodeView.ui_.variantTableWidget->currentItem()) {
			int index = hypNodeView.propInfoVector[item->row()].index;
			Execute::exec(
				QStringList() << QLatin1String("rus prove_step") +
				QLatin1String(" index=") + QString::number(index)
			);
		}
	}
	void Proof::slotDeleteNode() {
		if (QTableWidgetItem* item = hypNodeView.ui_.variantTableWidget->currentItem()) {
			int index = hypNodeView.propInfoVector[item->row()].index;
			Execute::exec(
				QStringList() << QLatin1String("rus prove_delete") +
				QLatin1String(" index=") + QString::number(index)
			);
		}
	}

	void Proof::slotStartProving(const QString& file, int line, int col) {
		tree_->clear();
		treeItems_.clear();
		root_ = nullptr;
		proofs_.clear();
		Execute::exec(command::prove(file, ProvingMode::INTERACTIVE, line, col));
	}
	void Proof::slotChooseVariant(QTreeWidgetItem* item) {
		if (!item || item->text(2) == QLatin1String("prop")) {
			return;
		}
		int index = item->text(1).toInt();
		if (index == -1) {
			return;
		}
		hypNodeView.show();
		hypNodeView.activateWindow();
		hypNodeView.raise();
		Execute::exec(
			QStringList() << QLatin1String("rus prove_info") +
			QLatin1String(" index=") + QString::number(index) +
			QLatin1String(" what=children")
		);
	}
	void Proof::stopProving() {
		Execute::exec(QStringList() << QLatin1String("rus prove_stop"));
		tree_->clear();
		treeItems_.clear();
		root_ = nullptr;
		//hide();
	}
	void Proof::slotProofsInfo() {
		if (QTreeWidgetItem* item = tree_->currentItem()) {
			if (item->text(2) == QLatin1String("prop")) {
				return;
			}
			int index = item->text(1).toInt();
			if (index == -1) {
				return;
			}
			hypNodeView.show();
			hypNodeView.activateWindow();
			hypNodeView.raise();
			Execute::exec(
				QStringList() << QLatin1String("rus prove_info") +
				QLatin1String(" index=") + QString::number(index) +
				QLatin1String(" what=proofs")
			);
		}
	}
	void Proof::slotShow() {
		mainWindow_->showToolView(proofView_);
	}
	void Proof::slotHide() {
		mainWindow_->hideToolView(proofView_);
	}
	void Proof::slotVisibilityChanged(bool visible) {
	}
	void Proof::slotUpdateXML(const QString& XMLSource, const Task& task) {
		if (XMLSource.isEmpty()) {
			return;
		}
		//QTextStream(stdout) << "XMLSource \n\n\n" << XMLSource << "\n\n\n";
		QDomDocument document;
		QString errorMsg;
		int errorLine = 0;
		int errorColumn = 0;
		if (!document.setContent(XMLSource, &errorMsg, &errorLine, &errorColumn)) {
			errorMsg += QLatin1String(" on line ");
			errorMsg += QString::number (errorLine);
			errorMsg += QLatin1String(" on column ");
			errorMsg += QString::number (errorColumn);
			KMessageBox::sorry(0, errorMsg);
			return;
		}
		QDomElement root = document.documentElement();
		QDomNode node = root.firstChild();
		if (root.tagName() == QLatin1String("new")) {
			buildTreeUp(node);
		} else if (root.tagName() == QLatin1String("info")) {
			processInfo(node);
		}
	}

	/****************************
	 *	Private members
	 ****************************/

	void Proof::setupSlotsAndSignals() {
		connect(popup_->addAction(QLatin1String("expand")), SIGNAL(triggered()), this, SLOT(slotExpandTree()));
		connect(popup_->addAction(QLatin1String("proofs")), SIGNAL(triggered()), this, SLOT(slotProofsInfo()));

		connect(tree_, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotShowContextMenu(const QPoint&)));
		connect(tree_, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(slotChooseVariant(QTreeWidgetItem*)));
		connect(proofView_, SIGNAL(toolVisibleChanged(bool)), this, SLOT(slotVisibilityChanged(bool)));
		connect(hypNodeView.ui_.variantTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(slotShowAssertionVariant(QTableWidgetItem*)));
		connect(hypNodeView.ui_.expandNode, SIGNAL(clicked()), this, SLOT(slotExpandNode()));
		connect(hypNodeView.ui_.deleteNode, SIGNAL(clicked()), this, SLOT(slotDeleteNode()));
	}
	void Proof::setupLayout() {
		QStringList titles;
		titles << i18nc("@title:column", "Proof"); // << i18nc ("@title:column", "Position");
		tree_->setColumnCount(1);
		tree_->setHeaderLabels(titles);
		tree_->setColumnHidden(1, true);
		tree_->setSortingEnabled(false);
		tree_->setRootIsDecorated(0);
		tree_->setContextMenuPolicy(Qt::CustomContextMenu);
		tree_->setIndentation(10);
		tree_->setLayoutDirection(Qt::LeftToRight);

		tree_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		tree_->adjustSize();
		tree_->updateGeometry();
		tree_->setContentsMargins(1, 1, 1, 1);
		tree_->setMaximumWidth(100000);
	}

	void Proof::processInfo(QDomNode& outlineNode) {
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("node")) {
					processInfoNode(outlineNode.firstChild());
				} else if (outlineElement.tagName() == QLatin1String("children")) {
					processInfoChildren(outlineNode.firstChild());
				} else if (outlineElement.tagName() == QLatin1String("proofs")) {
					processInfoProofs(outlineNode.firstChild());
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}

	void Proof::processInfoNode(QDomNode outlineNode) {
	}

	void Proof::processInfoChildren(QDomNode outlineNode) {
		QString kind = outlineNode.toElement().attribute(QLatin1String("kind"));
		hypNodeView.setVisible(true);
		hypNodeView.propInfoVector.clear();
		hypNodeView.ui_.variantTableWidget->setRowCount(0);
		hypNodeView.ui_.assertionTextEdit->clear();
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("prop")) {
					PropInfo info = infoProp(outlineNode);
					int row = hypNodeView.ui_.variantTableWidget->rowCount();
					hypNodeView.ui_.variantTableWidget->insertRow(row);
					QTableWidgetItem* assertion = new QTableWidgetItem(info.name);
					Qt::ItemFlags flags = assertion->flags();
					assertion->setFlags(flags & ~Qt::ItemIsEditable);
					hypNodeView.ui_.variantTableWidget->setItem(row, 0, assertion);
					hypNodeView.propInfoVector.append(info);
				} else if (outlineElement.tagName() == QLatin1String("hyp")) {
					HypInfo info = infoHyp(outlineNode);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}

	void Proof::processInfoProofs(QDomNode outlineNode) {
		QString kind = outlineNode.toElement().attribute(QLatin1String("kind"));
		hypNodeView.setVisible(true);
		hypNodeView.propInfoVector.clear();
		hypNodeView.ui_.variantTableWidget->setRowCount(0);
		hypNodeView.ui_.assertionTextEdit->clear();
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("prop")) {
					PropInfo info = infoProp(outlineNode);
					int row = hypNodeView.ui_.variantTableWidget->rowCount();
					hypNodeView.ui_.variantTableWidget->insertRow(row);
					QTableWidgetItem* assertion = new QTableWidgetItem(info.name);
					Qt::ItemFlags flags = assertion->flags();
					assertion->setFlags(flags & ~Qt::ItemIsEditable);
					hypNodeView.ui_.variantTableWidget->setItem(row, 0, assertion);
					hypNodeView.propInfoVector.append(info);
				} else if (outlineElement.tagName() == QLatin1String("hyp")) {
					HypInfo info = infoHyp(outlineNode);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}

	PropInfo Proof::infoProp(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		PropInfo info;
		info.name = outlineElement.attribute(QLatin1String("name"));
		info.hint = (outlineElement.attribute(QLatin1String("hint")) == QLatin1String("+"));
		info.index = outlineElement.attribute(QLatin1String("index")).toInt();
		info.parent = outlineElement.attribute(QLatin1String("parent")).toInt();
		for (const auto& ch : outlineElement.attribute(QLatin1String("children")).split(QLatin1Char(','))) {
			info.premises.append(ch.toInt());
		}
		QDomNode childNode = outlineNode.firstChild();
		QDomElement childElement = childNode.toElement();
		QString assertion;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("assertion")) {
				info.assertion = childElement.text();
			} else if (childElement.tagName() == QLatin1String("substitution")) {
				info.substitution = childElement.text();
			}
			childNode = childNode.nextSibling();
		}
		return info;
	}
	HypInfo Proof::infoHyp(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		HypInfo info;
		info.index = outlineElement.attribute(QLatin1String("index")).toInt();
		info.parent = outlineElement.attribute(QLatin1String("parent")).toInt();
		info.hint = (outlineElement.attribute(QLatin1String("hint")) == QLatin1String("+"));
		for (const auto& ch : outlineElement.attribute(QLatin1String("children")).split(QLatin1Char(','))) {
			info.variants.append(ch.toInt());
		}
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("expression")) {
				info.expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}
		return info;
	}

	void Proof::buildTreeUp (QDomNode& outlineNode) {
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("root")) {
					buildTreeUpRoot (outlineNode);
				} else if (outlineElement.tagName() == QLatin1String("hyp")) {
					buildTreeHyp (outlineNode);
				} else if (outlineElement.tagName() == QLatin1String("prop")) {
					buildTreeProp (outlineNode);
				} else if (outlineElement.tagName() == QLatin1String("ref")) {
					buildTreeRef (outlineNode);
				} /*else if (outlineElement.tagName() == QLatin1String("top")) {
					buildTreeTop (outlineNode);
				}*/
			}
			outlineNode = outlineNode.nextSibling();
		}
	}
	void Proof::buildTreeUpRoot(QDomNode& outlineNode) {
		HypInfo info = infoHyp(outlineNode);
		root_ = new QTreeWidgetItem(tree_);
		treeItems_[info.index] = root_;
		root_->setIcon(0, Icon::root());
		root_->setText(0, info.expression);
		root_->setText(1, QString::number(info.index));
		root_->setText(2, QLatin1String("root"));
		QString toolTip = QLatin1String("index: ");
		toolTip += QString::number(info.index);
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("types: ");
		toolTip += info.types;
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("expression: ");
		toolTip += info.expression;
		toolTip += QLatin1String("\n");
		root_->setToolTip (0, toolTip);
		//item->setText (2, outlineElement.attribute ("line", ""));
		//item->setText (3, outlineElement.attribute ("column", ""));
		//item->setText (4, "yes");
	}
	void Proof::buildTreeHyp(QDomNode& outlineNode) {
		HypInfo info = infoHyp(outlineNode);
		QTreeWidgetItem* parent = treeItems_ [info.parent];
		QTreeWidgetItem* item =	new QTreeWidgetItem(parent);
		treeItems_[info.index] = item;
		item->setIcon(0, Icon::hyp());
		item->setText(0, info.expression);
		item->setText(1, QString::number(info.index));
		item->setText(2, QLatin1String("hyp"));
		QString toolTip;
		if (info.hint) {
			toolTip += QLatin1String("<<< HINT >>>\n");
		}
		toolTip += QLatin1String("index: ");
		toolTip += QString::number(info.index);
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("types: ");
		toolTip += info.types;
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("expression: ");
		toolTip += info.expression;
		toolTip += QLatin1String("\n");
		item->setToolTip (0, toolTip);
		parent->setExpanded(true);
	}
	void Proof::buildTreeProp(QDomNode& outlineNode) {
		PropInfo info = infoProp(outlineNode);
		QTreeWidgetItem* parent = treeItems_ [info.parent];
		QTreeWidgetItem* item =	new QTreeWidgetItem(parent);
		treeItems_ [info.index] = item;
		item->setIcon(0, Icon::prop());
		item->setText(0, info.name);
		item->setText(1, QString::number(info.index));
		item->setText(2, QLatin1String("prop"));
		item->setText(3, info.assertion);
		QString toolTip;
		if (info.hint) {
			toolTip += QLatin1String("<<< HINT >>>\n");
		}
		toolTip += QLatin1String("index: ");
		toolTip += QString::number(info.index);
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("assertion: \n");
		toolTip += info.assertion;
		item->setToolTip(0, toolTip);
		parent->setExpanded(true);
	}
	void Proof::buildTreeRef(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		const bool hint = (outlineElement.attribute(QLatin1String("hint")) == QLatin1String("+"));
		const int index = outlineElement.attribute(QLatin1String("index")).toInt();

		QDomNode childNode = outlineNode.firstChild();
		QDomElement childElement = childNode.toElement();
		int downIndex = 0;
		QString expression;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("expression")) {
				expression = childElement.text();
			} else if (childElement.tagName() == QLatin1String("down")) {
				downIndex  = childElement.attribute(QLatin1String("index")).toInt();
			}
			childNode = childNode.nextSibling();
		}

		QTreeWidgetItem* parent = treeItems_ [downIndex];
		QTreeWidgetItem* item =	new QTreeWidgetItem(parent);
		treeItems_ [index] = item;

		item->setIcon(0, Icon::ref());
		item->setText(0, expression);
		item->setText(1, QString::number(index));
		item->setText(2, QLatin1String("ref"));

		QString toolTip;
		if (hint) {
			toolTip += QLatin1String("<<< HINT >>>\n");
		}
		toolTip += QLatin1String("index: ");
		toolTip += QString::number(index);
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("expression: ");
		toolTip += expression;
		item->setToolTip(0, toolTip);
	}
}
