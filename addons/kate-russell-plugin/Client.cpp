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

#ifndef PLUGIN_KATE_RUSSELL_MDL_CLIENT_CPP_
#define PLUGIN_KATE_RUSSELL_MDL_CLIENT_CPP_

#include <QByteArray>
#include <QDataStream>
#include <QtNetwork>
#include <QMetaObject>
#include <cstring>

#include "russell.hpp"
#include "Connection.hpp"
#include "RussellConfigPage.hpp"
#include "ProjectConfigTab.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace mdl{

	/****************************
	 *	Public members
	 ****************************/

	Client :: Client (View* view):
	view_ (view) {
		setupSlotsAndSignals();
	}

	bool
	Client :: execute(const QString& command) {
		return Connection::mod().execute (command);
	}

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
	void
	Client :: expandNode (const long node)
	{
		QString command = QStringLiteral("expand ");
		command += QString::number(node);
		Connection::mod().execute (command);
	}

		bool
	Client :: prove (const bool clearOutput)
	{
		if (!view_->currentIsRussell()) {
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
	Client :: translate (const bool clearOutput)
	{
		if (!view_->currentIsRussell()) {
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
    	return false; //startProcess (config_->getSourceRoot(), command);
	}
	bool
	Client :: verify (const bool clearOutput)
	{
		if (!view_->currentIsRussell() && !view_->currentIsMetamath()) {
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
		);*/
		QString targetPath (midPath);
		targetPath.chop (3);
		targetPath += QStringLiteral("smm");

		command.replace (QStringLiteral("%f"), targetPath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return false; //startProcess (config_->getTargetRoot(), command);
	}
	bool
	Client :: learn (const bool clearOutput)
	{
		if (!view_->currentIsRussell()) {
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
	Client :: lookupDefinition (const int line, const int column)
	{
		if (!view_->currentIsRussell()) return false;
		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString file = url.toLocalFile();
		const ProjectConfig* conf = ProjectConfig::find(file);
		if (!conf) return false;
		QString command = QStringLiteral("rus curr proj=") + conf->name();
		if (!Connection::mod().execute (command)) return false;
		command = QStringLiteral("rus lookup what=def in=") + conf->trimFile(file) + QStringLiteral(" ");
		command += QStringLiteral("line=") + QString::number(line) + QStringLiteral(" ");
		command += QStringLiteral("col=") + QString::number(column);
		if (!Connection::mod().execute (command)) return false;
		view_->clearOutput();

		if (!Connection::mod().data().isEmpty()) {
			QString def = i18n("Definition:");
			QMessageBox::information(view_->mainWindow()->activeView(), def, Connection::mod().data());
		}
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
	Client :: lookupLocation (const int line, const int column)
	{
		if (!view_->currentIsRussell()) return false;
		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString file = url.toLocalFile();
		const ProjectConfig* conf = ProjectConfig::find(file);
		if (!conf) return false;
		QString command = QStringLiteral("rus curr proj=") + conf->name();
		if (!Connection::mod().execute (command)) return false;
		command = QStringLiteral("rus lookup what=loc in=") + conf->trimFile(file) + QStringLiteral(" ");
		command += QStringLiteral("line=") + QString::number(line) + QStringLiteral(" ");
		command += QStringLiteral("col=") + QString::number(column);
		if (!Connection::mod().execute (command)) return false;
		view_->clearOutput();

		/*if (!Connection::mod().data().isEmpty()) {
			QString def = i18n("Definition:");
			QMessageBox::information(view_->mainWindow()->activeView(), def, Connection::mod().data());
		}*/
		view_->openLocation(Connection::mod().data());
		return true;
/*
		if (!view_->currentIsRussell()) {
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
	Client :: mine (const QString& options)
	{
		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		QString file = url.toLocalFile();
		const ProjectConfig* conf = ProjectConfig::find(file);
		if (!conf) return false;
		QString command = QStringLiteral("rus curr proj=") + conf->name();
		if (!Connection::mod().execute (command)) return false;
		switch (view_->getState()) {
		case View :: MINING_OUTLINE :
			command  = QStringLiteral("rus outline what=loc in=") + conf->trimFile(file) + QStringLiteral(" ");
			command += QStringLiteral("what=") + options;
			break;
		case View :: MINING_STRUCTURE :
			//command = QStringLiteral(" --mine-structure ");
			//break;
		case View :: MINING_TYPE_SYSTEM :
			//command = QStringLiteral(" --mine-type-system ");
			//break;
		default : return false;
		}
		QMessageBox::information(
			view_->mainWindow()->activeView(),
			QStringLiteral("mining"),
			QStringLiteral("about to exec: \n") + command
		);
		if (!Connection::mod().execute (command)) return false;
		view_->update();
		return true;

		///////////////////
/*
		if (!view_->currentIsRussell()) {
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
		if (!view_->currentIsRussell()) {
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
		QComboBox* commandComboBox = view_->getBottomUi().commandComboBox;
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
			int row = view_->getBottomUi().serverListWidget->count();
			view_->getBottomUi().serverListWidget->insertItem (row, QStringLiteral("> "));
			row = view_->getBottomUi().serverListWidget->count();
			view_->getBottomUi().serverListWidget->setCurrentRow (row);
			return;
		} else if (command == QStringLiteral("fell")) {
			view_->proof()->fell();
		} else if (reloadProverCommand (command)) {
			view_->proof()->fell();
			Connection::mod().execute (command);
		} else {
			Connection::mod().execute (command);
		}
	}
	void
	Client :: clearConsole()
	{
		QListWidget* serverListWidget = view_->getBottomUi().serverListWidget;
		const int count = serverListWidget->count();
		for (int i = 0; i < count; ++ i) {
			const int j = count - i - 1;
			QListWidgetItem* item = serverListWidget->item(j);
			serverListWidget->removeItemWidget (item);
		}
	}

	/****************************
	 *	Private members
	 ****************************/

	void
	Client :: setupSlotsAndSignals()
	{
		QObject :: connect
		(
			view_->getBottomUi().executeButton,
			SIGNAL (pressed()),
			this,
			SLOT (executeCommand())
		);
		QObject :: connect
		(
			view_->getBottomUi().clearButton,
			SIGNAL (pressed()),
			this,
			SLOT (clearConsole())
		);
	}
}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_CLIENT_CPP_ */

