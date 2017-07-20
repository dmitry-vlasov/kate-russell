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

#include <QProcess>
#include <QByteArray>

#include <KMessageBox>

#include "Server.hpp"

#include "Execute.hpp"
#include "RussellConfigPage.hpp"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	bool
	Process::isRunning() const {
		switch (kind_) {
		case RUSSELL:  return Execute::russell().connection();
		case METAMATH: return process_.state() == QProcess::Running;
		default: return false;
		}
	}

	bool
	Process :: start()
	{
		if (isRunning()) return true;
		stop();

		qDebug() << invocation_;
		QStringList com = invocation_.split(QLatin1Char(' '));
		process_.setProgram(com.first());
		com.removeFirst();
		process_.setArguments(com);
		process_.start();

		/*
		if (!process_.waitForStarted(300)) {
			KMessageBox::error(0, i18n("Failed to run \"%1\". exitStatus = %2", invocation_, process_.exitStatus()));
			process_.terminate();
			return false;
		}
		*/

		//qDebug() << process_;
		//qDebug() << process_.state();
		//qDebug() << process_.program();

		return true;
/*
		QString invocation  (config_->getSourceRoot());
		QString command (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --server ");
		command += QStringLiteral(" --port ");
		command += config_->getServerPort();

		// set working directory
		QUrl url (directory);
		//process_->setWorkingDirectory (url.toLocalFile (KUrl :: AddTrailingSlash));
		process_->setWorkingDirectory (url.toLocalFile());
		process_->setShellCommand (command);
		process_->setOutputChannelMode (KProcess :: SeparateChannels);

		KMessageBox::error (0, i18n ("about to run \"%1\"", command));

		process_->start();
		if(!process_->waitForStarted (500)) {
			KMessageBox::error (0, i18n ("Failed to run \"%1\". exitStatus = %2", command, process_->exitStatus()));
			return false;
		}
		isRun_ = true;
		return true;
*/
	}
	bool
	Process :: stop()
	{
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

	/****************************
	 *	Private members
	 ****************************/
/*
	bool
	Process :: lookForRunning()
	{
		//QString directory (config_->getSourceRoot());
		QString command (QStringLiteral("lsof -i :"));
		command += config_->getServerPort();
		KProcess process;
		process.setShellCommand (command);
		process.setOutputChannelMode (KProcess :: SeparateChannels);

		//KMessageBox::error (0, i18n ("about to run \"%1\"", command));

		process.start();
		if (!process.waitForStarted (500)) {
			KMessageBox::error (0, i18n ("Failed to run \"%1\". exitStatus = %2", command, process_->exitStatus()));
			return false;
		}
		if (!process.waitForFinished()) {
			KMessageBox::error (0, i18n ("Failed to run \"%1\". exitStatus = %2", command, process_->exitStatus()));
			return false;
		}
		QByteArray out = process.readAllStandardOutput();
		if (out.lastIndexOf ("mdl") > -1) {
			isRun_ = true;
			return true;
		}
		return false;
	}
*/
}
