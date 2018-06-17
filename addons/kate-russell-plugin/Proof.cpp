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

#include <KMessageBox>

#include "Proof.hpp"
#include "View.hpp"
#include "Icon.hpp"

#include "Proof.moc"

#include "commands.hpp"
#include "Execute.hpp"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	Proof::Proof
	(
		KTextEditor::MainWindow* mainWindow,
		View* russellView
	) :
	//QWidget (mainWindow->centralWidget()),
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
	Proof::~ Proof() {
		/*delete popup_;
		delete tree_;
		delete layout_;
		delete frame_;
		delete stop_;
		delete admit_;*/
		//delete proofView_;
	}

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
	void Proof::expandTree() {
		for (int i = 0; i < treeItems_.size(); ++ i) {
			treeItems_[i]->setExpanded (true);
		}
	}
	void Proof::doNothing() {
	}

	void Proof::startProving(const QString& file, int line, int col) {
		tree_->clear();
		treeItems_.clear();
		root_ = nullptr;
		proofs_.clear();
		Execute::exec(command::prove(file, ProvingMode::INTERACTIVE, line, col));
/*
		// setup plant options
		QString options = QLatin1String("setup ");
		options += QLatin1String("--interactive ");
		options += QLatin1String("--info-tree-sprout ");
		options += QLatin1String("--info-xml-format ");
		//russellView_->client()->execute (options);

		// do plant job
		//russellView_->client()->execute (QString(QLatin1String("plant")));

		// setup info options
		options = QLatin1String("setup ");
		options += QLatin1String("--info-tree-sprout ");
		options += QLatin1String("--info-xml-format ");
		options += QLatin1String("--hide-comments yes");
		//russellView_->client()->execute (options);

		// request for the result of plant operation
		//russellView_->client()->execute (QString(QLatin1String("info")));
		root_ = nullptr;
		proofs_.clear();
		const QString output; // =  Connection::get().data(); //russellView_->client()->getData();
		updateXML (output);

		// now set showing comments back to default
		options.clear();
		options += QLatin1String("setup --hide-comments no");
		//russellView_->client()->execute (options);*/
	}
	void Proof::growTree (QTreeWidgetItem* item) {
		if (!item) {
			return;
		}
		const QString indexString = item->text (1);
		const int index = indexString.toInt();
		if (index == -1) {
			return;
		}
		Execute::exec(QStringList() << QLatin1String("rus prove_step what=") + QString::number(index));

		/*
		// setup the expansion index:
		QString command = QLatin1String("setup --index ");
		command += QString :: number (index);
		//russellView_->client()->execute (command);

		// do grow job
		//russellView_->client()->execute (QString(QLatin1String("grow")));

		// setup info options
		QString options = QLatin1String("setup ");
		options += QLatin1String("--info-tree-sprout ");
		options += QLatin1String("--info-xml-format ");
		options += QLatin1String("--hide-comments yes");
		//russellView_->client()->execute (options);

		// request for the result of grow operation
		//russellView_->client()->execute (QString(QLatin1String("info")));
		const QString output; // = Connection::get().data(); //russellView_->client()->getData();
		updateXML (output);

		// now set showing comments back to default
		options.clear();
		options += QLatin1String("setup --hide-comments no");
		//russellView_->client()->execute (options);

		if (isProved()) {
			QString message = QLatin1String("Proofs found:\n");
			message += proofs_.join(QLatin1String("\n"));
			message += QLatin1String("admit or continue?");
			const int decision = KMessageBox :: questionYesNo
			(
				tree_,
				message,
				QLatin1String("proofs"),
				KStandardGuiItem :: yes(),
				KStandardGuiItem :: cont()
			);

			proofFound(0);

			/*if (decision == KMessageBox :: Yes) {

			} else {

			}* /
		}*/
	}
	void Proof::stopProving() {
		Execute::exec(QStringList() << QLatin1String("rus prove_stop"));
		tree_->clear();
		treeItems_.clear();
		root_ = nullptr;
		//hide();
	}
	void Proof::info() {
		if (QTreeWidgetItem* item = tree_->currentItem()) {
			const QString indexString = item->text(1);
			const int index = indexString.toInt();
			if (index == -1) {
				return;
			}
			Execute::exec(QStringList() << QLatin1String("rus prove_info what=") + QString::number(index));

			/*
			//russellView_->client()->execute (options);
			//russellView_->client()->execute (QString(QLatin1String("info")));
			QString nodeXML;
			//nodeXML += Connection::get().data(); //russellView_->client()->getData();
			updateXML (nodeXML);
			*/
		}
	}
	void Proof::show() {
		mainWindow_->showToolView(proofView_);
	}
	void Proof::hide() {
		mainWindow_->hideToolView(proofView_);
	}
	void Proof::visibilityChanged(bool visible) {
	}
	void Proof::updateXML(const QString& XMLSource) {
		QDomDocument document(QLatin1String("tree"));
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
		if (root.tagName() == QLatin1String("sprout")) {
			buildTree (node);
		} else if (root.tagName() == QLatin1String("node")) {
			buildNode (node);
		}
	}

	/****************************
	 *	Private members
	 ****************************/

	void Proof::setupSlotsAndSignals() {
		QAction* showAct = new QAction(QIcon(QLatin1String("open.png")), tr("&show..."), popup_);
		//showAct->setShortcuts(QKeySequence::Open);
		showAct->setStatusTip(tr("Show complete info"));
		connect(showAct, SIGNAL(triggered()), this, SLOT(info()));
		popup_->insertAction(nullptr, showAct);
/*
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("expand"), this, SLOT (expandTree()));
*/
		QAction* expand = popup_->addAction(QLatin1String("expand"));

		connect(expand, SIGNAL(changed()), this, SLOT(expandTree()));

		connect(tree_, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slotShowContextMenu(const QPoint&)));
		connect(tree_, SIGNAL(itemActivated(QTreeWidgetItem*, int)), this, SLOT(growTree(QTreeWidgetItem*)));
		connect(proofView_, SIGNAL(toolVisibleChanged(bool)), this, SLOT(visibilityChanged(bool)));
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

	void Proof::buildTree(QDomNode& outlineNode) {
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QLatin1String("up")) {
					QDomNode upNode = outlineNode.firstChild();
					buildTreeUp (upNode);
				} else if (outlineElement.tagName() == QLatin1String("root")) {
					QDomNode downNode = outlineNode.firstChild();
					buildRoot (downNode);
				}
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
		root_ = new QTreeWidgetItem(tree_);
		QDomElement outlineElement = outlineNode.toElement();
		const int index = outlineElement.attribute(QLatin1String("index")).toInt();
		const QString types = outlineElement.attribute(QLatin1String("types"));
		treeItems_[index] = root_;

		QDomNode childNode = outlineNode.firstChild();
		QString expression;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}
		root_->setIcon(0, Icon::root());
		root_->setText(0, expression);
		root_->setText(1, QString::number(index));
		root_->setText(2, QLatin1String("root"));
		QString toolTip = QLatin1String("index: ");
		toolTip += QString::number(index);
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("types: ");
		toolTip += types;
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("expression: ");
		toolTip += expression;
		toolTip += QLatin1String("\n");
		root_->setToolTip (0, toolTip);
		//item->setText (2, outlineElement.attribute ("line", ""));
		//item->setText (3, outlineElement.attribute ("column", ""));
		//item->setText (4, "yes");
	}
	void Proof::buildTreeHyp(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		const QString types = outlineElement.attribute(QLatin1String("types"));
		const bool hint = (outlineElement.attribute(QLatin1String("hint")) == QLatin1String("+"));
		const int index = outlineElement.attribute(QLatin1String("index")).toInt();
		std :: vector<int> downIndexes;

		QDomNode childNode = outlineNode.firstChild();
		QString expression;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("down")) {
				const int downIndex = childElement.attribute(QLatin1String("index")).toInt();
				downIndexes.push_back(downIndex);
			} else if (childElement.tagName() == QLatin1String("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}
		for (unsigned i = 0; i < downIndexes.size(); ++ i) {
			int downIndex = downIndexes[i];
			QTreeWidgetItem* parent = treeItems_ [downIndex];
			QTreeWidgetItem* item =	new QTreeWidgetItem(parent);
			treeItems_[index] = item;

			item->setIcon(0, Icon::hyp());
			item->setText(0, expression);
			item->setText(1, QString::number (index));
			item->setText(2, QLatin1String("hyp"));
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
			toolTip += QLatin1String("expression: ");
			toolTip += expression;
			toolTip += QLatin1String("\n");
			item->setToolTip (0, toolTip);
			break;
		}
	}
	void Proof::buildTreeProp(QDomNode& outlineNode) {
		QDomElement outlineElement = outlineNode.toElement();
		const QString name = outlineElement.attribute(QLatin1String("name"));
		const bool hint = (outlineElement.attribute(QLatin1String("hint")) == QLatin1String("+"));
		const int index = outlineElement.attribute(QLatin1String("index")).toInt();

		QDomNode childNode = outlineNode.firstChild();
		QDomElement childElement = childNode.toElement();
		int downIndex = 0; // = childElement.attribute("index", "").toInt();
		QString assertion;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QLatin1String("assertion")) {
				assertion = childElement.text();
			} else if (childElement.tagName() == QLatin1String("down")) {
				downIndex = childElement.attribute(QLatin1String("index")).toInt();
			}
			childNode = childNode.nextSibling();
		}
		QTreeWidgetItem* parent = treeItems_ [downIndex];
		QTreeWidgetItem* item =	new QTreeWidgetItem(parent);
		treeItems_ [index] = item;

		item->setIcon(0, Icon::prop());
		item->setText(0, name);
		item->setText(1, QString::number (index));
		item->setText(2, QLatin1String("prop"));
		item->setText(3, assertion);
		QString toolTip;
		if (hint) {
			toolTip += QLatin1String("<<< HINT >>>\n");
		}
		toolTip += QLatin1String("index: ");
		toolTip += QString::number(index);
		toolTip += QLatin1String("\n");
		toolTip += QLatin1String("assertion: \n");
		toolTip += assertion;
		item->setToolTip(0, toolTip);
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
