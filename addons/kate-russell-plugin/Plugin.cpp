/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Plugin.cpp                           */
/* Description:     a plugin for a Russell support plugin for Kate           */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#include <KPluginFactory>
#include <unistd.h>

#include "RussellConfigPage.hpp"
#include "Plugin.hpp"

#include "Execute.hpp"
#include "Launcher.hpp"
#include "View.hpp"

K_PLUGIN_FACTORY_WITH_JSON (KateRussellPluginFactory, "katerussellplugin.json", registerPlugin<russell::Plugin>();)

#include "Plugin.moc"

namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	Plugin :: Plugin (QObject *parent, const VariantList&) :
	KTextEditor::Plugin (parent) {
		#ifdef DEBUG_CREATE_DESTROY
		std :: cout << "Plugin :: Plugin (QObject *parent, const VariantList&):" << std :: endl;
		#endif
		//KGlobal :: locale()->insertCatalog ("kate-russell-plugin");

		if (RussellConfig::russellAutostart() && !RussellClient::connection()) {
			Launcher::russellClient().start();
		}
		if (RussellConfig::russellConsoleAutostart()) {
			Launcher::russellConsole().start();
		}
		if (RussellConfig::metamathAutostart()) {
			Launcher::metamath().start();
		}
		//usleep(100);
	}
	Plugin :: ~ Plugin() {
		#ifdef DEBUG_CREATE_DESTROY
		std :: cout << "Plugin :: ~ Plugin()" << std :: endl;
		#endif
	}
	
	QObject*
	Plugin :: createView (KTextEditor::MainWindow* mainWindow) {
		return new View (mainWindow, this);
	}

	KTextEditor::ConfigPage* Plugin::configPage(int number, QWidget *parent) {
		if (number != 0) return nullptr;
		return new RussellConfigPage(parent, this);
	}
}
