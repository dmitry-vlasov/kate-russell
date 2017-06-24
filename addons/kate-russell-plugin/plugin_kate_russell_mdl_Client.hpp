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

#ifndef PLUGIN_KATE_RUSSELL_MDL_CLIENT_HPP_
#define PLUGIN_KATE_RUSSELL_MDL_CLIENT_HPP_

#include <QTcpSocket>
#include <QListWidgetItem>
#include <QString>
#include "plugin_kate_russell.dpp"

namespace plugin {
namespace kate {
namespace russell {
namespace mdl {

class Client : public QObject {
Q_OBJECT
public :
	Client (View*, const Config*);
	virtual ~ Client();
	
	void ready();
	void execute (const QString&);
	const QString& getData() const;
	const QString& getMessages() const;
	bool connect();
	void disconnect();

	void readFile();
	void checkFile();
	void writeFile();
	void setupInFile();
	void setupOutFile();
	void setupPosition (const int line, const int column);
	void setupIndex (const int index);
	void startProving();
	void expandNode (const long);

Q_SIGNALS:
	void showServerMessages (QString);

public Q_SLOTS:
	void executeCommand();
	void clearConsole();

private :
	void setupSlotsAndSignals();
	void runCommand (const QString&);
	void readOutput ();

	View*         view_;
	const Config* config_;
	QTcpSocket*   tcpSocket_;
	bool isConnected_;
	bool isRunning_;

	QString data_;
	QString messages_;

	QString host_;
	int     port_;
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_CLIENT_HPP_ */

