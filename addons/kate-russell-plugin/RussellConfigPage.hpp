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

#include <QString>

#include <KConfigGroup>

#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KTextEditor/Application>
#include <KTextEditor/MainWindow>
#include <KTextEditor/ConfigPage>
#include <KTextEditor/Plugin>

#include "ui_RussellConfig.h"

namespace plugin {
namespace kate {
namespace russell {

class Plugin;

class RussellConfig {
public:
	static QString host();
    static int     port();
    static QString daemon_invocation();
    static bool    daemon_autostart();
    static bool    daemon_running();
private:
    RussellConfig();
    static RussellConfig& instance() {
    	static RussellConfig config;
    	return config;
    }
    KConfigGroup config;
};

class RussellConfigPage : public KTextEditor::ConfigPage {
    Q_OBJECT
public:
    explicit RussellConfigPage(QWidget* parent = nullptr, Plugin *plugin = nullptr);
    ~RussellConfigPage() {}

    QString name() const Q_DECL_OVERRIDE;
    QString fullName() const Q_DECL_OVERRIDE;
    QIcon icon() const Q_DECL_OVERRIDE;
    
    void apply() Q_DECL_OVERRIDE;
    void reset() Q_DECL_OVERRIDE;
    void defaults() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void resetConfigSlot();
    void startDaemonSlot();
    void stopDaemonSlot();
    bool checkDaemonSlot();
    void checkPortSlot(QString&);

private:

    bool listContains(const QString &target);

    Plugin*          plugin_;
    Ui::RussellConfig configUi_;
};

}}}


