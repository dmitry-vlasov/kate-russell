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

#include "russell.dpp"
#include "x_include.hpp"
#include "ui_ProjectsTab.h"

namespace plugin {
namespace kate {
namespace russell {

struct ProjectConfig {
	QString rusRoot;
	QString smmRoot;

	static QMap<QString, ProjectConfig>& projects() {
		static QMap<QString, ProjectConfig> projects_;
		return projects_;
	}
};

class ProjectConfigTab : public QWidget {
Q_OBJECT
public:
	ProjectConfigTab(QWidget* parent);
	~ProjectConfigTab();

private Q_SLOTS:
	void addProjectSlot();
	void delProjectSlot();
	void chooseRussellRootSlot();
	void chooseSmmRootSlot();
	void switchProjectSlot(int);

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
}
}

