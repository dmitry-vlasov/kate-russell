/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_mdl_Launcher.cpp                     */
/* Description:     a launcher class for a Russell support plugin for Kate   */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_MDL_LAUNCHER_CPP_
#define PLUGIN_KATE_RUSSELL_MDL_LAUNCHER_CPP_

#include "russell.hpp"

namespace plugin {
namespace kate {
namespace russell {
namespace mdl {

	/****************************
	 *	Public members
	 ****************************/

	Launcher :: Launcher (View* view, const Config* config):
	view_ (view),
	config_ (config),
	process_ (new KProcess()) {
	}
	Launcher ::  ~ Launcher() {
		delete process_;
	}

	bool 
	Launcher :: prove (const bool clearOutput)
	{
		if (!view_->currentIsRussell()) {
			return false;
		}
		QString command (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --prove ");
		command += config_->getProveOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		if (!checkLocal (config_->getSourceRoot())) {
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		command.replace (QStringLiteral("%f"), sourcePath);
		
		if (clearOutput) {
			view_->clearOutput();
		}
    	return startProcess (config_->getSourceRoot(), command);
	}
	bool 
	Launcher :: translate (const bool clearOutput)
	{
		if (!view_->currentIsRussell()) {
			return false;
		}
		QString command (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No translator is specified."));
			return false;
		}
		command += QStringLiteral(" -t ");
		command += config_->getTranslateOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		if (!checkLocal (config_->getTargetRoot())) {
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);
		QString targetPath (sourcePath);
		targetPath.chop (3);
		targetPath += QStringLiteral("smm");
		QString globalTargetPath (config_->getTargetRoot());
		globalTargetPath += QStringLiteral("/");
		globalTargetPath += targetPath;

		command.replace (QStringLiteral("%s"), sourcePath);
		command.replace (QStringLiteral("%t"), globalTargetPath);
		
		if (clearOutput) {
			view_->clearOutput();
		}
    	return startProcess (config_->getSourceRoot(), command);
	}
	bool
	Launcher :: verify (const bool clearOutput)
	{
		if (!view_->currentIsRussell() && !view_->currentIsMetamath()) {
			return false;
		}
		QString command (config_->getVerifier());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No verifyer is specified."));
			return false;
		}
		command += QStringLiteral(" ");
		command += config_->getVerifyOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		if (!checkLocal (config_->getTargetRoot())) {
			return false;
		}
		QString globalPath (url.toLocalFile());
		QString midPath
		(
			view_->currentIsMetamath() ?
			globalPath.mid (config_->getSourceRoot().size() + 1, -1) :
			globalPath.mid (config_->getTargetRoot().size() + 1, -1)
		);
		QString targetPath (midPath);
		targetPath.chop (3);
		targetPath += QStringLiteral("smm");

		command.replace (QStringLiteral("%f"), targetPath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return startProcess (config_->getTargetRoot(), command);
	}
	bool 
	Launcher :: learn (const bool clearOutput)
	{
		if (!view_->currentIsRussell()) {
			return false;
		}
		QString command (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --learn ");
		command += config_->getLearnOption();

		QUrl url (view_->currentFileUrl (true));
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		if (!checkLocal (config_->getSourceRoot())) {
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);
		QString universePath (config_->getSourceUniverse());

		command.replace (QStringLiteral("%f"), sourcePath);
		command.replace (QStringLiteral("%u"), universePath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return startProcess (config_->getSourceRoot(), command);
	}
	bool 
	Launcher :: lookupDefinition (const int line, const int column)
	{
		if (!view_->currentIsRussell()) {
			return false;
		}
		QString command (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No miner is specified."));
			return false; 
		}
		command += QStringLiteral(" --lookup-definition ");
		command += config_->getLookupOption();

		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		if (!checkLocal (config_->getSourceRoot())) {
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		command.replace (QStringLiteral("%l"), QString :: number (line + 1));
		command.replace (QStringLiteral("%c"), QString :: number (column + 1));
		command.replace (QStringLiteral("%f"), sourcePath);
		view_->clearOutput();

    	return startProcess (config_->getSourceRoot(), command);
	}
	bool 
	Launcher :: lookupLocation (const int line, const int column)
	{
		if (!view_->currentIsRussell()) {
			return false;
		}
		QString command (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --lookup-location ");
		command += config_->getLookupOption();

		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		if (!checkLocal (config_->getSourceRoot())) {
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);

		command.replace (QStringLiteral("%l"), QString :: number (line + 1));
		command.replace (QStringLiteral("%c"), QString :: number (column + 1));
		command.replace (QStringLiteral("%f"), sourcePath);
		view_->clearOutput();
    	return startProcess (config_->getSourceRoot(), command);
	}
	bool 
	Launcher :: mine (const QString& options)
	{
		if (!view_->currentIsRussell()) {
			return false;
		}
		QString command (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No miner is specified."));
			return false;
		}
		switch (view_->getState()) {
		case View :: MINING_OUTLINE : 
			command += QStringLiteral(" --mine-outline "); break;
		case View :: MINING_STRUCTURE :
			command += QStringLiteral(" --mine-structure "); break;
		case View :: MINING_TYPE_SYSTEM :
			command += QStringLiteral(" --mine-type-system "); break;
		default : return false;
		}
		command += options;

		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			return false;
		}
		if (!checkLocal (config_->getSourceRoot())) {
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);
		command += sourcePath;

		view_->clearOutput();

		//KMessageBox :: sorry (0, command);

    	return startProcess (config_->getSourceRoot(), command);
	}

	bool
	Launcher :: prove (const int line, const int column, const bool clearOutput)
	{
		if (!view_->currentIsRussell()) {
			return false;
		}
		QString command (config_->getProver());
		if (command.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("No prover is specified."));
			return false;
		}
		command += QStringLiteral(" --prove ");
		command += QStringLiteral(" --line ");
		command += QString :: number (line + 1);
		command += QStringLiteral(" --column ");
		command += QString :: number (column + 1);
		command += QStringLiteral(" ");
		command += config_->getProveOption();

		QUrl url (view_->currentFileUrl());
		if (url.isEmpty()) {
			KMessageBox :: sorry (0, i18n ("There's no active window."));
			return false;
		}
		if (!checkLocal (config_->getSourceRoot())) {
			return false;
		}
		QString globalSourcePath (url.toLocalFile());
		QString sourcePath = QStringLiteral("./");
		sourcePath += globalSourcePath.mid (config_->getSourceRoot().size() + 1, -1);
		command.replace (QStringLiteral("%f"), sourcePath);

		if (clearOutput) {
			view_->clearOutput();
		}
    	return startProcess (config_->getSourceRoot(), command);
	}

	bool
	Launcher :: stop()
	{
		if (process_->state() != QProcess :: NotRunning) {
			process_->terminate();
			return true;
		}
		return false;
	}

	KProcess* 
	Launcher :: process() {
		return process_;
	}

	bool
	Launcher :: checkLocal (const QString &path)
	{
		QUrl url (path);
		return checkLocal (url);
	}
	bool
	Launcher :: checkLocal (const QUrl &url)
	{
		if (url.path().isEmpty()) {
			KMessageBox :: sorry(0, i18n ("There is no file or directory specified for proving/translating/verifying."));
			return false;
		} else if (!url.isLocalFile()) {
			KMessageBox :: sorry (0, i18n
			(
				"The file \"%1\" is not a local file."
				"Non-local files cannot be processed.",
				url.path()
			));
			return false;
		}
		return true;
	}

	/****************************
	 *	Private members
	 ****************************/

	bool 
	Launcher :: startProcess
	(
		const QString& directory,
		const QString& command
	)
	{
		if (process_->state() != QProcess :: NotRunning) {
			return false;
		}
		// set working directory
		QUrl url (directory);
		//process_->setWorkingDirectory (url.toLocalFile (QUrl :: AddTrailingSlash));
		process_->setWorkingDirectory (url.toLocalFile());
		process_->setShellCommand (command);
		process_->setOutputChannelMode (KProcess :: SeparateChannels);

		KMessageBox::error (0, i18n ("about to run \"%1\"", command));

		process_->start();

		if(!process_->waitForStarted (500)) {
			KMessageBox::error (0, i18n ("Failed to run \"%1\". exitStatus = %2", command, process_->exitStatus()));
			return false;
		}
		QApplication :: setOverrideCursor (QCursor (Qt :: BusyCursor));
		return true;
	}
}
}
}
}

#endif /* PLUGIN_KATE_RUSSELL_MDL_LAUNCHER_CPP_ */

