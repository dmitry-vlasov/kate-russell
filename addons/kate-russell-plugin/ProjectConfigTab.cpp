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

#include <QDebug>
#include <QInputDialog>
#include <QFileDialog>

#include <KMessageBox>
#include <KConfigGroup>
#include <KSharedConfig>

#include "ProjectConfigTab.hpp"
#include "ProjectConfigTab.hpp"

#include "Enums.hpp"
#include "Execute.hpp"

namespace russell {

QString ProjectConfig::initProjectCommand() const {
	QString command;
	command += QLatin1String("rus curr proj=") + name_ + QLatin1String(";\n");
	command += QLatin1String("rus opts verbose root=") + rusRoot_ + QLatin1String(";\n");
	command += QLatin1String("mm  curr proj=") + name_ + QLatin1String(";\n");
	command += QLatin1String("mm  opts verbose root=") + mmRoot_ + QLatin1String(";\n");
	return command;
}

QString ProjectConfig::loadMainCommand() const {
	QString command;
	command += QLatin1String("rus read in=") + rusMain_ + QLatin1String(";\n");
	command += QLatin1String("rus parse;\n");
	command += QLatin1String("rus verify;\n");
	return command;
}

const ProjectConfig* ProjectConfig::find(const QString& file) {
	switch (file_type(file)) {
	case Lang::RUS: for (auto& p : projects()) if (file.startsWith(p.rusRoot())) return &p; break;
	case Lang::MM:  for (auto& p : projects()) if (file.startsWith(p.mmRoot()))  return &p; break;
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
	} else if (file.startsWith(mmRoot_)) {
		return trimFileName(file, mmRoot_);
	} else {
		KMessageBox :: sorry(0, i18n ("The main file %1 must be situated in the root directory.", file));
		return QStringLiteral("");
	}
}

QString ProjectConfig::rusTarget(const QString& file, bool full) const {
	QString target = trim_ext(trimFile(file)) + QLatin1String(".rus");
	return full ? rusRoot() + QLatin1String("/") + target : target;
}

QString ProjectConfig::mmTarget(const QString& file, bool full) const {
	QString target = trim_ext(trimFile(file)) + QLatin1String(".mm");
	return full ? mmRoot() + QLatin1String("/") + target : target;
}

QString ProjectConfig::mergedTarget(const QString& file, bool full) const {
	QString target = trim_ext(trimFile(file)) + QLatin1String("_merged.mm");
	return full ? mmRoot() + QLatin1String("/") + target : target;
}

QString ProjectConfig::target(const QString& file, Lang lang, bool full) const {
	switch (lang) {
	case Lang::RUS: return rusTarget(file, full);
	case Lang::MM:  return mmTarget(file, full);
	default: {
		KMessageBox :: sorry(0, i18n ("Target file %1 extension is not specified", trim_ext(file)));
		return QString();
	}
	}
}

ProjectConfigTab::ProjectConfigTab(QWidget* parent) : QWidget(parent), configGroup_(KSharedConfig::openConfig(), QLatin1String("Russell")) {
	configUi_.setupUi(this);
	connect(configUi_.addProjectButton, SIGNAL(clicked()), this, SLOT(addProjectSlot()));
	connect(configUi_.delProjectButton, SIGNAL(clicked()), this, SLOT(delProjectSlot()));
	connect(configUi_.chooseRusRootButton, SIGNAL(clicked()), this, SLOT(chooseRussellRootSlot()));
	connect(configUi_.chooseRusMainButton, SIGNAL(clicked()), this, SLOT(chooseRussellMainSlot()));
	connect(configUi_.chooseMmRootButton,  SIGNAL(clicked()), this, SLOT(chooseMmRootSlot()));
	connect(configUi_.autoinitCheckBox,  SIGNAL(stateChanged(int)), this, SLOT(checkAutoinitSlot(int)));
	connect(configUi_.autoloadCheckBox,  SIGNAL(stateChanged(int)), this, SLOT(checkAutoloadSlot(int)));
	connect(configUi_.projectsComboBox, SIGNAL(activated(int)), this, SLOT(switchProjectSlot(int)));
	connect(configUi_.initProjectButton, SIGNAL(clicked()), this, SLOT(initProjectSlot()));
	connect(configUi_.loadMainButton, SIGNAL(clicked()), this, SLOT(loadMainSlot()));
	loadConfig();
	QString initCommand;
	for (auto& conf : ProjectConfig::projects()) {
		if (conf.autoinit()) {
			initCommand += conf.initProjectCommand();
		}
	}
	for (auto& conf : ProjectConfig::projects()) {
		if (conf.autoload()) {
			initCommand += conf.loadMainCommand();
		}
	}
	if (initCommand.size()) Execute::russellClient().execute(initCommand);
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
	configUi_.delProjectButton->setEnabled   (true);
	configUi_.chooseMmRootButton->setEnabled (true);
	configUi_.chooseRusRootButton->setEnabled(true);
	configUi_.chooseRusMainButton->setEnabled(true);
	configUi_.mmRootLineEdit->setEnabled (true);
	configUi_.rusRootLineEdit->setEnabled(true);
	configUi_.rusMainLineEdit->setEnabled(true);
	configUi_.loadMainButton->setEnabled (true);
	configUi_.initProjectButton->setEnabled(true);

	configGroup_.writeEntry(QLatin1String("ProjectsCount"), configUi_.projectsComboBox->count());
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
	if (!name.size()) {
		KMessageBox :: sorry(0, i18n ("Empty project name is now allowed."));
		return;
	}
	if (name.contains(QLatin1String(" ")) || name.contains(QLatin1String("\t")) ||
		name.contains(QLatin1String("\n")) || name.contains(QLatin1String("\r"))) {
		KMessageBox :: sorry(0, i18n ("Project name shouldn't contain space symbols."));
		return;
	}
	if (!ok_pressed) return;
	addProject(name);
}

void ProjectConfigTab::delProjectSlot() {
	QString name = configUi_.projectsComboBox->currentText();
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
		configUi_.chooseRusMainButton->setEnabled(false);
		configUi_.mmRootLineEdit->setEnabled(false);
		configUi_.rusRootLineEdit->setEnabled(false);
		configUi_.rusMainLineEdit->setEnabled(false);
		configUi_.loadMainButton->setEnabled(false);
		configUi_.initProjectButton->setEnabled(false);
	} else {
		QString name = configUi_.projectsComboBox->currentText();
		configUi_.mmRootLineEdit->setText (ProjectConfig::projects()[name].mmRoot());
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

void ProjectConfigTab::chooseMmRootSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	QString current_root = configUi_.mmRootLineEdit->text();
	QString new_root = QFileDialog :: getExistingDirectory (this, i18n("Metamath root directory"), current_root);
	ProjectConfig::projects()[name].setMmRoot(new_root);
	configUi_.mmRootLineEdit->setText(new_root);
}

void ProjectConfigTab::checkAutoinitSlot(int state) {
	QString name = configUi_.projectsComboBox->currentText();
	ProjectConfig::projects()[name].setAutoinit(state == Qt::Checked);
}

void ProjectConfigTab::checkAutoloadSlot(int state) {
	QString name = configUi_.projectsComboBox->currentText();
	ProjectConfig::projects()[name].setAutoload(state == Qt::Checked);
}

void ProjectConfigTab::switchProjectSlot(int index) {
	QString name = configUi_.projectsComboBox->itemText(index);
	configUi_.projectGroupBox->setTitle(QLatin1String("Project: ") + name);
	configUi_.mmRootLineEdit->setText (ProjectConfig::projects()[name].mmRoot());
	configUi_.rusRootLineEdit->setText(ProjectConfig::projects()[name].rusRoot());
	configUi_.rusMainLineEdit->setText(ProjectConfig::projects()[name].rusMain());
	configUi_.autoinitCheckBox->setCheckState(ProjectConfig::projects()[name].autoinit() ? Qt::Checked : Qt::Unchecked);
	configUi_.autoloadCheckBox->setCheckState(ProjectConfig::projects()[name].autoload() ? Qt::Checked : Qt::Unchecked);
}

void ProjectConfigTab::initProjectSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	QString command = ProjectConfig::projects()[name].initProjectCommand();
	Execute::russellClient().execute(command);
}

