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

namespace plugin {
namespace kate {
namespace russell {
namespace mdl{

	/****************************
	 *	Public members
	 ****************************/

	Client :: Client (View* view, const Config* config):
	view_ (view),
	config_ (config),
	tcpSocket_ (new QTcpSocket (view)),
	isConnected_ (false),
	isRunning_ (false),
	data_ (),
	messages_ (),
	host_(),
	port_ () {
		setupSlotsAndSignals();
	}
	Client ::  ~ Client() {
		delete tcpSocket_;
	}

	void
	Client :: execute (const QString& command)
	{
		if (!connect()) {
			return;
		}
#if OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT
		QTextStream (stdout) << "========================================\n";
		QTextStream (stdout) << " about to execute:\n";
		QTextStream (stdout) << " \t" << command << "\n";
		QTextStream (stdout) << "========================================\n\n";
#endif
		QString consoleCommand = QStringLiteral("> ");
		consoleCommand += command;
		QListWidgetItem* commandItem = new QListWidgetItem (consoleCommand);
		view_->getBottomUi().serverListWidget->addItem (commandItem);

		data_.clear();
		messages_.clear();
		runCommand (command);
		readOutput();
		disconnect();
#if OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT
		QTextStream (stdout) << "\n\n\n";
#endif
	}
	const QString&
	Client :: getData() const {
		return data_;
	}
	const QString&
	Client :: getMessages() const {
		return messages_;
	}

	bool
	Client :: connect()
	{
		if (isConnected_) {
			std :: cout << "already connected to server" << std :: endl;
			return true;
		}
		host_ = config_->getServerHost();
		const QString& portString = config_->getServerPort();
		bool ok = true;
		port_ = portString.toInt (&ok);
		if (!ok) {
			port_ = -1;
			std :: cout << "non-numeric connection port" << std :: endl;
			isConnected_ = false;
		}
		tcpSocket_->connectToHost (host_, port_);
		isConnected_ = tcpSocket_->waitForConnected();
		if (!isConnected_) {
			std :: cout << "not connected to server:" << std :: endl;
			QTextStream (stdout) << "\t" << tcpSocket_->errorString() << "\n";
			QTextStream (stdout) << "\thost: " << host_ << "\n";
		}
		return isConnected_;
	}
	void
	Client :: disconnect()
	{
		tcpSocket_->disconnectFromHost();
		isConnected_ = false;
	}

	void
	Client :: setupInFile()
	{
		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return;
		}
		if (!Launcher :: checkLocal (config_->getSourceRoot())) {
			return;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		QString command = QStringLiteral("setup ");
		command += QStringLiteral(" --in ");
		command += sourcePath;
		execute (command);
	}
	void
	Client :: setupOutFile()
	{
		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return;
		}
		if (!Launcher :: checkLocal (config_->getSourceRoot())) {
			return;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		QString command = QStringLiteral("setup ");
		command += QStringLiteral(" -w ");
		command += QStringLiteral(" --out ");
		command += sourcePath;
		execute (command);
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
		execute (command);
	}
	void
	Client :: setupIndex (const int index)
	{
		QString command = QStringLiteral("setup");
		command += QStringLiteral(" --index ");
		command += QString :: number (index);
		command += QStringLiteral(" ");
		execute (command);
	}
	void
	Client :: expandNode (const long node)
	{
		QString command = QStringLiteral("expand ");
		command += QString::number(node);
		execute (command);
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
			execute (command);
		} else {
			execute (command);
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
	void
	Client :: runCommand (const QString& command)
	{
		char *asciiCommand = new char [command.length() + 1];
		for (int i = 0; i < command.length(); ++ i) {
			const QChar qch = command[i];
			asciiCommand [i] = qch.toLatin1();
		}
		asciiCommand [command.length()] = '\0';
		tcpSocket_->write (asciiCommand);
		if (tcpSocket_->waitForBytesWritten()) {
			isRunning_ = true;
		}
		delete[] asciiCommand;
	}
	void
	Client :: readOutput()
	{
		if (tcpSocket_->waitForReadyRead (300000)) {
			QByteArray read;
			bool start = true;
			while (true) {
				size_t bytesAvailable = 0;
				if (start || tcpSocket_->waitForReadyRead (3000)) {
					bytesAvailable = tcpSocket_->bytesAvailable();
					read.append (tcpSocket_->read (bytesAvailable));
				}
				start = false;
				if (bytesAvailable == 0) {
					break;
				}
 			}
			const char* buffer = read.data();
			const size_t size  = read.size();

			// Data is the first part of the received chunk of bytes
			data_ += QString :: fromUtf8 (buffer);
			// Messages are the second part of the received chunk of bytes
			const char* messages = buffer;
			while (*messages != '\0') {
				++ messages;
			}
			// Shift on one byte, because it is a delimiter: '\0'
			if (messages != buffer + size) {
				++ messages;
				messages_ += QString :: fromUtf8 (messages);
			}
#if OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT
			QTextStream (stdout) << "read from server: " << size << " bytes \n";
			QTextStream (stdout) << "------------------\n";
			QTextStream (stdout) << "data: " << data_.size() <<  " bytes \n";
			QTextStream (stdout) << data_ << "\n";
			QTextStream (stdout) << "messages: " << messages_.size() << " bytes  \n";
			QTextStream (stdout) << messages_ << "\n";
			QTextStream (stdout) << "------------------\n";
#endif
			showServerMessages (messages_);
		}
		isRunning_ = false;
	}
}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_CLIENT_CPP_ */

