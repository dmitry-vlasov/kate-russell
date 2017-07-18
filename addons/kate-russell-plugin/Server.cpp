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

#include <QByteArray>

#include <KMessageBox>

#include "Connection.hpp"
#include "Server.hpp"
#include "RussellConfigPage.hpp"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	bool
	Server :: start()
	{
		if (Connection::mod().connection() || mod().process_.state() != QProcess::NotRunning) {
			return true;
		}
		QString command = RussellConfig::daemon_invocation();
		mod().process_.setShellCommand (command);
		mod().process_.setOutputChannelMode (KProcess :: SeparateChannels);
		mod().process_.start();

		if(!mod().process_.waitForStarted(100)) {
			KMessageBox::error(0, i18n("Failed to run \"%1\". exitStatus = %2", command, mod().process_.exitStatus()));
			mod().process_.terminate();
			return false;
		}
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
	Server :: stop()
	{
		if (mod().process_.state() != QProcess :: NotRunning) {
			mod().process_.terminate();
			if (!mod().process_.waitForFinished(100)) {
				KMessageBox::error(0, i18n("Failed to terminate daemon, exitStatus = %1", mod().process_.exitStatus()));
				mod().process_.kill();
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
	Server :: lookForRunning()
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
