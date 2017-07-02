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
		/*if (!Launcher :: checkLocal (config_->getSourceRoot())) {
			return;
		}*/
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
		/*if (!Launcher :: checkLocal (config_->getSourceRoot())) {
			return;
		}*/
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

