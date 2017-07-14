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
#include "Connection.hpp"
#include <QDebug>
#include <QInputDialog>

namespace plugin {
namespace kate {
namespace russell {

SourceType file_type(const QString& file) {
	if (file.endsWith(QStringLiteral(".rus"))) return SourceType::RUS;
	if (file.endsWith(QStringLiteral(".smm"))) return SourceType::SMM;
	if (file.endsWith(QStringLiteral(".mm")))  return SourceType::MM;
	return SourceType::OTHER;
}

void ProjectConfig::initProject() {
	mdl::Connection::mod().execute(QStringLiteral("rus curr proj=") + name_);
	mdl::Connection::mod().execute(QStringLiteral("rus opts verbose root=") + rusRoot_);
	mdl::Connection::mod().execute(QStringLiteral("smm curr proj=") + name_);
	mdl::Connection::mod().execute(QStringLiteral("smm opts verbose root=") + smmRoot_);
}

void ProjectConfig::loadMain() {
	mdl::Connection::mod().execute(QStringLiteral("rus read in=") + rusMain_);
	mdl::Connection::mod().execute(QStringLiteral("rus parse"));
	//mdl::Connection::mod().execute(QStringLiteral("smm read in=") + smmMain);
}

const ProjectConfig* ProjectConfig::find(const QString& file) {
	switch (file_type(file)) {
	case SourceType::RUS: for (auto& p : projects()) if (file.startsWith(p.rusRoot())) return &p; break;
	case SourceType::SMM: for (auto& p : projects()) if (file.startsWith(p.smmRoot())) return &p; break;
	default: break;
	}
	return nullptr;
}

static QString trimFileName(const QString& file, const QString& prefix) {
	QString trimmed = file;
	trimmed =trimmed.mid(prefix.size());
	if (trimmed.startsWith(QLatin1Char('/'))) trimmed = trimmed.mid(1);
	return trimmed;
}

QString ProjectConfig::trimFile(const QString& file) const {
	if (file.startsWith(rusRoot_)) {
		return trimFileName(file, rusRoot_);
	} else if (file.startsWith(smmRoot_)) {
		return trimFileName(file, smmRoot_);
	} else {
		KMessageBox :: sorry(0, i18n ("The main file must be situated in the root directory."));
		return QStringLiteral("");
	}
}

ProjectConfigTab::ProjectConfigTab(QWidget* parent) : QWidget(parent), configGroup_(KSharedConfig::openConfig(), QStringLiteral("Russell")) {
	configUi_.setupUi(this);
	connect(configUi_.addProjectButton, SIGNAL(clicked()), this, SLOT(addProjectSlot()));
	connect(configUi_.delProjectButton, SIGNAL(clicked()), this, SLOT(delProjectSlot()));
	connect(configUi_.chooseRusRootButton, SIGNAL(clicked()), this, SLOT(chooseRussellRootSlot()));
	connect(configUi_.chooseRusMainButton, SIGNAL(clicked()), this, SLOT(chooseRussellMainSlot()));
	connect(configUi_.chooseSmmRootButton, SIGNAL(clicked()), this, SLOT(chooseSmmRootSlot()));
	connect(configUi_.projectsComboBox, SIGNAL(activated(int)), this, SLOT(switchProjectSlot(int)));
	connect(configUi_.initProjectButton, SIGNAL(clicked()), this, SLOT(initProjectSlot()));
	connect(configUi_.loadMainButton, SIGNAL(clicked()), this, SLOT(loadMainSlot()));
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
	ProjectConfig::projects()[name].setName(name);
	configUi_.projectsComboBox->addItem(name);
	int index = configUi_.projectsComboBox->count() - 1;
	configUi_.projectsComboBox->setCurrentIndex(index);
	configUi_.delProjectButton->setEnabled(true);
	configUi_.chooseRusRootButton->setEnabled(true);
	configUi_.chooseSmmRootButton->setEnabled(true);
	configUi_.chooseRusMainButton->setEnabled(true);
	configUi_.smmRootLineEdit->setEnabled(true);
	configUi_.rusRootLineEdit->setEnabled(true);
	configUi_.rusMainLineEdit->setEnabled(true);
	configUi_.loadMainButton->setEnabled(true);
	configUi_.initProjectButton->setEnabled(true);

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
		QStringLiteral("project_%1").arg(project_count + 1),
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
		configUi_.chooseRusMainButton->setEnabled(false);
		configUi_.smmRootLineEdit->setEnabled(false);
		configUi_.rusRootLineEdit->setEnabled(false);
		configUi_.rusMainLineEdit->setEnabled(false);
		configUi_.loadMainButton->setEnabled(false);
		configUi_.initProjectButton->setEnabled(false);
	} else {
		QString name = configUi_.projectsComboBox->currentText();
		configUi_.smmRootLineEdit->setText(ProjectConfig::projects()[name].smmRoot());
		configUi_.rusRootLineEdit->setText(ProjectConfig::projects()[name].rusRoot());
		configUi_.rusMainLineEdit->setText(ProjectConfig::projects()[name].rusMain());
	}
}

void ProjectConfigTab::chooseRussellMainSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	QString current_main = configUi_.rusMainLineEdit->text();
	QString current_root = configUi_.rusRootLineEdit->text();
	QString new_main = QFileDialog :: getOpenFileName (this, i18n("Russell main file"), current_main);
	if (!new_main.startsWith(current_root)) {
		KMessageBox :: sorry(0, i18n ("The main file must be situated in the root directory."));
		return;
	}
	new_main = new_main.mid(current_root.size());
	if (new_main.startsWith(QLatin1Char('/'))) new_main = new_main.mid(1);

