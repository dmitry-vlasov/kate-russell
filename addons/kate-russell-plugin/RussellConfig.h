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

#include <ktexteditor/view.h>
#include <ktexteditor/document.h>

#include <ktexteditor/application.h>
#include <ktexteditor/mainwindow.h>
#include <KTextEditor/ConfigPage>
#include <KTextEditor/Plugin>

#include "x_include.hpp"
#include "ui_RussellConfig.h"

namespace plugin {
namespace kate {
namespace russell {

class Plugin;

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
    void defaults() Q_DECL_OVERRIDE {}

private Q_SLOTS:
    void addGlobalTagTarget();
    void delGlobalTagTarget();
    void updateGlobalDB();
    void updateDone(int exitCode, QProcess::ExitStatus status);

private:

    bool listContains(const QString &target);

    QProcess         process;
    Plugin*          plugin;
    Ui_RussellConfig configUi;
};

}}}


