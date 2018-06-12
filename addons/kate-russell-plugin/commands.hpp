/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Client.hpp                       */
/* Description:     a client for mdl as a server                             */
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

#include "Enums.hpp"

namespace russell {

	class ProjectConfig;

namespace command {

	QStringList read(const QString& file, ActionScope scope = ActionScope::FILE);
	QStringList translate(const QString& file, ActionScope scope = ActionScope::FILE, Lang target = Lang::MM);
	QStringList merge(const QString& file, ActionScope scope = ActionScope::FILE);

	QStringList verifyRus(const QString& file, ActionScope scope = ActionScope::FILE);
	QString     verifyMm(const QString& file, ActionScope scope = ActionScope::FILE);
	QString     eraseMm(const QString& file, ActionScope scope = ActionScope::FILE);

	QStringList lookupDefinition(const QString& file, const int line, const int column);
	QStringList lookupLocation(const QString& file, const int line, const int column);
	QStringList mine (const QString& file, State state, const QString& options);

	//QString prove(const QString& file, ProvingMode mode);
	//QString prove(const QString& file, ProvingMode mode, const int line, const int column);
	//QString expandNode(const long);

	//QString learn(const bool clearOutput = true);
}
	bool fileChanged(const QString& file);
	void registerFileRead(const QString& file);

	struct FileConf {
		QString file;
		const ProjectConfig* conf;
		operator bool() const { return conf; }
	};

	FileConf chooseFileConf(const QString& file, ActionScope scope);
}
