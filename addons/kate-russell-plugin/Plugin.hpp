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

#pragma once

//#include <KTextEditor/View>
#include <KTextEditor/Plugin>
//#include <KTextEditor/Application>
#include <KTextEditor/MainWindow>

//#include <KPluginFactory>

namespace russell {

typedef QList<QVariant> VariantList;

class Plugin : public KTextEditor::Plugin {
Q_OBJECT
public:
	Plugin (QObject* parent = 0, const VariantList& = VariantList());
	virtual ~ Plugin();

	QObject *createView (KTextEditor::MainWindow *mainWindow) Q_DECL_OVERRIDE;

	int configPages() const Q_DECL_OVERRIDE { return 1; };

	KTextEditor::ConfigPage* configPage (int number = 0, QWidget *parent = nullptr) Q_DECL_OVERRIDE;
};

}
