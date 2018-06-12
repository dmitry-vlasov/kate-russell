/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Connection.cpp                       */
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

#include <KMessageBox>

#include "RussellConfigPage.hpp"
#include "Execute.hpp"

#include "Launcher.hpp"
#include "View.hpp"

#define OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT true

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	RussellClient::RussellClient():
	code_(0),
	size_(0),
	isBusy_(false) {
		connect(&socket_, SIGNAL(readyRead()), this, SLOT(readyRead()));
	}

	bool
	RussellClient::connection()
	{
		if (socket_.state() == QTcpSocket::SocketState::ConnectedState) {
			//std :: cout << "already connected to server" << std :: endl;
			return true;
		}
		socket_.disconnectFromHost();
		QString host = russell::RussellConfig::russellHost();
		int port = russell::RussellConfig::russlelPort();
		socket_.connectToHost (host, port);
		bool isConnected = socket_.waitForConnected(100);
		if (!isConnected) {
			//std :: cout << "not connected to server:" << std :: endl;
			QTextStream (stdout) << "\t" << socket_.errorString() << "\n";
			QTextStream (stdout) << "\tat: " << host << ":" << port << "\n";
		}
		return isConnected;
	}

	/****************************
	 *	Public slots
	 ****************************/

	bool
	RussellClient::execute (const QString& command)
	{
		if (!connection() || isBusy_) {
			return false;
		}
		command_ = command;
		isBusy_ = true;
#if OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT
		QTextStream (stdout) << "========================================\n";
		QTextStream (stdout) << " about to execute:\n";
		QTextStream (stdout) << " \t" << command_ << "\n";
		QTextStream (stdout) << "========================================\n\n";
#endif
		data_.clear();
		messages_.clear();
		code_ = 1;
		if (!runCommand()) {
			isBusy_ = false;
			return false;
		}
#if OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT
		QTextStream (stdout) << "\n\n\n";
#endif
		return !code_;
	}

	/****************************
	 *	Private slots
	 ****************************/

	void
	RussellClient::readyRead()
	{
		while (socket_.bytesAvailable() > 0) {
			buffer_.append(socket_.readAll());
			// While can process data, process it
			while ((size_ == 0 && buffer_.size() >= 4) || (size_ > 0 && buffer_.size() >= size_)) {
				//if size of data has received completely, then store it on our global variable
				if (size_ == 0 && buffer_.size() >= 4) {
					QDataStream data(&buffer_, QIODevice::ReadOnly);
					data.setByteOrder(QDataStream::LittleEndian);
					data >> size_;
					buffer_.remove(0, 4);
				}
				// If data has received completely, then emit our SIGNAL with the data
				if (size_ > 0 && buffer_.size() >= size_) {
					makeOutput();
					size_ = 0;
					buffer_.clear();
					emit dataReceived(code_, messages_, data_);
				}
			}
		}
	}

	/****************************
	 *	Private members
	 ****************************/

	bool
	RussellClient::runCommand()
	{
		uint msg_len = command_.length() + sizeof(uint);
		char *asciiCommand = new char [msg_len];
		*((uint*)asciiCommand) = command_.length();
		for (int i = 0; i < command_.length(); ++ i) {
			const QChar qch = command_[i];
			asciiCommand [i + sizeof(uint)] = qch.toLatin1();
		}
		socket_.write (asciiCommand, msg_len);
		bool written = socket_.waitForBytesWritten(100);
		delete[] asciiCommand;
		return written;
	}
	void
	RussellClient::makeOutput()
	{
		QDataStream data(&buffer_, QIODevice::ReadOnly);
		data.setByteOrder(QDataStream::LittleEndian);
		data >> code_;
		quint32 len = 0;
		quint8  c = 0;

		data >> len;
		char msg[len];
		for (int i = 0; i < len; ++ i) {
			data >> c;
			msg[i] = c;
		}
		messages_ = QString::fromUtf8(msg, len);

		data >> len;
		char dat[len];
		for (int i = 0; i < len; ++ i) data >> (qint8&)dat[i];
		data_ = QString::fromUtf8(dat, len);

#if OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT
		QTextStream (stdout) << "read from server: " << size_ << " bytes \n";
		QTextStream (stdout) << "------------------\n";
		QTextStream (stdout) << "data: " << data_.size() <<  " bytes \n";
		if (data_.size()) QTextStream (stdout) << data_ << "\n";
		QTextStream (stdout) << "messages: " << messages_.size() << " bytes  \n";
		if (messages_.size()) QTextStream (stdout) << messages_ << "\n";
		QTextStream (stdout) << "------------------\n";
#endif
		//showServerMessages (messages_);
		if (command_ == QLatin1String("exit")) {
			socket_.disconnectFromHost();
		}
		isBusy_ = false;
	}


	Russell::Russell() {
		connect(&client, SIGNAL(dataReceived(quint32, QString, QString)), this, SLOT(slotDataReceived(quint32, QString, QString)));
		connect(&console, SIGNAL(dataReceived(quint32, QString, QString)), this, SLOT(slotDataReceived(quint32, QString, QString)));
	}
	bool Russell::success() const {
		return RussellConfig::runner() == QLatin1String("CONSOLE") ? console.success() : client.success();
	}
	bool Russell::connection() {
		return RussellConfig::runner() == QLatin1String("CONSOLE") ? console.connection() : client.connection();
	}
	bool Russell::isBusy() const {
		return isBusy_;
	}
	bool Russell::execute(const QString& command) {
		isBusy_ = true;
		//QTextStream(stdout) << "ABOUT TO EXEC: " << command << "\n";
		return RussellConfig::runner() == QLatin1String("CONSOLE") ? console.execute(command) : client.execute(command);
	}
	bool Russell::execute(const QStringList& commands) {

		/*QTextStream(stdout) << "ENQUEING COMMANDS: \n";
		for (auto c : commands) {
			QTextStream(stdout) << "\t" << c << "\n";
		}
		QTextStream(stdout) << "\n";*/

		if (!commands.empty()) {
			commandQueue.append(commands);
			return isBusy_ ? true : execute(commandQueue.takeFirst());
		} else {
			return true;
		}
	}
	void Russell::slotDataReceived(quint32 code, QString msg, QString data) {
		emit dataReceived(code, msg, data);
		if (!commandQueue.empty()) {
			//QTextStream(stdout) << "ABOUT TO EXEC from queue: " << commandQueue.first() << "\n";
			execute(commandQueue.takeFirst());
		} else {
			isBusy_ = false;
		}
	}

	bool
	Metamath::execute (const QString& command) {
		QByteArray data = command.toLatin1();
		qint64 size = Launcher::metamath().process().write(data);
		View::get()->getBottomUi().metamathTextEdit->appendPlainText(command);
		return size == data.size();
	}

	RussellConsole::RussellConsole() : code_(0) {
		connect(&Launcher::russellConsole().process(), SIGNAL(readyReadStandardOutput()), this, SLOT(readyReadOutput()));
		connect(&Launcher::russellConsole().process(), SIGNAL(readyReadStandardError()), this, SLOT(readyReadError()));
	}

	bool
	RussellConsole::execute (const QString& command) {
		command_ = command.endsWith(QLatin1Char('\n')) ? command : command + QLatin1Char('\n');
		buffer_.clear();
		data_.clear();
		messages_.clear();
		code_ = 0;
		QByteArray data = command_.toLatin1();
		qint64 size = Launcher::russellConsole().process().write(data);
		return size == data.size();
	}

	void RussellConsole::readyReadOutput() {
		QString data = QString::fromUtf8(Launcher::russellConsole().process().readAllStandardOutput());
		buffer_.append(data);

		appendText(View::get()->getBottomUi().russellTextEdit, data);
		View::get()->getBottomUi().qtabwidget->setCurrentIndex(1);

		QStringList lines = data.split(QLatin1Char('\n'));
		if (lines.size() && lines.last().trimmed() == QLatin1String(">>")) {
			makeOutput();
			//QTextStream(stdout) << "DATA:\n" << data_ << "\n";
			data_ = data_.trimmed();
			if (data_.endsWith(QLatin1String(">>"))) {
				data_ = data_.mid(0, data_.length() - 2);
				data_ = data_.trimmed();
			}
			emit dataReceived(code_, messages_, data_);
		}
	}

	void RussellConsole::readyReadError() {
		QString err = QString::fromUtf8(Launcher::russellConsole().process().readAllStandardError());
		buffer_.append(err);

		appendText(View::get()->getBottomUi().russellTextEdit, err);
		View::get()->getBottomUi().qtabwidget->setCurrentIndex(1);

		QStringList lines = buffer_.split(QLatin1Char('\n'));
		if (lines.size() && lines.last() == QLatin1String("> ")) {
			messages_ = buffer_;
		}
	}

	void RussellConsole::makeOutput() {
		QStringList lines = buffer_.split(QLatin1Char('\n'));
		if (lines.size()) {
			if (lines.first().startsWith(QLatin1String("error: "))) {
				code_ = lines.first().mid(7).toInt();
				lines.removeFirst();
			}
			bool in_messages = true;
			while (!lines.empty()) {
				QString line = lines.takeFirst();

				//QTextStream(stdout) << "LINE: " << line << "\n";

				if (line == QLatin1String("***** DATA *****")) {
					in_messages = false;
				} else {
					if (in_messages) {
						messages_ += line + QLatin1Char('\n');
					} else {
						data_ += line + QLatin1Char('\n');
					}
				}
			}
		}
	}
}
