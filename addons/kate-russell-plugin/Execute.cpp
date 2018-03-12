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
#include "Server.hpp"
#include "Execute.hpp"
#include "View.hpp"

#define OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT true

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	bool
	Metamath::execute (const QString& command) {
		QByteArray data = command.toLatin1();
		qint64 size = Server::metamath().process().write(data);
		View::get()->getBottomUi().metamathTextEdit->appendPlainText(command);
		return size == data.size();
	}

	Russell :: Russell():
	code_(0),
	size_(0),
	isBusy_(false) {
		connect(&socket_, SIGNAL(readyRead()), this, SLOT(readyRead()));
	}


	bool
	Russell :: connection()
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
	Russell :: execute (const QString& command)
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
	Russell::readyRead()
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
	Russell :: runCommand()
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
	Russell :: makeOutput()
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
		if (command_ == QStringLiteral("exit")) {
			socket_.disconnectFromHost();
		}
		isBusy_ = false;
	}
}
