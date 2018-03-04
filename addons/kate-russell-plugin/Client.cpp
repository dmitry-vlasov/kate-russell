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

#include <QByteArray>
#include <QDataStream>
#include <QtNetwork>
#include <QMetaObject>
#include <QMessageBox>

#include <KMessageBox>

#include <KTextEditor/View>

#include "RussellConfigPage.hpp"
#include "ProjectConfigTab.hpp"
#include "Client.hpp"

#include "Execute.hpp"
#include "View.hpp"
#include "Proof.hpp"

namespace russell {

	struct FileConf {
		QString file;
		const ProjectConfig* conf;
		operator bool() const { return conf; }
	};

	static FileConf chooseFileConf(const QString& file, ActionScope scope) {
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

	/****************************
	 *	Public members
	 ****************************/

	Client :: Client (View* view):
	view_ (view) {
		setupSlotsAndSignals();
	}

	bool
	Client :: execute(const QString& command) {
		return Execute::russell().execute (command);
	}
	bool
	Client::read(const QString& file, ActionScope scope) {
		if (FileConf fc = chooseFileConf(file, scope)) {
			if (fileChanged(fc.file)) {
				QString command;
				QString lang = lang_string(file_type(fc.file));
				command += lang + QStringLiteral(" curr proj=") + fc.conf->name() + QStringLiteral(";");
				command += lang + QStringLiteral(" read ") + QStringLiteral("in=") + fc.conf->trimFile(file) + QStringLiteral(";");
				command += lang + QStringLiteral(" parse ;");
				command += lang + QStringLiteral(" verify ") + QStringLiteral("in=") + fc.conf->trimFile(file) + QStringLiteral(";");
				timestamps[fc.file] = efs::last_write_time(fc.file.toStdString());
				return Execute::russell().execute (command);
			} else {
				return true;
			}
		} else {
			return false;
		}
	}
/*
	void
	Client :: setupInFile()
	{
		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		//sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		QString command = QStringLiteral("setup ");
		command += QStringLiteral(" --in ");
		command += sourcePath;
		Connection::mod().execute (command);
	}
	void
	Client :: setupOutFile()
	{
		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		//sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		QString command = QStringLiteral("setup ");
		command += QStringLiteral(" -w ");
		command += QStringLiteral(" --out ");
		command += sourcePath;
		Connection::mod().execute (command);
	}
	void
	Client :: setupPosition (const int line, const int column)
	{
		QString command = QStringLiteral("setup");
		command += QStringLiteral(" --line ");
		command += QString :: number (line + 1);
		command += QStringLiteral(" --column ");
		command += QString :: number (column + 1);
		command += QStringLiteral(" ");
		Connection::mod().execute (command);
	}
	void
	Client :: setupIndex (const int index)
	{
		QString command = QStringLiteral("setup");
		command += QStringLiteral(" --index ");
		command += QString :: number (index);
		command += QStringLiteral(" ");
		Connection::mod().execute (command);
	}
*/
	void
	Client :: expandNode (const long node)
	{
		QString command = QStringLiteral("expand ");
		command += QString::number(node);
		Execute::russell().execute (command);
	}

		bool
	Client :: prove (const bool clearOutput)
	{
		if (!view_->currentIsRus()) {
			return false;
		}
		QString command; // (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --prove ");
		//command += config_->getProveOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath; // = globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		command.replace (QStringLiteral("%f"), sourcePath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	//return startProcess (config_->getSourceRoot(), command);
		return false;
	}
	bool
	Client :: translate (const QString& file, ActionScope scope, Lang target)
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

			if (!Execute::russell().execute (command)) return false;
			view_->clearOutput();
			return true;
		} else {
			return false;
		}

