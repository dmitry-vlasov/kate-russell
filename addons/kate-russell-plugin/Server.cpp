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

#ifndef PLUGIN_KATE_RUSSELL_MDL_SERVER_CPP_
#define PLUGIN_KATE_RUSSELL_MDL_SERVER_CPP_

#include <QByteArray>

#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace mdl {

	/****************************
	 *	Public members
	 ****************************/

	Server :: Server (View* view, const Config* config):
	view_ (view),
	config_ (config),
	process_ (new KProcess()),
	isRun_ (false) {
	}
	Server ::  ~ Server() {
		if (process_ != NULL) {
			delete process_;
		}
	}

	bool
	Server :: start()
	{
		if (lookForRunning()) {
			return true;
		}
		QString directory (config_->getSourceRoot());
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
	}
	bool
	Server :: stop()
	{
		isRun_ = false;
		if (process_->state() != QProcess :: NotRunning) {
			process_->terminate();
			return true;
		}
		return false;
	}
	KProcess*
	Server :: process() {
		return process_;
	}
	bool
	Server :: isRun() const {
		return isRun_;
	}

	/****************************
	 *	Public slots
	 ****************************/

	/****************************
	 *	Private members
	 ****************************/

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
}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_SERVER_CPP_ */

