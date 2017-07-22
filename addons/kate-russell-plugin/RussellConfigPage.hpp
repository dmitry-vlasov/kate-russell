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
#include <QString>

#include <KConfigGroup>

#include <KTextEditor/ConfigPage>
#include <KTextEditor/Plugin>

#include "ui_RussellConfig.h"

namespace russell {

class Plugin;

class RussellConfig {
public:
	static QString russellHost();
    static int     russlelPort();
    static QString russellInvocation();
    static bool    russellAutostart();
    static QString metamathInvocation();
    static bool    metamathAutostart();
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
    ~RussellConfigPage() { apply(); }

    QString name() const Q_DECL_OVERRIDE;
    QString fullName() const Q_DECL_OVERRIDE;
    QIcon icon() const Q_DECL_OVERRIDE;
    
    void apply() Q_DECL_OVERRIDE;
    void reset() Q_DECL_OVERRIDE;
    void defaults() Q_DECL_OVERRIDE;

private Q_SLOTS:
    void resetRussellConfigSlot();
    void startRussellSlot();
    void stopRussellSlot();
    void killRussellSlot();
    bool checkRussellSlot();
    void startedRussellSlot();
    void finishedRussellSlot(int exitCode, QProcess::ExitStatus exitStatus);

    void startMetamathSlot();
    void stopMetamathSlot();
    void killMetamathSlot();
    void startedMetamathSlot();
    void finishedMetamathSlot(int exitCode, QProcess::ExitStatus exitStatus);

    void checkPortSlot(QString);

private:

    bool listContains(const QString &target);

    Plugin* plugin_;
    Ui::RussellConfig configUi_;
};

}


