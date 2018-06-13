/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       Execute.hpp                                              */
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

#include <variant>
#include <memory>

#include <QTcpSocket>
#include <QByteArray>
#include <QString>

namespace russell {

class RussellClient : public QObject {
Q_OBJECT
public:
	RussellClient(const QString& command);
	static bool connection();
	bool execute();

Q_SIGNALS:
	void dataReceived(quint32, QString, QString);

private Q_SLOTS:
	void readyRead();
private:
	bool runCommand();
	void makeOutput();

	QTcpSocket socket_;
	QByteArray buffer_;
	QString data_;
	QString messages_;
	quint32 code_;
	quint32 size_;
	QString command_;
};

class RussellConsole : public QObject {
Q_OBJECT
public:
	RussellConsole(const QString& command);
	bool execute ();

Q_SIGNALS:
	void dataReceived(quint32, QString, QString);

private Q_SLOTS:
	void readyReadOutput();
	void readyReadError();

private:
	void makeOutput();

	quint32 code_;
	QString command_;

	QString buffer_;
	QString data_;
	QString messages_;
};


class MetamathConsole : public QObject {
Q_OBJECT
public:
	MetamathConsole(const QString& command);
	bool execute();

Q_SIGNALS:
	void dataReceived(quint32, QString, QString);

private Q_SLOTS:
	void readyReadOutput();
	void readyReadError();

private:
	QString command_;
	QString buffer_;
};

class Execute : public QObject {
Q_OBJECT
public :
	static void exec(const QStringList& commands) {
		mod().execCommands(commands);
	}
	static Execute& mod() { static Execute exec; return exec; }

Q_SIGNALS:
	void dataReceived(QString, quint32, QString, QString);

private Q_SLOTS:
	void slotDataReceived(quint32, QString, QString);

private :
	Execute() { }

	typedef std::variant<
		std::unique_ptr<RussellClient>,
		std::unique_ptr<RussellConsole>,
		std::unique_ptr<MetamathConsole>
	> Executor;

	void execCommand(const QString& command);
	void execCommands(const QStringList& commands);

	Executor    executor;
	QStringList commandQueue;
	QString     commandCurrent;
};

}
