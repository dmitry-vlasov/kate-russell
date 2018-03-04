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
#include "Client.hpp"
#include "Icon.hpp"

#include "Proof.moc"
#include "Execute.hpp"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	Proof :: Proof
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
			QStringLiteral("kate_private_plugin_kateproveplugin_proof"),
			KTextEditor::MainWindow :: Left,
			QIcon (QStringLiteral("application-x-ms-dos-executable")),
			i18n ("Proof")
		)
	),
	tree_ (new QTreeWidget (proofView_)),
	popup_ (new QMenu (tree_)),

	treeItems_ (),
	root_ (NULL),
	proofs_ ()
	{
		setupSlotsAndSignals();
		setupLayout();
	}
	Proof :: ~ Proof()
	{
		/*delete popup_;
		delete tree_;
		delete layout_;
		delete frame_;
		delete stop_;
		delete admit_;*/
		//delete proofView_;
	}

	bool
	Proof :: isProved() const {
		return !proofs_.empty();
	}
	int
	Proof :: getProofIndex() const {
		return 0;
	}

	/****************************
	 *	Public slots
	 ****************************/

	void 
	Proof :: slotShowContextMenu (const QPoint& point)
	{
		if (popup_ != NULL) {
			popup_->popup (tree_->mapToGlobal (point));
		}
	}
	void 
	Proof :: expandTree()
	{
		for (int i = 0; i < treeItems_.size(); ++ i) {
			treeItems_[i]->setExpanded (true);
		}
	}
	void
	Proof :: doNothing() {
	}

	void
	Proof :: startProving()
	{
		tree_->clear();
		treeItems_.clear();

		// setup plant options
		QString options = QStringLiteral("setup ");
		options += QStringLiteral("--interactive ");
		options += QStringLiteral("--info-tree-sprout ");
		options += QStringLiteral("--info-xml-format ");
		russellView_->client()->execute (options);

		// do plant job
		russellView_->client()->execute (QString(QStringLiteral("plant")));

		// setup info options
		options = QStringLiteral("setup ");
		options += QStringLiteral("--info-tree-sprout ");
		options += QStringLiteral("--info-xml-format ");
		options += QStringLiteral("--hide-comments yes");
		russellView_->client()->execute (options);

		// request for the result of plant operation
		russellView_->client()->execute (QString(QStringLiteral("info")));
		root_ = NULL;
		proofs_.clear();
		const QString output; // =  Connection::get().data(); //russellView_->client()->getData();
		updateXML (output);

		// now set showing comments back to default
		options.clear();
		options += QStringLiteral("setup --hide-comments no");
		russellView_->client()->execute (options);
	}
	void
	Proof :: growTree (QTreeWidgetItem* item)
	{
		if (item == NULL) {
			return;
		}
		const QString indexString = item->text (1);
		const int index = indexString.toInt();
		if (index == -1) {
			return;
		}
		// setup the expansion index:
		QString command = QStringLiteral("setup --index ");
		command += QString :: number (index);
		russellView_->client()->execute (command);

		// do grow job
		russellView_->client()->execute (QString(QStringLiteral("grow")));

		// setup info options
		QString options = QStringLiteral("setup ");
		options += QStringLiteral("--info-tree-sprout ");
		options += QStringLiteral("--info-xml-format ");
		options += QStringLiteral("--hide-comments yes");
		russellView_->client()->execute (options);

		// request for the result of grow operation
		russellView_->client()->execute (QString(QStringLiteral("info")));
		const QString output; // = Connection::get().data(); //russellView_->client()->getData();
		updateXML (output);

		// now set showing comments back to default
		options.clear();
		options += QStringLiteral("setup --hide-comments no");
		russellView_->client()->execute (options);

		if (isProved()) {
			QString message = QStringLiteral("Proofs found:\n");
			message += proofs_.join(QStringLiteral("\n"));
			message += QStringLiteral("admit or continue?");
			const int decision = KMessageBox :: questionYesNo
			(
				tree_,
				message,
				QStringLiteral("proofs"),
				KStandardGuiItem :: yes(),
				KStandardGuiItem :: cont()
			);

			proofFound(0);

			/*if (decision == KMessageBox :: Yes) {

			} else {

			}*/
		}
	}
	void
	Proof :: stopProving()
	{
		russellView_->client()->execute (QString(QStringLiteral("fell")));
		tree_->clear();
		treeItems_.clear();
		root_ = NULL;
		//hide();
	}
	void
	Proof :: info()
	{
		QTreeWidgetItem* item = tree_->currentItem();
		if (item == NULL) {
			return;
		}
		const QString indexString = item->text (1);
		const int index = indexString.toInt();
		if (index == -1) {
			return;
		}
		QString options = QStringLiteral("setup ");
		options += QStringLiteral("--info-tree-node ");
		options += QStringLiteral("--info-xml-format ");
		options += QStringLiteral("--index ");
		options += QString :: number (index);
		russellView_->client()->execute (options);
		russellView_->client()->execute (QString(QStringLiteral("info")));
		QString nodeXML;
		//nodeXML += Connection::get().data(); //russellView_->client()->getData();
		updateXML (nodeXML);
	}
	void
	Proof :: show() {
		mainWindow_->showToolView (proofView_);
	}
	void
	Proof :: hide() {
		mainWindow_->hideToolView (proofView_);
	}
	void
	Proof :: visibilityChanged (bool visible) {
	}

	/****************************
	 *	Private members
	 ****************************/

	void
	Proof :: setupSlotsAndSignals()
	{
		QAction* showAct = new QAction (QIcon(QStringLiteral("open.png")), tr("&show..."), popup_);
		//showAct->setShortcuts(QKeySequence::Open);
		showAct->setStatusTip (tr("Show complete info"));
		connect(showAct, SIGNAL (triggered()), this, SLOT (info()));
		popup_->insertAction (NULL, showAct);
/*
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("FUCK"), this, SLOT (doNothing()));
		popup_->insertItem (i18n ("expand"), this, SLOT (expandTree()));
*/
		QAction* expand = popup_->addAction(QStringLiteral("expand"));

		connect
		(
			expand,
			SIGNAL (changed ()),
			this,
			SLOT (expandTree())
		);

		connect
		(
			tree_,
			SIGNAL (customContextMenuRequested (const QPoint&)),
			this,
			SLOT (slotShowContextMenu (const QPoint&))
		);
		connect
		(
			tree_,
			SIGNAL (itemActivated (QTreeWidgetItem*, int)),
			this,
			SLOT (growTree (QTreeWidgetItem*))
		);
		connect
		(
			proofView_,
			SIGNAL (toolVisibleChanged (bool)),
			this,
			SLOT (visibilityChanged (bool))
		);
	}
	void
	Proof :: setupLayout()
	{
		QStringList titles;
		titles << i18nc ("@title:column", "Proof"); // << i18nc ("@title:column", "Position");
		tree_->setColumnCount (1);
		tree_->setHeaderLabels (titles);
		tree_->setColumnHidden (1, true);
		tree_->setSortingEnabled (false);
		tree_->setRootIsDecorated (0);
		tree_->setContextMenuPolicy (Qt :: CustomContextMenu);
		tree_->setIndentation (10);
		tree_->setLayoutDirection (Qt :: LeftToRight);

		tree_->setSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
		tree_->adjustSize();
		tree_->updateGeometry();
		tree_->setContentsMargins(1, 1, 1, 1);
		tree_->setMaximumWidth(100000);

		//proofView_->setMaximumWidth(100000);
		//proofView_->etSizePolicy (QSizePolicy::Expanding, QSizePolicy::Expanding);
		//proofView_->setContentsMargins(1, 1, 1, 1);
	}

	void
	Proof :: updateXML (const QString& XMLSource)
	{
		QDomDocument document (QStringLiteral("tree"));
		QString errorMsg;
		int errorLine = 0;
		int errorColumn = 0;
		if (!document.setContent (XMLSource, &errorMsg, &errorLine, &errorColumn)) {
			errorMsg += QStringLiteral(" on line ");
			errorMsg += QString :: number (errorLine);
			errorMsg += QStringLiteral(" on column ");
			errorMsg += QString :: number (errorColumn);
			KMessageBox :: sorry (0, errorMsg);
			return;
		}
		QDomElement root = document.documentElement();
		QDomNode node = root.firstChild();
		if (root.tagName() == QStringLiteral("sprout")) {
			buildTree (node);
		} else if (root.tagName() == QStringLiteral("node")) {
			buildNode (node);
		}
	}

	void
	Proof :: buildTree (QDomNode& outlineNode)
	{
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QStringLiteral("up")) {
					QDomNode upNode = outlineNode.firstChild();
					buildTreeUp (upNode);
				} else if (outlineElement.tagName() == QStringLiteral("root")) {
					QDomNode downNode = outlineNode.firstChild();
					buildRoot (downNode);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}
	void
	Proof :: buildTreeUp (QDomNode& outlineNode)
	{
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QStringLiteral("root")) {
					buildTreeUpRoot (outlineNode);
				} else if (outlineElement.tagName() == QStringLiteral("hyp")) {
					buildTreeHyp (outlineNode);
				} else if (outlineElement.tagName() == QStringLiteral("prop")) {
					buildTreeProp (outlineNode);
				} else if (outlineElement.tagName() == QStringLiteral("ref")) {
					buildTreeRef (outlineNode);
				} else if (outlineElement.tagName() == QStringLiteral("top")) {
					buildTreeTop (outlineNode);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}
	void
	Proof :: buildTreeUpRoot (QDomNode& outlineNode)
	{
		root_ = new QTreeWidgetItem (tree_);
		QDomElement outlineElement = outlineNode.toElement();
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();
		const QString types = outlineElement.attribute (QStringLiteral("types"));
		treeItems_[index] = root_;

		QDomNode childNode = outlineNode.firstChild();
		QString expression;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}
		root_->setIcon (0, Icon :: root());
		root_->setText (0, expression);
		root_->setText (1, QString :: number (index));
		root_->setText (2, QStringLiteral("root"));
		QString toolTip = QStringLiteral("index: ");
		toolTip += QString :: number (index);
		toolTip += QStringLiteral("\n");
		toolTip += QStringLiteral("types: ");
		toolTip += types;
		toolTip += QStringLiteral("\n");
		toolTip += QStringLiteral("expression: ");
		toolTip += expression;
		toolTip += QStringLiteral("\n");
		root_->setToolTip (0, toolTip);
		//item->setText (2, outlineElement.attribute ("line", ""));
		//item->setText (3, outlineElement.attribute ("column", ""));
		//item->setText (4, "yes");
	}
	void
	Proof :: buildTreeHyp (QDomNode& outlineNode)
	{
		QDomElement outlineElement = outlineNode.toElement();
		const QString types = outlineElement.attribute (QStringLiteral("types"));
		const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();
		std :: vector<int> downIndexes;

		QDomNode childNode = outlineNode.firstChild();
		QString expression;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("down")) {
				const int downIndex = childElement.attribute(QStringLiteral("index")).toInt();
				downIndexes.push_back(downIndex);
			} else if (childElement.tagName() == QStringLiteral("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}
		for (unsigned i = 0; i < downIndexes.size(); ++ i) {
			int downIndex = downIndexes[i];
			QTreeWidgetItem* parent = treeItems_ [downIndex];
			QTreeWidgetItem* item =	new QTreeWidgetItem (parent);
			treeItems_[index] = item;

			item->setIcon (0, Icon :: hyp());
			item->setText (0, expression);
			item->setText (1, QString :: number (index));
			item->setText (2, QStringLiteral("hyp"));
			QString toolTip;
			if (hint) {
				toolTip += QStringLiteral("<<< HINT >>>\n");
			}
			toolTip += QStringLiteral("index: ");
			toolTip += QString :: number (index);
			toolTip += QStringLiteral("\n");
			toolTip += QStringLiteral("types: ");
			toolTip += types;
			toolTip += QStringLiteral("\n");
			toolTip += QStringLiteral("expression: ");
			toolTip += expression;
			toolTip += QStringLiteral("\n");
			item->setToolTip (0, toolTip);
			break;
		}
	}
	void
	Proof :: buildTreeProp (QDomNode& outlineNode)
	{
		QDomElement outlineElement = outlineNode.toElement();
		const QString name = outlineElement.attribute (QStringLiteral("name"));
		const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();

		QDomNode childNode = outlineNode.firstChild();
		QDomElement childElement = childNode.toElement();
		int downIndex = 0; // = childElement.attribute("index", "").toInt();
		QString assertion;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("assertion")) {
				assertion = childElement.text();
			} else if (childElement.tagName() == QStringLiteral("down")) {
				downIndex = childElement.attribute(QStringLiteral("index")).toInt();
			}
			childNode = childNode.nextSibling();
		}
		QTreeWidgetItem* parent = treeItems_ [downIndex];
		QTreeWidgetItem* item =	new QTreeWidgetItem (parent);
		treeItems_ [index] = item;

		item->setIcon (0, Icon :: prop());
		item->setText (0, name);
		item->setText (1, QString :: number (index));
		item->setText (2, QStringLiteral("prop"));
		item->setText (3, assertion);
		QString toolTip;
		if (hint) {
			toolTip += QStringLiteral("<<< HINT >>>\n");
		}
		toolTip += QStringLiteral("index: ");
		toolTip += QString :: number (index);
		toolTip += QStringLiteral("\n");
		toolTip += QStringLiteral("assertion: \n");
		toolTip += assertion;
		item->setToolTip (0, toolTip);
	}
	void
	Proof :: buildTreeRef (QDomNode& outlineNode)
	{
		QDomElement outlineElement = outlineNode.toElement();
		const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();

		QDomNode childNode = outlineNode.firstChild();
		QDomElement childElement = childNode.toElement();
		int downIndex = 0;
		QString expression;
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("expression")) {
				expression = childElement.text();
			} else if (childElement.tagName() == QStringLiteral("down")) {
				downIndex  = childElement.attribute(QStringLiteral("index")).toInt();
			}
			childNode = childNode.nextSibling();
		}

		QTreeWidgetItem* parent = treeItems_ [downIndex];
		QTreeWidgetItem* item =	new QTreeWidgetItem (parent);
		treeItems_ [index] = item;

		item->setIcon (0, Icon :: ref());
		item->setText (0, expression);
		item->setText (1, QString :: number (index));
		item->setText (2, QStringLiteral("ref"));

		QString toolTip;
		if (hint) {
			toolTip += QStringLiteral("<<< HINT >>>\n");
		}
		toolTip += QStringLiteral("index: ");
		toolTip += QString :: number (index);
		toolTip += QStringLiteral("\n");
		toolTip += QStringLiteral("expression: ");
		toolTip += expression;
		item->setToolTip (0, toolTip);
	}
	void
	Proof :: buildTreeTop (QDomNode& outlineNode)
	{
		QDomElement outlineElement = outlineNode.toElement();
		const QString reference = outlineElement.attribute (QStringLiteral("reference"));
		const QString types = outlineElement.attribute (QStringLiteral("types"));
		const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();
		const int downIndex = outlineElement.attribute(QStringLiteral("down")).toInt();

		QString expression;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}
		QTreeWidgetItem* parent = treeItems_ [downIndex];
		QTreeWidgetItem* item =	new QTreeWidgetItem (parent);
		treeItems_ [index] = item;

		item->setIcon (0, Icon :: top());
		item->setText (0, reference);
		item->setText (1, QString :: number (index));
		item->setText (2, QStringLiteral("top"));
		QString toolTip;
		if (hint) {
			toolTip += QStringLiteral("<<< HINT >>>\n");
		}
		toolTip += QStringLiteral("index: ");
		toolTip += QString :: number (index);
		toolTip += QStringLiteral("\n");
		toolTip += QStringLiteral("types: ");
		toolTip += types;
		toolTip += QStringLiteral("\n");
		toolTip += QStringLiteral("reference: ");
		toolTip += reference;
		toolTip += QStringLiteral("\n");
		toolTip += QStringLiteral("expression: ");
		toolTip += expression;
		item->setToolTip (0, toolTip);
	}
	void
	Proof :: buildRoot (QDomNode& outlineNode)
	{
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QStringLiteral("proof" )) {
					buildRootProof (outlineNode);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}
	void
	Proof :: buildRootProof (QDomNode& outlineNode)
	{
		//root->setIcon (1, Icon :: proved());
		QDomElement outlineElement = outlineNode.toElement();
		QString proof = outlineElement.text();
		proofs_ << proof;
		root_->setToolTip (0, proofs_.join (QStringLiteral("\n")));
	}

	void
	Proof :: buildNode (QDomNode& outlineNode)
	{
		while (!outlineNode.isNull()) {
			QDomElement outlineElement = outlineNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QStringLiteral("prop")) {
					buildNodeProp (outlineNode);
				} else if (outlineElement.tagName() == QStringLiteral("hyp")) {
					buildNodeHyp (outlineNode);
				} else if (outlineElement.tagName() == QStringLiteral("ref")) {
					buildNodeRef (outlineNode);
				} else if (outlineElement.tagName() == QStringLiteral("root")) {
					buildNodeRoot (outlineNode);
				} else if (outlineElement.tagName() == QStringLiteral("top")) {
					buildNodeTop (outlineNode);
				}
			}
			outlineNode = outlineNode.nextSibling();
		}
	}
	void
	Proof :: buildNodeRoot (QDomNode& outlineNode) {
		buildNodeHyp (outlineNode);
	}
	void
	Proof :: buildNodeHyp (QDomNode& outlineNode)
	{
		QDomElement outlineElement = outlineNode.toElement();
		const QString types = outlineElement.attribute (QStringLiteral("types"));
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();
		const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));

		QString expression;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}

		QDialog* infoBox = new QDialog (this);
		QGridLayout* layout = new QGridLayout (infoBox);

		QLabel*    indexLabel = new QLabel (QString(QStringLiteral("index:")));
		QLineEdit* indexText  = new QLineEdit (QString :: number (index));
		indexText->setReadOnly (true);
		layout->addWidget (indexLabel, 0, 0, 1, 1);
		layout->addWidget (indexText,  0, 1, 1, 1);

		QLabel*    infoLabel = new QLabel (QString(QStringLiteral("hypothesis:")));
		QLineEdit* infoText  = new QLineEdit (expression);
		infoText->setReadOnly (true);
		layout->addWidget (infoLabel, 1, 0, 1, 1);
		layout->addWidget (infoText,  1, 1, 1, 1);

		QLabel*    typesLabel = new QLabel (QString(QStringLiteral("types:")));
		QLineEdit* typesText  = new QLineEdit (types);
		typesText->setReadOnly (true);
		layout->addWidget (typesLabel, 2, 0, 1, 1);
		layout->addWidget (typesText,  2, 1, 1, 1);

		QTableWidget* evidenceList = buildNodeEvidences (outlineNode);
		layout->addWidget (evidenceList, 3, 0, 2, 5);
		infoBox->setVisible (true);
	}
	void
	Proof :: buildNodeProp (QDomNode& outlineNode)
	{
		QDomElement outlineElement = outlineNode.toElement();
		const QString name = outlineElement.attribute (QStringLiteral("name"));
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();
		const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));

		QString assertion;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("assertion")) {
				assertion = childElement.text();
			}
			childNode = childNode.nextSibling();
		}

		QDialog* infoBox = new QDialog (this);
		QGridLayout* layout = new QGridLayout (infoBox);

		QLabel*    indexLabel = new QLabel (QString(QStringLiteral("index:")));
		QLineEdit* indexText  = new QLineEdit (QString :: number (index));
		indexText->setReadOnly (true);
		layout->addWidget (indexLabel, 0, 0, 1, 1);
		layout->addWidget (indexText,  0, 1, 1, 1);

		QLabel*    infoLabel = new QLabel (QString(QStringLiteral("proposition:")));
		QLineEdit* infoText  = new QLineEdit (name);
		infoText->setReadOnly (true);
		layout->addWidget (infoLabel, 1, 0, 1, 1);
		layout->addWidget (infoText,  1, 1, 1, 1);

		QTableWidget* evidenceList = buildNodeEvidences (outlineNode);
		layout->addWidget (evidenceList, 2, 0, 2, 5);
		layout->setEnabled (true);
		infoBox->setVisible (true);
	}
	void
	Proof :: buildNodeRef (QDomNode& outlineNode)
	{
		QDomElement outlineElement = outlineNode.toElement();
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();
		const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));

		QString expression;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}

		QDialog* infoBox = new QDialog (this);
		QGridLayout* layout = new QGridLayout (infoBox);

		QLabel*    indexLabel = new QLabel (QString(QStringLiteral("index:")));
		QLineEdit* indexText  = new QLineEdit (QString :: number (index));
		indexText->setReadOnly (true);
		layout->addWidget (indexLabel, 0, 0, 1, 1);
		layout->addWidget (indexText,  0, 1, 1, 1);

		QLabel*    infoLabel = new QLabel (QString(QStringLiteral("reference to:")));
		QLineEdit* infoText  = new QLineEdit (expression);
		infoText->setReadOnly (true);
		layout->addWidget (infoLabel, 1, 0, 1, 1);
		layout->addWidget (infoText,  1, 1, 1, 1);

		QTableWidget* evidenceList = buildNodeEvidences (outlineNode);
		layout->addWidget (evidenceList, 2, 0, 2, 5);
		infoBox->setVisible (true);
	}
	void
	Proof :: buildNodeTop (QDomNode& outlineNode)
	{
		QDomElement outlineElement = outlineNode.toElement();
		const QString types = outlineElement.attribute (QStringLiteral("types"));
		const int index = outlineElement.attribute (QStringLiteral("index")).toInt();
		const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));
		const QString reference = outlineElement.attribute (QStringLiteral("reference"));

		QString expression;
		QDomNode childNode = outlineNode.firstChild();
		while (!childNode.isNull()) {
			QDomElement childElement = childNode.toElement();
			if (childElement.tagName() == QStringLiteral("expression")) {
				expression = childElement.text();
			}
			childNode = childNode.nextSibling();
		}

		QDialog* infoBox = new QDialog (this);
		QGridLayout* layout = new QGridLayout (infoBox);

		QLabel*    indexLabel = new QLabel (QString(QStringLiteral("index:")));
		QLineEdit* indexText  = new QLineEdit (QString :: number (index));
		indexText->setReadOnly (true);
		layout->addWidget (indexLabel, 0, 0, 1, 1);
		layout->addWidget (indexText,  0, 1, 1, 1);

		QLabel*    typesLabel = new QLabel (QString(QStringLiteral("types:")));
		QLineEdit* typesText  = new QLineEdit (types);
		typesText->setReadOnly (true);
		layout->addWidget (typesLabel, 1, 0, 1, 1);
		layout->addWidget (typesText,  1, 1, 1, 1);

		QLabel*    refInfoLabel = new QLabel (QString(QStringLiteral("reference to:")));
		QLineEdit* refInfoText  = new QLineEdit (reference);
		refInfoText->setReadOnly (true);
		layout->addWidget (refInfoLabel, 2, 0, 1, 1);
		layout->addWidget (refInfoText,  2, 1, 1, 1);

		QLabel*    expInfoLabel = new QLabel (QString(QStringLiteral("expression:")));
		QLineEdit* expInfoText  = new QLineEdit (expression);
		expInfoText->setReadOnly (true);
		layout->addWidget (expInfoLabel, 3, 0, 1, 1);
		layout->addWidget (expInfoText,  3, 1, 1, 1);

		QTableWidget* evidenceList = buildNodeEvidences (outlineNode);
		layout->addWidget (evidenceList, 4, 0, 2, 5);
		infoBox->setVisible (true);
	}
	QTableWidget*
	Proof :: buildNodeEvidences (QDomNode& outlineNode)
	{
		int rowCount = 0;
		QDomNode countingNode = outlineNode.firstChild();
		while (!countingNode.isNull()) {
			QDomElement outlineElement = countingNode.toElement();
			if (!outlineElement.isNull()) {
				if (outlineElement.tagName() == QStringLiteral("evidence")) {
					++ rowCount;
				}
			}
			countingNode = countingNode.nextSibling();
		}
		QTableWidget* evidenceList = new QTableWidget (rowCount, 4);
		evidenceList->setHorizontalHeaderItem (0, new QTableWidgetItem (QString(QStringLiteral("hint"))));
		evidenceList->setHorizontalHeaderItem (1, new QTableWidgetItem (QString(QStringLiteral("proof"))));
		evidenceList->setHorizontalHeaderItem (2, new QTableWidgetItem (QString(QStringLiteral("expression"))));
		evidenceList->setHorizontalHeaderItem (3, new QTableWidgetItem (QString(QStringLiteral("types"))));
		evidenceList->setHorizontalHeaderItem (4, new QTableWidgetItem (QString(QStringLiteral("substitution"))));
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
				if (outlineElement.tagName() == QStringLiteral("evidence")) {
					const bool hint = (outlineElement.attribute (QStringLiteral("hint")) == QStringLiteral("+"));
					const QString proof = outlineElement.attribute (QStringLiteral("proof"));
					const QString types = outlineElement.attribute (QStringLiteral("types"));

					QString expression;
					QString substitution;
					QDomNode childNode = outlineElement.firstChild();
					while (!childNode.isNull()) {
						QDomElement childElement = childNode.toElement();
						if (childElement.tagName() == QStringLiteral("expression")) {
							expression = childElement.text();
						}
						if (childElement.tagName() == QStringLiteral("substitution")) {
							substitution = childElement.text();
						}
						childNode = childNode.nextSibling();
					}

					QTableWidgetItem* hintItem = new QTableWidgetItem (hint ? QString(QStringLiteral("<<HINT>>")) : QStringLiteral(""));
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
