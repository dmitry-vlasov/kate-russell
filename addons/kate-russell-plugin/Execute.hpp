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

#include <QTcpSocket>
#include <QByteArray>
#include <QString>

namespace russell {

struct RussellConsole {
	RussellConsole() : isBusy_(true) { }
	bool execute(const QString& command);
	bool isBusy() const { return isBusy_; }
private:
	bool isBusy_;
};
/*
class RussellClient : public QObject {
Q_OBJECT
public:
	RussellClient();
	bool success() const { return !code_; }
	bool connection();
	bool isBusy() const { return isBusy_; }

Q_SIGNALS:
	void dataReceived(quint32, QString, QString);

public Q_SLOTS:
	bool execute(const QString&);

private Q_SLOTS:
	void readyRead();

private:
	bool runCommand();
	bool readOutput();
	void makeOutput();

	QTcpSocket socket_;
	QByteArray buffer_;
	QString data_;
	QString messages_;
	quint32 code_;
	quint32 size_;
	bool    isBusy_;
	QString command_;
};
*/

class Russell : public QObject {
Q_OBJECT
public :
	enum Runner { CLIENT, CONSOLE };

	Russell();
	bool success() const { return !code_; }
	bool connection();
	bool isBusy() const { return isBusy_; }
	bool execute(const QString&);

Q_SIGNALS:
	void dataReceived(quint32, QString, QString);

private Q_SLOTS:
	void readyRead();

private:
	bool runCommand();
	bool readOutput();
	void makeOutput();

	QTcpSocket socket_;
	QByteArray buffer_;
	QString data_;
	QString messages_;
	quint32 code_;
	quint32 size_;
	bool    isBusy_;
	QString command_;
};

struct Metamath {
	Metamath() : isBusy_(true) { }
	bool execute(const QString& command);
	bool isBusy() const { return isBusy_; }
private:
	bool isBusy_;
};

class Execute {
public :
	static Russell& russell() { return mod().russell_; }
	static Metamath& metamath() { return mod().metamath_; }

private :
	static Execute& mod() { static Execute exec; return exec; }
	Execute() { }
	Russell  russell_;
	Metamath metamath_;
};


}
