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

namespace plugin {
namespace kate {
namespace russell {

class View;

namespace mdl {

class Client : public QObject {
Q_OBJECT
public :
	Client (View*);
	
	bool execute(const QString&);
	void readFile();
	void checkFile();
	void writeFile();
	void setupInFile();
	void setupOutFile();
	void setupPosition (const int line, const int column);
	void setupIndex (const int index);
	void startProving();
	void expandNode (const long);

	bool prove (const bool clearOutput = true);
	bool translate (const bool clearOutput = true);
	bool verify    (const bool clearOutput = true);
	bool learn     (const bool clearOutput = true);
	bool lookupDefinition (const int line, const int column);
	bool lookupLocation   (const int line, const int column);
	bool mine (const QString&);

	bool prove (
		const int line,
		const int column,
		const bool clearOutput = true
	);

Q_SIGNALS:
	void showServerMessages (QString);

public Q_SLOTS:
	void executeCommand();
	void clearConsole();

private :
	void setupSlotsAndSignals();
	View* view_;
};

}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_CLIENT_HPP_ */

