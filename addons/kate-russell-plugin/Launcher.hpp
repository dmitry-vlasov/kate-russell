/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Server.hpp                       */
/* Description:     an mdl server launcher                                   */
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

#include <QProcess>
#include "RussellConfigPage.hpp"

namespace russell {

class Process {
public :
	enum Kind { RUSSELL_CLIENT, RUSSELL_CONSOLE, METAMATH };
	Process(const QString& i, Kind k) : invocation_(i), kind_(k) { }
	~Process() { stop(); }
	bool start();
	bool stop();
	QProcess& process() { return process_; }
	bool isRunning() const;

private :
	QString  invocation_;
	Kind     kind_;
	QProcess process_;
};

struct Launcher {
public :
	static Process& russellClient() { return mod().russellClient_; }
	static Process& russellConsole() { return mod().russellConsole_; }
	static Process& metamath() { return mod().metamath_; }

private :
	static Launcher& mod() { static Launcher server; return server; }
	Launcher() :
		russellClient_(RussellConfig::russellInvocation(), Process::RUSSELL_CLIENT),
		russellConsole_(RussellConfig::russellConsoleInvocation(), Process::RUSSELL_CONSOLE),
		metamath_(RussellConfig::metamathInvocation(), Process::METAMATH) { }
	Process russellClient_;
	Process russellConsole_;
	Process metamath_;
};

}
