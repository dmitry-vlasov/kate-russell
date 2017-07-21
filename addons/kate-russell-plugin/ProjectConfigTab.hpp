/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_config_Config.hpp                    */
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

#pragma once

#include <QString>
#include <QWidget>

#include <KConfigGroup>

#include "ui_ProjectsTab.h"

namespace russell {

class ProjectConfig {
public:
	const QString& name() const { return name_; }
	const QString& rusRoot() const { return rusRoot_; }
	const QString& rusMain() const { return rusMain_; }
	const QString& smmRoot() const { return smmRoot_; }
	const QString&  mmRoot() const { return  mmRoot_; }

	void setName(const QString& name) { name_ = name; }
	void setSmmRoot(const QString& smmRoot) { smmRoot_ = smmRoot; }
	void setRusRoot(const QString& rusRoot) { rusRoot_ = rusRoot; }
	void setMmRoot(const QString& mmRoot) { mmRoot_ = mmRoot; }
	void setRusMain(const QString& rusMain) { rusMain_ = rusMain; }

	void initProject();
	void loadMain();

	QString trimFile(const QString& file) const;
	QString smmTarget(const QString& file) const;
	QString mmTarget(const QString& file) const;

	static QMap<QString, ProjectConfig>& projects() {
		static QMap<QString, ProjectConfig> projects_;
		return projects_;
	}

	static const ProjectConfig* find(const QString& file);

private:
	QString name_;
	QString rusRoot_;
	QString rusMain_;
	QString smmRoot_;
	QString mmRoot_;
};

class ProjectConfigTab : public QWidget {
Q_OBJECT
public:
	ProjectConfigTab(QWidget* parent);
	~ProjectConfigTab();

private Q_SLOTS:
	void addProjectSlot();
	void delProjectSlot();
	void chooseRussellMainSlot();
	void chooseRussellRootSlot();
	void chooseSmmRootSlot();
	void chooseMmRootSlot();
	void switchProjectSlot(int);
	void initProjectSlot();
	void loadMainSlot();

private:
	void addProject(const QString&);
	void loadConfig();
	void loadConfigForProject(int i);
	void saveConfig();
	void saveConfigForProject(int i);
	void removeConfigForProject(int i);

	Ui::ProjectsTab configUi_;
	KConfigGroup configGroup_;
};

}