		/*
		if (!view_->currentIsRus()) {
			return false;
		}
		QString command; // (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No translator is specified."));
			return false;
		}
		command += QStringLiteral(" -t ");
		//command += config_->getTranslateOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath; // = globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);
		QString targetPath (sourcePath);
		targetPath.chop (3);
		targetPath += QStringLiteral("smm");
		QString globalTargetPath; // (config_->getTargetRoot());
		globalTargetPath += QStringLiteral("/");
		globalTargetPath += targetPath;

		command.replace (QStringLiteral("%s"), sourcePath);
		command.replace (QStringLiteral("%t"), globalTargetPath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return false; //startProcess (config_->getSourceRoot(), command);*/

	}
	bool
	Client :: verify (const QString& file, ActionScope scope)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString lang = lang_string(file_type(fc.file));

			QString command;
			command += lang + QStringLiteral(" curr proj=") + fc.conf->name() + QStringLiteral(";");

			command += lang + QStringLiteral(" verify ");
			command += QStringLiteral("in=") + fc.conf->rusTarget(fc.file) + QStringLiteral(";");
			command += QStringLiteral("out=") + fc.conf->smmTarget(fc.file) + QStringLiteral(";");

			if (!Execute::russell().execute (command)) return false;
			//view_->clearOutput();
			return true;
		} else {
			return false;
		}

		/*
		if (!view_->currentIsRus() && !view_->currentIsMetamath()) {
			return false;
		}
		QString command; // (config_->getVerifier());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No verifyer is specified."));
			return false;
		}
		command += QStringLiteral(" ");
		//command += config_->getVerifyOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString globalPath (url.toLocalFile());
		QString midPath;
		/*(
			view_->currentIsMetamath() ?
			globalPath.mid (config_->getSourceRoot().size() + 1, -1) :
			globalPath.mid (config_->getTargetRoot().size() + 1, -1)
		);* /
		QString targetPath (midPath);
		targetPath.chop (3);
		targetPath += QStringLiteral("smm");

		command.replace (QStringLiteral("%f"), targetPath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return false; //startProcess (config_->getTargetRoot(), command);
    	*/
	}
	bool
	Client :: merge (const QString& file, ActionScope scope)
	{
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString lang = lang_string(file_type(fc.file));
			QString command;
			command += QStringLiteral("mm curr proj=") + fc.conf->name() + QStringLiteral(";");
			command += QStringLiteral("mm merge ");
			command += QStringLiteral("in=") + fc.conf->mmTarget(fc.file) + QStringLiteral(" ");
			command += QStringLiteral("out=") + fc.conf->mergedTarget(fc.file) + QStringLiteral(" ");
			command += QStringLiteral("out-root=") + fc.conf->mmRoot();

			if (!Execute::russell().execute (command)) return false;
			//view_->clearOutput();
			return true;
		} else {
			return false;
		}

		/*
		if (!view_->currentIsRus() && !view_->currentIsMetamath()) {
			return false;
		}
		QString command; // (config_->getVerifier());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No verifyer is specified."));
			return false;
		}
		command += QStringLiteral(" ");
		//command += config_->getVerifyOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString globalPath (url.toLocalFile());
		QString midPath;
		/*(
			view_->currentIsMetamath() ?
			globalPath.mid (config_->getSourceRoot().size() + 1, -1) :
			globalPath.mid (config_->getTargetRoot().size() + 1, -1)
		);* /
		QString targetPath (midPath);
		targetPath.chop (3);
		targetPath += QStringLiteral("smm");

		command.replace (QStringLiteral("%f"), targetPath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return false; //startProcess (config_->getTargetRoot(), command);
    	*/
	}
	bool
	Client::verifyMm(const QString& file, ActionScope scope) {
		if (FileConf fc = chooseFileConf(file, scope)) {
			QString command;
			command += QStringLiteral("read \"") + fc.conf->mmRoot() + QStringLiteral("/") + fc.conf->mergedTarget(fc.file) + QStringLiteral("\"");
			command += QStringLiteral(" / verify\n");
			//qDebug() << command;
			return Execute::metamath().execute(command);
		} else {
			return false;
		}
	}
	bool
	Client :: learn (const bool clearOutput)
	{
		if (!view_->currentIsRus()) {
			return false;
		}
		QString command; // (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --learn ");
		//command += config_->getLearnOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath; // = globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);
		QString universePath; // (config_->getSourceUniverse());

		command.replace (QStringLiteral("%f"), sourcePath);
		command.replace (QStringLiteral("%u"), universePath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return false; //startProcess (config_->getSourceRoot(), command);
	}
	bool
	Client :: lookupDefinition (const QString& file, const int line, const int column)
	{
		const ProjectConfig* conf = ProjectConfig::find(file);
		if (!conf) return false;
		QString command = QStringLiteral("rus curr proj=") + conf->name() + QStringLiteral(";");
		//if (!Connection::mod().execute (command)) return false;
		command += QStringLiteral("rus lookup what=def in=") + conf->trimFile(file) + QStringLiteral(" ");
		command += QStringLiteral("line=") + QString::number(line) + QStringLiteral(" ");
		command += QStringLiteral("col=") + QString::number(column);
		if (!Execute::russell().execute (command)) return false;
		view_->clearOutput();
		return true;
/*
		QString command; // (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No miner is specified."));
			return false;
		}
		command += QStringLiteral(" --lookup-definition ");
		//command += config_->getLookupOption();

		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		//sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		command.replace (QStringLiteral("%l"), QString :: number (line + 1));
		command.replace (QStringLiteral("%c"), QString :: number (column + 1));
		command.replace (QStringLiteral("%f"), sourcePath);
		view_->clearOutput();

    	return false; //startProcess (config_->getSourceRoot(), command);
 */
	}
	bool
	Client :: lookupLocation (const QString& file, const int line, const int column)
	{
		const ProjectConfig* conf = ProjectConfig::find(file);
		if (!conf) return false;
		QString command = QStringLiteral("rus curr proj=") + conf->name() + QStringLiteral(";");
		//if (!Connection::mod().execute (command)) return false;
		command += QStringLiteral("rus lookup what=loc in=") + conf->trimFile(file) + QStringLiteral(" ");
		command += QStringLiteral("line=") + QString::number(line) + QStringLiteral(" ");
		command += QStringLiteral("col=") + QString::number(column);
		if (!Execute::russell().execute (command)) return false;
		view_->clearOutput();

		/*if (!Connection::mod().data().isEmpty()) {
			QString def = i18n("Definition:");
			QMessageBox::information(view_->mainWindow()->activeView(), def, Connection::mod().data());
		}*/
		//view_->openLocation(Connection::mod().data());
		return true;
/*
		if (!view_->currentIsRus()) {
			return false;
		}
		QString command; // (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --lookup-location ");
		//command += config_->getLookupOption();

		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		//sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		command.replace (QStringLiteral("%l"), QString :: number (line + 1));
		command.replace (QStringLiteral("%c"), QString :: number (column + 1));
		command.replace (QStringLiteral("%f"), sourcePath);
		view_->clearOutput();
    	return false; //startProcess (config_->getSourceRoot(), command);
   */
	}
	bool
	Client :: mine (const QString& file, const QString& options)
	{
		const ProjectConfig* conf = ProjectConfig::find(file);
		if (!conf) return false;
		QString command = QStringLiteral("rus curr proj=") + conf->name() + QStringLiteral(";");
		//if (!Connection::mod().execute (command)) return false;
		switch (view_->getState()) {
		case View :: MINING_OUTLINE :
			command += QStringLiteral("rus outline in=") + conf->trimFile(file) + QStringLiteral(" ");
			command += QStringLiteral("what=") + options;
			break;
		case View :: MINING_STRUCTURE :
		case View :: MINING_TYPE_SYSTEM :
			command  = QStringLiteral("rus struct ");
			command += QStringLiteral("what=") + options;
			break;
		default : return false;
		}
		if (!Execute::russell().execute (command)) return false;
		//view_->setOutput(Connection::mod().data());
		//view_->update();
		return true;

		///////////////////
/*
		if (!view_->currentIsRus()) {
			return false;
		}
		QString command; // (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No miner is specified."));
			return false;
		}
		switch (view_->getState()) {
		case View :: MINING_OUTLINE :
			command += QStringLiteral(" --mine-outline "); break;
		case View :: MINING_STRUCTURE :
			command += QStringLiteral(" --mine-structure "); break;
		case View :: MINING_TYPE_SYSTEM :
			command += QStringLiteral(" --mine-type-system "); break;
		default : return false;
		}
		command += options;

		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		//sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);
		command += sourcePath;

		view_->clearOutput();

		//KMessageBox :: sorry (0, command);

    	return false; //startProcess (config_->getSourceRoot(), command);
    */
	}

	bool
	Client :: prove (const int line, const int column, const bool clearOutput)
	{
		if (!view_->currentIsRus()) {
			return false;
		}
		QString command; // (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --prove ");
		command += QStringLiteral(" --line ");
		command += QString :: number (line + 1);
		command += QStringLiteral(" --column ");
		command += QString :: number (column + 1);
		command += QStringLiteral(" ");
		//command += config_->getProveOption();

		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		//sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);
		command.replace (QStringLiteral("%f"), sourcePath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return false; //startProcess (config_->getSourceRoot(), command);
	}

	/****************************
	 *	Public slots
	 ****************************/

	static bool reloadProverCommand (const QString& command) {
		return (command == QStringLiteral("exit")) || (command == QStringLiteral("read"));
	}

	void
	Client :: executeCommand()
	{
		QComboBox* commandComboBox = view_->getBottomUi().russellCommandComboBox;
		QString command = commandComboBox->currentText();
		bool addCommand = true;
		for (int i = 0; i < commandComboBox->count(); ++ i) {
			if (commandComboBox->itemText (i) == command) {
				addCommand = false;
				break;
			}
		}
		if (addCommand) {
			commandComboBox->addItem (command);
		}
		command = command.trimmed();
		if (command[0] == QLatin1Char ('>')) {
			command.remove (0, 1);
		}
		command = command.trimmed();
		if (command.size() == 0) {
			int row = view_->getBottomUi().russellListWidget->count();
			view_->getBottomUi().russellListWidget->insertItem (row, QStringLiteral("> "));
			row = view_->getBottomUi().russellListWidget->count();
			view_->getBottomUi().russellListWidget->setCurrentRow (row);
		} else if (command == QStringLiteral("fell")) {
			view_->proof()->stopProving();
		} else if (reloadProverCommand (command)) {
			view_->proof()->stopProving();
			Execute::russell().execute (command);
		} else {
			Execute::russell().execute (command);
		}
	}
	void
	Client :: clearConsole()
	{
		QListWidget* listWidget = view_->getBottomUi().russellListWidget;
		const int count = listWidget->count();
		for (int i = 0; i < count; ++ i) {
			const int j = count - i - 1;
			QListWidgetItem* item = listWidget->item(j);
			listWidget->removeItemWidget (item);
		}
	}

	/****************************
	 *	Private members
	 ****************************/

	void
	Client :: setupSlotsAndSignals()
	{
		/*
		QObject :: connect
		(
			view_->getBottomUi().russellExecuteButton,
			SIGNAL (pressed()),
			this,
			SLOT (executeCommand())
		);*/
		/*
		QObject :: connect
		(
			view_->getBottomUi().russellClearButton,
			SIGNAL (pressed()),
			this,
			SLOT (clearConsole())
		);*/
	}

	bool Client::fileChanged(const QString& file) const {
		auto p = timestamps.find(file);
		if (p != timestamps.end()) {
			return p->second != efs::last_write_time(file.toStdString());
		} else {
			return true;
		}
	}

}
