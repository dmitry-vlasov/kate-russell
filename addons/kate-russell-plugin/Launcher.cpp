/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Server.cpp                       */
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

#include "Launcher.hpp"

#include <QProcess>
#include <QByteArray>

#include <KMessageBox>

#include "Execute.hpp"
#include "RussellConfigPage.hpp"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	bool Process::isRunning() const {
		switch (kind_) {
		case RUSSELL_CLIENT:  return RussellClient::connection();
		case RUSSELL_CONSOLE: return process_.state() == QProcess::Running;
		case METAMATH:        return process_.state() == QProcess::Running;
		default: return false;
		}
	}

	bool Process::start() {
		if (isRunning()) return true;
		stop();
		//qDebug() << invocation_;
		process_.setProgram(invocation_);
		switch (kind_) {
			case RUSSELL_CLIENT:  process_.setArguments(QStringList() << QLatin1String("-d")); break;
			case RUSSELL_CONSOLE: process_.setArguments(QStringList() << QLatin1String("-c") << QLatin1String("--verb")); break;
			case METAMATH: break;
		}
		process_.start();
		return isRunning();
	}

	bool Process::stop() {
		if (process_.state() != QProcess :: NotRunning) {
			process_.terminate();
			if (!process_.waitForFinished(100)) {
				KMessageBox::error(0, i18n("Failed to terminate daemon, exitStatus = %1", process_.exitStatus()));
				process_.kill();
				return false;
			}
			return true;
		}
		return false;
	}
}
