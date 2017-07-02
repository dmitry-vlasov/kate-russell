/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Config.cpp                    */
/* Description:     a config class for a Russell support plugin for Kate     */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#include <KConfigGroup>
#include <KSharedConfig>

#include "ProjectConfigTab.hpp"
#include "russell.hpp"
#include <QDebug>
#include <QInputDialog>

namespace plugin {
namespace kate {
namespace russell {

ProjectConfigTab::ProjectConfigTab(QWidget* parent) : QWidget(parent), configGroup_(KSharedConfig::openConfig(), QStringLiteral("Russell")) {
	configUi_.setupUi(this);
	connect(configUi_.addProjectButton, SIGNAL(clicked()), this, SLOT(addProjectSlot()));
	connect(configUi_.delProjectButton, SIGNAL(clicked()), this, SLOT(delProjectSlot()));
	connect(configUi_.chooseRusRootButton, SIGNAL(clicked()), this, SLOT(chooseRussellRootSlot()));
	connect(configUi_.chooseSmmRootButton, SIGNAL(clicked()), this, SLOT(chooseSmmRootSlot()));
	connect(configUi_.projectsComboBox, SIGNAL(activated(int)), this, SLOT(switchProjectSlot(int)));
	loadConfig();
}

ProjectConfigTab::~ProjectConfigTab() {
	saveConfig();
}

void ProjectConfigTab::addProject(const QString& name) {
	if (!name.size()) {
		KMessageBox :: sorry(0, i18n ("Empty project name is now allowed."));
		return;
	}
	if (configUi_.projectsComboBox->findText(name) != -1) {
		KMessageBox :: sorry(0, i18n ("The project with this name is already defined."));
		return;
	}
	configUi_.projectsComboBox->addItem(name);
	int index = configUi_.projectsComboBox->count() - 1;
	configUi_.projectsComboBox->setCurrentIndex(index);
	configUi_.delProjectButton->setEnabled(true);
	configUi_.chooseRusRootButton->setEnabled(true);
	configUi_.chooseSmmRootButton->setEnabled(true);
	configUi_.smmRootLineEdit->setEnabled(true);
	configUi_.russellRootLineEdit->setEnabled(true);

	configGroup_.writeEntry(QStringLiteral("ProjectsCount"), configUi_.projectsComboBox->count());
	switchProjectSlot(index);
}

	/*******************************
	 *	Private slot members
	 *******************************/

void ProjectConfigTab::addProjectSlot() {
	int project_count = configUi_.projectsComboBox->count();
	bool ok_pressed;
	QString name = QInputDialog::getText(
		this,
		i18n("Add project"),
		i18n("Name:"),
		QLineEdit::Normal,
		QStringLiteral("project %1").arg(project_count + 1),
		&ok_pressed
	);
	if (!ok_pressed) return;
	addProject(name);
}

void ProjectConfigTab::delProjectSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	if (!name.size()) {
		KMessageBox :: sorry(0, i18n ("Empty project name is now allowed."));
		return;
	}
	int index = configUi_.projectsComboBox->findText(name);
	if (index == -1) {
		KMessageBox :: sorry(0, i18n ("The project with this name is not defined."));
		return;
	}
	ProjectConfig::projects().remove(name);
	configUi_.projectsComboBox->removeItem(index);
	removeConfigForProject(index);
	if (!configUi_.projectsComboBox->count()) {
		configUi_.delProjectButton->setEnabled(false);
		configUi_.chooseRusRootButton->setEnabled(false);
		configUi_.chooseSmmRootButton->setEnabled(false);
		configUi_.smmRootLineEdit->setEnabled(false);
		configUi_.russellRootLineEdit->setEnabled(false);
	} else {
		QString name = configUi_.projectsComboBox->currentText();
		configUi_.smmRootLineEdit->setText(ProjectConfig::projects()[name].smmRoot);
		configUi_.russellRootLineEdit->setText(ProjectConfig::projects()[name].rusRoot);
	}
}

void ProjectConfigTab::chooseRussellRootSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	QString current_root = configUi_.russellRootLineEdit->text();
	QString new_root = QFileDialog :: getExistingDirectory (this, i18n("Russell root directory"), current_root);
	ProjectConfig::projects()[name].rusRoot = new_root;
	configUi_.russellRootLineEdit->setText(new_root);
}

void ProjectConfigTab::chooseSmmRootSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	QString current_root = configUi_.smmRootLineEdit->text();
	QString new_root = QFileDialog :: getExistingDirectory (this, i18n("Smm root directory"), current_root);
	ProjectConfig::projects()[name].smmRoot = new_root;
	configUi_.smmRootLineEdit->setText(new_root);
}

void ProjectConfigTab::switchProjectSlot(int index) {
	QString name = configUi_.projectsComboBox->itemText(index);
	configUi_.projectGroupBox->setTitle(QStringLiteral("Project: ") + name);
	configUi_.smmRootLineEdit->setText(ProjectConfig::projects()[name].smmRoot);
	configUi_.russellRootLineEdit->setText(ProjectConfig::projects()[name].rusRoot);
}

	/****************************
	 *	Private members
	 ****************************/

void ProjectConfigTab::loadConfig() {
    int count = configGroup_.readEntry(QStringLiteral("ProjectsCount"), 0);
    for (int i = 0; i < count; ++i) {
    	loadConfigForProject(i);
    }
    int index = configGroup_.readEntry(QStringLiteral("ProjectIndex"), -1);
    configUi_.projectsComboBox->setCurrentIndex(index);
    switchProjectSlot(index);
}

void ProjectConfigTab::loadConfigForProject(int i) {
	QString name = configGroup_.readEntry(QStringLiteral("Projects %1 name").arg(i), QStringLiteral("project %1").arg(i));
	ProjectConfig::projects()[name].rusRoot = configGroup_.readEntry(QStringLiteral("Projects %1 Rus root").arg(i), "");
	ProjectConfig::projects()[name].smmRoot = configGroup_.readEntry(QStringLiteral("Projects %1 Smm root").arg(i), "");
	addProject(name);
}


void ProjectConfigTab::saveConfig() {
	int count = configUi_.projectsComboBox->count();
	configGroup_.writeEntry(QStringLiteral("ProjectsCount"), count);
	int index = configUi_.projectsComboBox->currentIndex();
	configGroup_.writeEntry(QStringLiteral("ProjectIndex"), index);
	for (int i = 0; i < count; ++i) {
		saveConfigForProject(i);
    }
}

void ProjectConfigTab::saveConfigForProject(int i) {
	QString name = configUi_.projectsComboBox->itemText(i);
	configGroup_.writeEntry(QStringLiteral("Projects %1 name").arg(i), name);
	configGroup_.writeEntry(QStringLiteral("Projects %1 Rus root").arg(i), ProjectConfig::projects()[name].rusRoot);
	configGroup_.writeEntry(QStringLiteral("Projects %1 Smm root").arg(i), ProjectConfig::projects()[name].smmRoot);
}

void ProjectConfigTab::removeConfigForProject(int i) {
	configGroup_.writeEntry(QStringLiteral("ProjectsCount"), configUi_.projectsComboBox->count());
	QString name = configUi_.projectsComboBox->itemText(i);
	configGroup_.deleteEntry(QStringLiteral("Projects %1 name").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 Rus root").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 Smm root").arg(i));
}

}}}