	ProjectConfig::projects()[name].setRusMain(new_main);
	configUi_.rusMainLineEdit->setText(new_main);
}

void ProjectConfigTab::chooseRussellRootSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	QString current_root = configUi_.rusRootLineEdit->text();
	QString new_root = QFileDialog :: getExistingDirectory (this, i18n("Russell root directory"), current_root);
	ProjectConfig::projects()[name].setRusRoot(new_root);
	configUi_.rusRootLineEdit->setText(new_root);
}

void ProjectConfigTab::chooseSmmRootSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	QString current_root = configUi_.smmRootLineEdit->text();
	QString new_root = QFileDialog :: getExistingDirectory (this, i18n("Smm root directory"), current_root);
	ProjectConfig::projects()[name].setSmmRoot(new_root);
	configUi_.smmRootLineEdit->setText(new_root);
}

void ProjectConfigTab::switchProjectSlot(int index) {
	QString name = configUi_.projectsComboBox->itemText(index);
	configUi_.projectGroupBox->setTitle(QStringLiteral("Project: ") + name);
	configUi_.smmRootLineEdit->setText(ProjectConfig::projects()[name].smmRoot());
	configUi_.rusRootLineEdit->setText(ProjectConfig::projects()[name].rusRoot());
	configUi_.rusMainLineEdit->setText(ProjectConfig::projects()[name].rusMain());
}

void ProjectConfigTab::initProjectSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	ProjectConfig::projects()[name].initProject();
}

void ProjectConfigTab::loadMainSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	ProjectConfig::projects()[name].loadMain();
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
	QString name = configGroup_.readEntry(QStringLiteral("Projects %1 name").arg(i), "");
	ProjectConfig::projects()[name].setRusRoot(configGroup_.readEntry(QStringLiteral("Projects %1 Rus root").arg(i), ""));
	ProjectConfig::projects()[name].setRusMain(configGroup_.readEntry(QStringLiteral("Projects %1 Rus main").arg(i), ""));
	ProjectConfig::projects()[name].setSmmRoot(configGroup_.readEntry(QStringLiteral("Projects %1 Smm root").arg(i), ""));
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
	configGroup_.writeEntry(QStringLiteral("Projects %1 Rus root").arg(i), ProjectConfig::projects()[name].rusRoot());
	configGroup_.writeEntry(QStringLiteral("Projects %1 Rus main").arg(i), ProjectConfig::projects()[name].rusMain());
	configGroup_.writeEntry(QStringLiteral("Projects %1 Smm root").arg(i), ProjectConfig::projects()[name].smmRoot());
}

void ProjectConfigTab::removeConfigForProject(int i) {
	configGroup_.writeEntry(QStringLiteral("ProjectsCount"), configUi_.projectsComboBox->count());
	QString name = configUi_.projectsComboBox->itemText(i);
	configGroup_.deleteEntry(QStringLiteral("Projects %1 name").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 Rus root").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 Rus main").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 Smm root").arg(i));
}

}}}

