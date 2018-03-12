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
//#include <QByteArray>
//#include <QDataStream>
//#include <QtNetwork>
//#include <QMetaObject>
//#include <QMessageBox>

//#include <KMessageBox>

//#include <KTextEditor/View>

#include "RussellConfigPage.hpp"
#include "ProjectConfigTab.hpp"
#include "Execute.hpp"
#include "View.hpp"
#include "Proof.hpp"

namespace russell { namespace command {

	QString read(const QString& file, ActionScope scope)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString command;
			QString lang = lang_string(file_type(fc.file));
			command += lang + QStringLiteral(" curr proj=") + fc.conf->name() + QStringLiteral(";");
			command += lang + QStringLiteral(" read ") + QStringLiteral("in=") + fc.conf->trimFile(file) + QStringLiteral(";");
			command += lang + QStringLiteral(" parse ;");
			command += lang + QStringLiteral(" verify ") + QStringLiteral("in=") + fc.conf->trimFile(file) + QStringLiteral(";");
			return command;
		} else {
			return QString();
		}
	}

	QString translate(const QString& file, ActionScope scope, Lang target)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString command;
			QString src_lang = lang_string(file_type(fc.file));
			QString tgt_lang = lang_string(target);
			command += src_lang + QStringLiteral(" curr proj=") + fc.conf->name() + QStringLiteral(";");
			command += tgt_lang + QStringLiteral(" curr proj=") + fc.conf->name() + QStringLiteral(";");

			command += src_lang + QStringLiteral(" transl ");
			command += QStringLiteral("in=") + fc.conf->trimFile(fc.file) + QStringLiteral(" ");
			command += QStringLiteral("out=") + fc.conf->target(fc.file, target) + QStringLiteral(";");
			command += tgt_lang + QStringLiteral(" write deep=true ");
			command += QStringLiteral("in=") + fc.conf->target(fc.file, target) + QStringLiteral(";");

			return command;
		} else {
			return QString();
		}
	}

	QString merge(const QString& file, ActionScope scope)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString lang = lang_string(file_type(fc.file));
			QString command;
			command += QStringLiteral("mm curr proj=") + fc.conf->name() + QStringLiteral(";");
			command += QStringLiteral("mm merge ");
			command += QStringLiteral("in=") + fc.conf->trimFile(fc.file) + QStringLiteral(" ");
			command += QStringLiteral("out=") + fc.conf->mergedTarget(fc.file) + QStringLiteral(" ");
			command += QStringLiteral("out-root=") + fc.conf->mmRoot() + QStringLiteral(";");
			return command;
		} else {
			return QString();
		}
	}

	QString verifyRus(const QString& file, ActionScope scope)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString lang = lang_string(file_type(fc.file));
			QString command;
			command += lang + QStringLiteral(" curr proj=") + fc.conf->name() + QStringLiteral(";");
			command += lang + QStringLiteral(" verify ");
			command += QStringLiteral("in=") + fc.conf->rusTarget(fc.file) + QStringLiteral(";");
			return command;
		} else {
			return QString();
		}
	}

	QString verifyMm(const QString& file, ActionScope scope) {
		if (FileConf fc = chooseFileConf(file, scope)) {
			translate(file, scope, Lang::SMM);
			translate(file, scope, Lang::MM);
			merge(file, scope);
			QString command;
			command += QStringLiteral("read \"") + fc.conf->mergedTarget(fc.file, true) + QStringLiteral("\"");
			command += QStringLiteral(" / verify\n");
			//qDebug() << command;
			return command;
		} else {
			return QString();
		}
	}


	QString lookupDefinition(const QString& file, const int line, const int column)
	{
		if (FileConf fc = chooseFileConf(file, ActionScope::FILE)) {
			QString command = QStringLiteral("rus curr proj=") + fc.conf->name() + QStringLiteral(";");
			command += QStringLiteral("rus lookup what=def in=") + fc.conf->trimFile(fc.file) + QStringLiteral(" ");
			command += QStringLiteral("line=") + QString::number(line) + QStringLiteral(" ");
			command += QStringLiteral("col=") + QString::number(column) + QStringLiteral(";");
			return command;
		} else {
			return QString();
		}
	}

	QString lookupLocation(const QString& file, const int line, const int column)
	{
		if (FileConf fc = chooseFileConf(file, ActionScope::FILE)) {
			QString command = QStringLiteral("rus curr proj=") + fc.conf->name() + QStringLiteral(";");
			command += QStringLiteral("rus lookup what=loc in=") + fc.conf->trimFile(file) + QStringLiteral(" ");
			command += QStringLiteral("line=") + QString::number(line) + QStringLiteral(" ");
			command += QStringLiteral("col=") + QString::number(column) + QStringLiteral(";");
			return command;
		} else {
			return QString();
		}
	}

	QString mine(const QString& file, State state, const QString& options)
	{
		if (FileConf fc = chooseFileConf(file, ActionScope::FILE)) {
			QString command = QStringLiteral("rus curr proj=") + fc.conf->name() + QStringLiteral(";");
			//if (!Connection::mod().execute (command)) return false;
			switch (state) {
			case State::MINING_OUTLINE :
				command += QStringLiteral("rus outline in=") + fc.conf->trimFile(fc.file) + QStringLiteral(" ");
				command += QStringLiteral("what=") + options + QStringLiteral(";");
				break;
			case State::MINING_STRUCTURE :
			case State::MINING_TYPE_SYSTEM :
				command  = QStringLiteral("rus struct ");
				command += QStringLiteral("what=") + options + QStringLiteral(";");
				break;
			default : return QString();
			}
			return command;
		} else {
			return QString();
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
			auto projFile = conf->rusRoot() + QStringLiteral("/") + conf->rusMain();
			return FileConf {projFile, conf};
		}
		case ActionScope::FILE: return FileConf{file, conf};
		default: return FileConf {file, conf};
		}
	}
}
