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

#include <experimental/filesystem>
#include <map>

#include <QString>
#include <QObject>

#include "Kind.hpp"

namespace russell {

namespace efs = std::experimental::filesystem;

class View;

class Client : public QObject {
Q_OBJECT
public :
	Client (View*);
	
	bool execute(const QString&);
	bool read(const QString& file, ActionScope scope = ActionScope::FILE);
	void readFile();
	void checkFile();
	void writeFile();
	//void setupInFile();
	//void setupOutFile();
	//void setupPosition (const int line, const int column);
	//void setupIndex (const int index);
	void startProving();
	void expandNode (const long);

	bool prove (const bool clearOutput = true);
	bool translate (const QString& file, ActionScope scope = ActionScope::FILE, Lang target = Lang::SMM);
	bool verify    (const QString& file, ActionScope scope = ActionScope::FILE);
	bool verifyMm  (const QString& file, ActionScope scope = ActionScope::FILE);
	bool merge     (const QString& file, ActionScope scope = ActionScope::FILE);
	bool learn     (const bool clearOutput = true);
	bool lookupDefinition (const QString& file, const int line, const int column);
	bool lookupLocation   (const QString& file, const int line, const int column);
	bool mine (const QString& file, const QString& options);

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
	bool fileChanged(const QString& file) const;

	View* view_;
	std::map<QString, efs::file_time_type> timestamps;
};

}
