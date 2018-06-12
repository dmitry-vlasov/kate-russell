/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Client.cpp                       */
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

#include <experimental/filesystem>
#include <map>

#include "commands.hpp"

#include "RussellConfigPage.hpp"
#include "ProjectConfigTab.hpp"
#include "Execute.hpp"
#include "View.hpp"
#include "Proof.hpp"

namespace russell { namespace command {

	QStringList read(const QString& file, ActionScope scope)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QStringList commands;
			QString lang = lang_string(file_type(fc.file));
			commands << lang + QLatin1String(" curr proj=") + fc.conf->name();
			commands << lang + QLatin1String(" read in=") + fc.conf->trimFile(file);
			commands << lang + QLatin1String(" parse");
			commands << lang + QLatin1String(" verify in=") + fc.conf->trimFile(file);
			return commands;
		} else {
			return QStringList();
		}
	}

	QStringList translate(const QString& file, ActionScope scope, Lang target)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QStringList commands;
			QString src_lang = lang_string(file_type(fc.file));
			QString tgt_lang = lang_string(target);
			commands << src_lang + QLatin1String(" curr proj=") + fc.conf->name();
			commands << tgt_lang + QLatin1String(" curr proj=") + fc.conf->name();

			commands << src_lang + QLatin1String(" transl in=") + fc.conf->trimFile(fc.file) + QLatin1String(" out=") + fc.conf->target(fc.file, target);
			commands << tgt_lang + QLatin1String(" write deep=true in=") + fc.conf->target(fc.file, target);

			return commands;
		} else {
			return QStringList();
		}
	}

	QStringList merge(const QString& file, ActionScope scope)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString lang = lang_string(file_type(fc.file));
			QStringList commands;
			commands << QLatin1String("mm curr proj=") + fc.conf->name();
			commands <<
				QLatin1String("mm merge in=") + fc.conf->trimFile(fc.file) +
				QLatin1String(" out=") + fc.conf->mergedTarget(fc.file) +
				QLatin1String(" out-root=") + fc.conf->mmRoot();
			return commands;
		} else {
			return QStringList();
		}
	}

	QStringList verifyRus(const QString& file, ActionScope scope)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString lang = lang_string(file_type(fc.file));
			QStringList commands;
			commands << lang + QLatin1String(" curr proj=") + fc.conf->name();
			commands << lang + QLatin1String(" verify in=") + fc.conf->rusTarget(fc.file);
			return commands;
		} else {
			return QStringList();
		}
	}

	QString verifyMm(const QString& file, ActionScope scope) {
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString command;
			command += QLatin1String("read \"") + fc.conf->mergedTarget(fc.file, true) + QLatin1String("\"");
			command += QLatin1String(" / verify \n");
			//qDebug() << command;
			return command;
		} else {
			return QString();
		}
	}

	QString eraseMm(const QString& file, ActionScope scope) {
		if (FileConf fc = chooseFileConf(file, scope)) {
			//return QLatin1String("erase \"") + fc.conf->mergedTarget(fc.file, true) + QLatin1String("\"\n");
			return QLatin1String("erase\n");
		} else {
			return QString();
		}
	}

	QStringList lookupDefinition(const QString& file, const int line, const int column)
	{
		if (FileConf fc = chooseFileConf(file, ActionScope::FILE)) {
			QStringList commands;
			commands << QLatin1String("rus curr proj=") + fc.conf->name();
			commands <<
				QLatin1String("rus lookup what=def in=") + fc.conf->trimFile(fc.file) +
				QLatin1String(" line=") + QString::number(line) +
				QLatin1String(" col=") + QString::number(column);
			return commands;
		} else {
			return QStringList();
		}
	}

	QStringList lookupLocation(const QString& file, const int line, const int column)
	{
		if (FileConf fc = chooseFileConf(file, ActionScope::FILE)) {
			QStringList commands;
			commands << QLatin1String("rus curr proj=") + fc.conf->name();
			commands <<
				QLatin1String("rus lookup what=loc in=") + fc.conf->trimFile(file) +
				QLatin1String(" line=") + QString::number(line) +
				QLatin1String(" col=") + QString::number(column);
			return commands;
		} else {
			return QStringList();
		}
	}

	QStringList mine(const QString& file, State state, const QString& options)
	{
		if (FileConf fc = chooseFileConf(file, ActionScope::FILE)) {
			QStringList commands;
			commands << QLatin1String("rus curr proj=") + fc.conf->name();
			//if (!Connection::mod().execute (command)) return false;
			switch (state) {
			case State::MINING_OUTLINE :
				commands << QLatin1String("rus outline in=") + fc.conf->trimFile(fc.file) + QLatin1String(" what=") + options;
				break;
			case State::MINING_STRUCTURE :
			case State::MINING_TYPE_SYSTEM :
				commands << QLatin1String("rus struct what=") + options;
				break;
			default : return QStringList();
			}
			return commands;
		} else {
			return QStringList();
		}
	}
}
	namespace efs = std::experimental::filesystem;

	static std::map<QString, efs::file_time_type> timestamps;

	bool fileChanged(const QString& file) {
		auto p = timestamps.find(file);
		if (p != timestamps.end()) {
			return p->second != efs::last_write_time(file.toStdString());
		} else {
			return true;
		}
	}

	void registerFileRead(const QString& file) {
		timestamps[file] = efs::last_write_time(file.toStdString());
	}

	FileConf chooseFileConf(const QString& file, ActionScope scope) {
		const ProjectConfig* conf = ProjectConfig::find(file);
		if (!conf) return FileConf {file, nullptr};
		switch (scope) {
		case ActionScope::PROJ: {
			auto projFile = conf->rusRoot() + QLatin1String("/") + conf->rusMain();
			return FileConf {projFile, conf};
		}
		case ActionScope::FILE: return FileConf{file, conf};
		default: return FileConf {file, conf};
		}
	}
}
