/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Client.hpp                       */
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
#include <QString>

namespace plugin {
namespace kate {
namespace russell {
namespace mdl {

class Connection : public QObject {
public :
	static const Connection& get() { return mod(); }
	static Connection& mod() { static Connection conn; return conn; }

	bool execute (const QString&);
	bool success() const { return !code_; }
	const QString& data() const { return data_; }
	const QString& messages() const { return messages_; }
	uint code() const { return code_; }
	bool established() { return execute(QStringLiteral("status")); }

private :
	Connection();
	virtual ~ Connection();

	bool connect();
	bool runCommand (const QString&);
	bool readOutput();

	QTcpSocket* tcpSocket_;
	QString data_;
	QString messages_;
	uint    code_;
};

}
}
}
}


