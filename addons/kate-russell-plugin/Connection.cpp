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
#include <cstring>

#include "russell.hpp"
#include "Connection.hpp"
#include "RussellConfig.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace mdl{

using std::string;

struct Return {
	Return(const string& t = "", bool s = true) : msg(t), code(s ? 0 : -1) { }
	Return(const string& t, const string& d, bool s = true) : msg(t), data(d), code(s ? 0 : -1) { }
	operator bool() const { return success(); }
	string msg;
	string data;
	uint   code;

	bool success() const { return !code; }

	string to_string() const {
		uint len = msg.size() + data.size() + sizeof(uint) * 3;
		char array[len];
		char* arr = array;

		*reinterpret_cast<uint*>(arr) = code;
		arr += sizeof(uint);

		*reinterpret_cast<uint*>(arr) = msg.size();
		arr += sizeof(uint);
		for (char c : msg) *arr++ = c;

		*reinterpret_cast<uint*>(arr) = data.size();
		arr += sizeof(uint);
		for (char c : data) *arr++ = c;

		return string(array, len);
	}
	static Return from_string(const string& str) {
		const char* array = str.c_str();
		Return ret;

		ret.code = *reinterpret_cast<const uint*>(array);
		array += sizeof(uint);

		uint msg_len = *reinterpret_cast<const uint*>(array);
		array += sizeof(uint);
		while (msg_len-- > 0) ret.msg += *array++;

		uint data_len = *reinterpret_cast<const uint*>(array);
		array += sizeof(uint);
		while (data_len-- > 0) ret.data += *array++;

		return ret;
	}
};

	/****************************
	 *	Public members
	 ****************************/

	Connection :: Connection():
	tcpSocket_ (new QTcpSocket()),
	isConnected_ (false),
	isRunning_ (false),
	data_ (),
	messages_ (),
	success_(true) {
	}
	Connection ::  ~ Connection() {
		delete tcpSocket_;
	}

	void
	Connection :: execute (const QString& command)
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

		data_.clear();
		messages_.clear();
		runCommand (command);
		readOutput();
		//disconnect();
#if OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT
		QTextStream (stdout) << "\n\n\n";
#endif
	}
	const QString&
	Connection :: data() const {
		return data_;
	}
	const QString&
	Connection :: messages() const {
		return messages_;
	}

	bool
	Connection :: connect()
	{
		if (isConnected_) {
			std :: cout << "already connected to server" << std :: endl;
			return true;
		}
		QString host = russell::RussellConfigPage::host();
		int port = russell::RussellConfigPage::port();
		tcpSocket_->connectToHost (host, port);
		isConnected_ = tcpSocket_->waitForConnected();
		if (!isConnected_) {
			std :: cout << "not connected to server:" << std :: endl;
			QTextStream (stdout) << "\t" << tcpSocket_->errorString() << "\n";
			QTextStream (stdout) << "\tat: " << host << ":" << port << "\n";
		}
		return isConnected_;
	}
	void
	Connection :: disconnect()
	{
		tcpSocket_->disconnectFromHost();
		isConnected_ = false;
	}

	/****************************
	 *	Private members
	 ****************************/

	void
	Connection :: runCommand (const QString& command)
	{
		uint msg_len = command.length() + sizeof(uint);
		char *asciiCommand = new char [msg_len];
		*((uint*)asciiCommand) = command.length();
		for (int i = 0; i < command.length(); ++ i) {
			const QChar qch = command[i];
			asciiCommand [i + sizeof(uint)] = qch.toLatin1();
		}
		tcpSocket_->write (asciiCommand, msg_len);
		if (tcpSocket_->waitForBytesWritten()) {
			isRunning_ = true;
		}
		delete[] asciiCommand;
	}
	void
	Connection :: readOutput()
	{
		if (tcpSocket_->waitForReadyRead (3000)) {
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
			std::string str(read.data() + sizeof(uint), read.size() - sizeof(uint));
			Return ret = Return::from_string(str);
			success_ = ret.success();
			data_ += QString :: fromUtf8(ret.data.c_str(), ret.data.size());
			messages_ += QString :: fromUtf8(ret.msg.c_str(), ret.msg.size());

#if OUTPUT_CLIENT_DEBUG_INFO_TO_STDOUT
			QTextStream (stdout) << "read from server: " << read.size() << " bytes \n";
			QTextStream (stdout) << "------------------\n";
			QTextStream (stdout) << "data: " << data_.size() <<  " bytes \n";
			if (data_.size()) QTextStream (stdout) << data_ << "\n";
			QTextStream (stdout) << "messages: " << messages_.size() << " bytes  \n";
			if (messages_.size()) QTextStream (stdout) << messages_ << "\n";
			QTextStream (stdout) << "------------------\n";
#endif
			//showServerMessages (messages_);
		}
		isRunning_ = false;
	}
}
}
}
}