void ProjectConfigTab::loadMainSlot() {
	QString name = configUi_.projectsComboBox->currentText();
	QString command = ProjectConfig::projects()[name].loadMainCommand();
	Execute::russellClient().execute(command);
}

	/****************************
	 *	Private members
	 ****************************/

void ProjectConfigTab::loadConfig() {
    int count = configGroup_.readEntry(QLatin1String("ProjectsCount"), 0);
    for (int i = 0; i < count; ++i) {
    	loadConfigForProject(i);
    }
    int index = configGroup_.readEntry(QLatin1String("ProjectIndex"), -1);
    configUi_.projectsComboBox->setCurrentIndex(index);
    switchProjectSlot(index);
}

void ProjectConfigTab::loadConfigForProject(int i) {
	QString name = configGroup_.readEntry(QStringLiteral("Projects %1 name").arg(i), "");
	ProjectConfig::projects()[name].setRusRoot(configGroup_.readEntry(QStringLiteral("Projects %1 Rus root").arg(i), ""));
	ProjectConfig::projects()[name].setRusMain(configGroup_.readEntry(QStringLiteral("Projects %1 Rus main").arg(i), ""));
	ProjectConfig::projects()[name].setMmRoot (configGroup_.readEntry(QStringLiteral("Projects %1 Mm  root").arg(i), ""));
	ProjectConfig::projects()[name].setAutoinit(configGroup_.readEntry(QStringLiteral("Projects %1 autoinit").arg(i)) == QStringLiteral("true"));
	ProjectConfig::projects()[name].setAutoload(configGroup_.readEntry(QStringLiteral("Projects %1 autoload").arg(i)) == QStringLiteral("true"));
	addProject(name);
}


