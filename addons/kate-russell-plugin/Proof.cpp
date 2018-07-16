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
#include "ProofNode.hpp"

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
		nodeView.reset(new ProofNode());
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
			popup_->popup (tree_->mapToGlobal (point));
		}
	}
	void Proof::slotShowAssertionVariant(QTableWidgetItem* item) {
		nodeView.get()->ui_.assertionTextEdit->setPlainText(
			align_assertion(nodeView.get()->propInfoVector[item->row()].assertion)
		);
	}

	void Proof::slotExpandTree() {
		for (int i = 0; i < treeItems_.size(); ++ i) {
			if (treeItems_[i]) {
				treeItems_[i]->setExpanded(true);
			}
		}
	}
	void Proof::slotDoNothing() {
	}
	void Proof::slotExpandNode() {
		if (QTableWidgetItem* item = nodeView.get()->ui_.variantTableWidget->currentItem()) {
			int index = nodeView.get()->propInfoVector[item->row()].index;
			Execute::exec(
				QStringList() << QLatin1String("rus prove_step") +
				QLatin1String(" index=") + QString::number(index)
			);
		}
	}
	void Proof::slotDeleteNode() {
		if (QTableWidgetItem* item = nodeView.get()->ui_.variantTableWidget->currentItem()) {
			int index = nodeView.get()->propInfoVector[item->row()].index;
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
	void Proof::slotGrowTree(QTreeWidgetItem* item) {
		if (!item) {
			return;
		}
		const QString indexString = item->text(1);
		const int index = indexString.toInt();
		if (index == -1) {
			return;
		}
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
	void Proof::slotInfo() {
		if (QTreeWidgetItem* item = tree_->currentItem()) {
			const QString indexString = item->text(1);
			const int index = indexString.toInt();
			if (index == -1) {
				return;
			}
			Execute::exec(
				QStringList() << QLatin1String("rus prove_info") +
				QLatin1String(" index=") + QString::number(index) +
				QLatin1String(" what=children")
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
			buildTree(node);
		} else if (root.tagName() == QLatin1String("node")) {
			buildNode(node);
		} else if (root.tagName() == QLatin1String("info")) {
			processInfo(node);
		}
	}

	/****************************
	 *	Private members
	 ****************************/

	void Proof::setupSlotsAndSignals() {
		QAction* showAct = new QAction(QIcon(QLatin1String("open.png")), tr("&show..."), popup_);
		//showAct->setShortcuts(QKeySequence::Open);
		showAct->setStatusTip(tr("Show complete info"));
		connect(showAct, SIGNAL(triggered()), this, SLOT(slotInfo()));
		popup_->insertAction(nullptr, showAct);
/*
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("expand"), this, SLOT (expandTree()));
*/
		QAction* expand = popup_->addAction(QLatin1String("expand"));

		connect(expand, SIGNAL(changed()), this, SLOT(slotExpandTree()));

		connect(tree_, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotShowContextMenu(const QPoint&)));
		connect(tree_, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(slotGrowTree(QTreeWidgetItem*)));
		connect(proofView_, SIGNAL(toolVisibleChanged(bool)), this, SLOT(slotVisibilityChanged(bool)));
		connect(nodeView.get()->ui_.variantTableWidget, SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(slotShowAssertionVariant(QTableWidgetItem*)));
		connect(nodeView.get()->ui_.expandNode, SIGNAL(clicked()), this, SLOT(slotExpandNode()));
		connect(nodeView.get()->ui_.deleteNode, SIGNAL(clicked()), this, SLOT(slotDeleteNode()));
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

		//proofView_->setMaximumWidth(100000);
		//proofView_->etSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
		//proofView_->setContentsMargins(1, 1, 1, 1);
	}

	void Proof::processInfo(QDomNode& outlineNode) {
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("node")) {
					processInfoNode(outlineNode.firstChild());
				} else if (outlineElement.tagName() == QLatin1String("children")) {
					processInfoChildren(outlineNode.firstChild());
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}

	void Proof::processInfoNode(QDomNode outlineNode) {
	}

	void Proof::processInfoChildren(QDomNode outlineNode) {
		QString kind = outlineNode.toElement().attribute(QLatin1String("kind"));
		nodeView.get()->setVisible(true);
		nodeView.get()->propInfoVector.clear();
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("prop")) {
					PropInfo info = infoProp(outlineNode);
					int row = nodeView->ui_.variantTableWidget->rowCount();
					nodeView.get()->ui_.variantTableWidget->insertRow(row);
					QTableWidgetItem* assertion = new QTableWidgetItem(info.name);
					Qt::ItemFlags flags = assertion->flags();
					assertion->setFlags(flags & ~Qt::ItemIsEditable);
					nodeView.get()->ui_.variantTableWidget->setItem(row, 0, assertion);
					nodeView.get()->propInfoVector.append(info);
					//QTextStream(stdout) << "A: " << info.assertion << "\n";
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

	void Proof::buildTree(QDomNode& outlineNode) {
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				buildTreeUp (outlineElement);
			}
			outlineNode = outlineNode.nextSibling();
		}
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
				} else if (outlineElement.tagName() == QLatin1String("top")) {
					buildTreeTop (outlineNode);
				}
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
	void Proof::buildTreeTop(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		const QString reference = outlineElement.attribute(QLatin1String("reference"));
		const QString types = outlineElement.attribute(QLatin1String("types"));
		const bool hint = (outlineElement.attribute(QLatin1String("hint")) == QLatin1String("+"));
		const int index = outlineElement.attribute(QLatin1String("index")).toInt();
		const int downIndex = outlineElement.attribute(QLatin1String("down")).toInt();

		QString expression;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}
		QTreeWidgetItem* parent = treeItems_ [downIndex];
		QTreeWidgetItem* item =	new QTreeWidgetItem(parent);
		treeItems_ [index] = item;

		item->setIcon(0, Icon::top());
		item->setText(0, reference);
		item->setText(1, QString::number(index));
		item->setText(2, QLatin1String("top"));
		QString toolTip;
		if (hint) {
			toolTip += QLatin1String("<<< HINT >>>\n");
		}
		toolTip += QLatin1String("index: ");
		toolTip += QString::number (index);
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("types: ");
		toolTip += types;
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("reference: ");
		toolTip += reference;
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("expression: ");
		toolTip += expression;
		item->setToolTip(0, toolTip);
	}
	void Proof::buildRoot(QDomNode& outlineNode) {
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("proof" )) {
					buildRootProof (outlineNode);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}
	void Proof::buildRootProof(QDomNode& outlineNode) {
		//root->setIcon (1, Icon :: proved());
		QDomElement outlineElement = outlineNode.toElement();
		QString proof = outlineElement.text();
		proofs_ << proof;
		root_->setToolTip(0, proofs_.join(QLatin1String("\n")));
	}

	void Proof::buildNode(QDomNode& outlineNode) {
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("prop")) {
					buildNodeProp (outlineNode);
				} else if (outlineElement.tagName() == QLatin1String("hyp")) {
					buildNodeHyp (outlineNode);
				} else if (outlineElement.tagName() == QLatin1String("ref")) {
					buildNodeRef (outlineNode);
				} else if (outlineElement.tagName() == QLatin1String("root")) {
					buildNodeRoot (outlineNode);
				} else if (outlineElement.tagName() == QLatin1String("top")) {
					buildNodeTop (outlineNode);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}
	void Proof::buildNodeRoot(QDomNode& outlineNode) {
		buildNodeHyp (outlineNode);
	}
	void Proof::buildNodeHyp(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		const QString types = outlineElement.attribute(QLatin1String("types"));
		const int index = outlineElement.attribute(QLatin1String("index")).toInt();
		const bool hint = (outlineElement.attribute(QLatin1String("hint")) == QLatin1String("+"));

		QString expression;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}

		QDialog* infoBox = new QDialog(this);
		QGridLayout* layout = new QGridLayout(infoBox);

		QLabel*    indexLabel = new QLabel(QString(QLatin1String("index:")));
		QLineEdit* indexText  = new QLineEdit(QString::number(index));
		indexText->setReadOnly (true);
		layout->addWidget(indexLabel, 0, 0, 1, 1);
		layout->addWidget(indexText,  0, 1, 1, 1);

		QLabel*    infoLabel = new QLabel(QString(QLatin1String("hypothesis:")));
		QLineEdit* infoText  = new QLineEdit(expression);
		infoText->setReadOnly (true);
		layout->addWidget(infoLabel, 1, 0, 1, 1);
		layout->addWidget(infoText,  1, 1, 1, 1);

		QLabel*    typesLabel = new QLabel(QString(QLatin1String("types:")));
		QLineEdit* typesText  = new QLineEdit(types);
		typesText->setReadOnly(true);
		layout->addWidget(typesLabel, 2, 0, 1, 1);
		layout->addWidget(typesText,  2, 1, 1, 1);

		QTableWidget* evidenceList = buildNodeEvidences(outlineNode);
		layout->addWidget(evidenceList, 3, 0, 2, 5);
		infoBox->setVisible(true);
	}
	void Proof::buildNodeProp(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		const QString name = outlineElement.attribute(QLatin1String("name"));
		const int index = outlineElement.attribute(QLatin1String("index")).toInt();
		const bool hint = (outlineElement.attribute(QLatin1String("hint")) == QLatin1String("+"));

		QString assertion;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("assertion")) {
				assertion = childElement.text();
			}
			childNode = childNode.nextSibling();
		}

		QDialog* infoBox = new QDialog (this);
		QGridLayout* layout = new QGridLayout (infoBox);

		QLabel*    indexLabel = new QLabel (QString(QLatin1String("index:")));
		QLineEdit* indexText  = new QLineEdit (QString :: number (index));
		indexText->setReadOnly (true);
		layout->addWidget (indexLabel, 0, 0, 1, 1);
		layout->addWidget (indexText,  0, 1, 1, 1);

		QLabel*    infoLabel = new QLabel (QString(QLatin1String("proposition:")));
		QLineEdit* infoText  = new QLineEdit (name);
		infoText->setReadOnly (true);
		layout->addWidget (infoLabel, 1, 0, 1, 1);
		layout->addWidget (infoText,  1, 1, 1, 1);

		QTableWidget* evidenceList = buildNodeEvidences (outlineNode);
		layout->addWidget (evidenceList, 2, 0, 2, 5);
		layout->setEnabled (true);
		infoBox->setVisible (true);
	}
	void Proof::buildNodeRef(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		const int index = outlineElement.attribute (QLatin1String("index")).toInt();
		const bool hint = (outlineElement.attribute (QLatin1String("hint")) == QLatin1String("+"));

		QString expression;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}

		QDialog* infoBox = new QDialog (this);
		QGridLayout* layout = new QGridLayout (infoBox);

		QLabel*    indexLabel = new QLabel (QString(QLatin1String("index:")));
		QLineEdit* indexText  = new QLineEdit (QString :: number (index));
		indexText->setReadOnly (true);
		layout->addWidget (indexLabel, 0, 0, 1, 1);
		layout->addWidget (indexText,  0, 1, 1, 1);

		QLabel*    infoLabel = new QLabel (QString(QLatin1String("reference to:")));
		QLineEdit* infoText  = new QLineEdit (expression);
		infoText->setReadOnly (true);
		layout->addWidget (infoLabel, 1, 0, 1, 1);
		layout->addWidget (infoText,  1, 1, 1, 1);

		QTableWidget* evidenceList = buildNodeEvidences (outlineNode);
		layout->addWidget (evidenceList, 2, 0, 2, 5);
		infoBox->setVisible (true);
	}
	void Proof::buildNodeTop(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		const QString types = outlineElement.attribute (QLatin1String("types"));
		const int index = outlineElement.attribute (QLatin1String("index")).toInt();
		const bool hint = (outlineElement.attribute (QLatin1String("hint")) == QLatin1String("+"));
		const QString reference = outlineElement.attribute (QLatin1String("reference"));

		QString expression;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}

		QDialog* infoBox = new QDialog (this);
		QGridLayout* layout = new QGridLayout (infoBox);

		QLabel*    indexLabel = new QLabel (QString(QLatin1String("index:")));
		QLineEdit* indexText  = new QLineEdit (QString :: number (index));
		indexText->setReadOnly (true);
		layout->addWidget (indexLabel, 0, 0, 1, 1);
		layout->addWidget (indexText,  0, 1, 1, 1);

		QLabel*    typesLabel = new QLabel (QString(QLatin1String("types:")));
		QLineEdit* typesText  = new QLineEdit (types);
		typesText->setReadOnly (true);
		layout->addWidget (typesLabel, 1, 0, 1, 1);
		layout->addWidget (typesText,  1, 1, 1, 1);

		QLabel*    refInfoLabel = new QLabel (QString(QLatin1String("reference to:")));
		QLineEdit* refInfoText  = new QLineEdit (reference);
		refInfoText->setReadOnly (true);
		layout->addWidget (refInfoLabel, 2, 0, 1, 1);
		layout->addWidget (refInfoText,  2, 1, 1, 1);

		QLabel*    expInfoLabel = new QLabel (QString(QLatin1String("expression:")));
		QLineEdit* expInfoText  = new QLineEdit (expression);
		expInfoText->setReadOnly (true);
		layout->addWidget (expInfoLabel, 3, 0, 1, 1);
		layout->addWidget (expInfoText,  3, 1, 1, 1);

		QTableWidget* evidenceList = buildNodeEvidences (outlineNode);
		layout->addWidget (evidenceList, 4, 0, 2, 5);
		infoBox->setVisible (true);
	}
	QTableWidget* Proof::buildNodeEvidences(QDomNode& outlineNode) {
		int rowCount = 0;
		QDomNode countingNode = outlineNode.firstChild();
		while (!countingNode.isNull()) {
			QDomElement outlineElement = countingNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("evidence")) {
					++ rowCount;
				}
			}
			countingNode = countingNode.nextSibling();
		}
		QTableWidget* evidenceList = new QTableWidget (rowCount, 4);
		evidenceList->setHorizontalHeaderItem (0, new QTableWidgetItem (QString(QLatin1String("hint"))));
		evidenceList->setHorizontalHeaderItem (1, new QTableWidgetItem (QString(QLatin1String("proof"))));
		evidenceList->setHorizontalHeaderItem (2, new QTableWidgetItem (QString(QLatin1String("expression"))));
		evidenceList->setHorizontalHeaderItem (3, new QTableWidgetItem (QString(QLatin1String("types"))));
		evidenceList->setHorizontalHeaderItem (4, new QTableWidgetItem (QString(QLatin1String("substitution"))));
		//evidenceList->setSizePolicy(QSizePolicy (QSizePolicy :: Maximum, QSizePolicy :: Maximum));
		QDomNode evidenceNode = outlineNode.firstChild();
		int row = 0;
		int maxProofSize = 0;
		int maxExprSize = 0;
		Qt::ItemFlags flags;
		flags |= Qt::ItemIsEnabled;
		while (!evidenceNode.isNull()) {
			QDomElement outlineElement = evidenceNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("evidence")) {
					const bool hint = (outlineElement.attribute (QLatin1String("hint")) == QLatin1String("+"));
					const QString proof = outlineElement.attribute (QLatin1String("proof"));
					const QString types = outlineElement.attribute (QLatin1String("types"));

					QString expression;
					QString substitution;
					QDomNode childNode = outlineElement.firstChild();
					while (!childNode.isNull()) {
						QDomElement childElement = childNode.toElement();
						if (childElement.tagName() == QLatin1String("expression")) {
							expression = childElement.text();
						}
						if (childElement.tagName() == QLatin1String("substitution")) {
							substitution = childElement.text();
						}
						childNode = childNode.nextSibling();
					}

					QTableWidgetItem* hintItem = new QTableWidgetItem (hint ? QString(QLatin1String("<<HINT>>")) : QLatin1String(""));
					QTableWidgetItem* proofItem = new QTableWidgetItem (proof);
					QTableWidgetItem* expressionItem = new QTableWidgetItem (expression);
					QTableWidgetItem* typesItem = new QTableWidgetItem (types);
					QTableWidgetItem* substitutionItem = new QTableWidgetItem (substitution);
					proofItem->setFlags (flags);
					expressionItem->setFlags (flags);
					typesItem->setFlags (flags);
					substitutionItem->setFlags (flags);
					maxProofSize = (maxProofSize < proof.size()) ? proof.size() : maxProofSize;
					maxExprSize  = (maxExprSize < expression.size()) ? expression.size() : maxExprSize;
					evidenceList->setItem (row, 0, hintItem);
					evidenceList->setItem (row, 1, proofItem);
					evidenceList->setItem (row, 2, expressionItem);
					evidenceList->setItem (row, 3, typesItem);
					evidenceList->setItem (row, 4, substitutionItem);
					++ row;
				}
			}
			evidenceNode = evidenceNode.nextSibling();
		}
		//const int stetchFactor = 10;
		//evidenceList->setColumnWidth (0, maxProofSize * stetchFactor);
		//evidenceList->setColumnWidth (1, maxExprSize * stetchFactor);
		evidenceList->setVisible (true);
		evidenceList->adjustSize();
		return evidenceList;
	}
}