void ProjectConfigTab::saveConfig() {
	int count = configUi_.projectsComboBox->count();
	configGroup_.writeEntry(QLatin1String("ProjectsCount"), count);
	int index = configUi_.projectsComboBox->currentIndex();
	configGroup_.writeEntry(QLatin1String("ProjectIndex"), index);
	for (int i = 0; i < count; ++i) {
		saveConfigForProject(i);
    }
}

void ProjectConfigTab::saveConfigForProject(int i) {
	QString name = configUi_.projectsComboBox->itemText(i);
	configGroup_.writeEntry(QStringLiteral("Projects %1 name").arg(i), name);
	configGroup_.writeEntry(QStringLiteral("Projects %1 Rus root").arg(i), ProjectConfig::projects()[name].rusRoot());
	configGroup_.writeEntry(QStringLiteral("Projects %1 Rus main").arg(i), ProjectConfig::projects()[name].rusMain());
	configGroup_.writeEntry(QStringLiteral("Projects %1 Mm  root").arg(i), ProjectConfig::projects()[name].mmRoot());
	configGroup_.writeEntry(QStringLiteral("Projects %1 autoinit").arg(i), ProjectConfig::projects()[name].autoinit() ? "true" : "false");
	configGroup_.writeEntry(QStringLiteral("Projects %1 autoload").arg(i), ProjectConfig::projects()[name].autoload() ? "true" : "false");
}

void ProjectConfigTab::removeConfigForProject(int i) {
	configGroup_.writeEntry(QStringLiteral("ProjectsCount"), configUi_.projectsComboBox->count());
	QString name = configUi_.projectsComboBox->itemText(i);
	configGroup_.deleteEntry(QStringLiteral("Projects %1 name").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 Rus root").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 Rus main").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 Mm root").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 autoinit").arg(i));
	configGroup_.deleteEntry(QStringLiteral("Projects %1 autoload").arg(i));
}

}
