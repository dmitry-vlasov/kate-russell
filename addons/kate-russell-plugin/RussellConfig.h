#ifndef KATE_CTAGS_PLUGIN_H
#define KATE_CTAGS_PLUGIN_H
/* Description : Kate CTags plugin
 *
 * Copyright (C) 2008-2011 by Kare Sars <kare.sars@iki.fi>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <ktexteditor/view.h>
#include <ktexteditor/document.h>

#include <ktexteditor/application.h>
#include <ktexteditor/mainwindow.h>
#include <KTextEditor/ConfigPage>
#include <KTextEditor/Plugin>

#include "kate_ctags_view.h"
#include "ui_CTagsGlobalConfig.h"

//******************************************************************/
class KateCTagsPlugin : public KTextEditor::Plugin
{
    Q_OBJECT

    public:
        explicit KateCTagsPlugin(QObject* parent = 0, const QList<QVariant> & = QList<QVariant>());
        virtual ~KateCTagsPlugin() {}

        QObject *createView(KTextEditor::MainWindow *mainWindow) Q_DECL_OVERRIDE;
   
        int configPages() const Q_DECL_OVERRIDE { return 1; };
        KTextEditor::ConfigPage *configPage (int number = 0, QWidget *parent = 0) Q_DECL_OVERRIDE;
        void readConfig();
        
        KateCTagsView *m_view;
};

//******************************************************************/
class KateCTagsConfigPage : public KTextEditor::ConfigPage {
    Q_OBJECT
public:
    explicit KateCTagsConfigPage( QWidget* parent = 0, KateCTagsPlugin *plugin = 0 );
    ~KateCTagsConfigPage() {}

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

    QProcess              m_proc;
    KateCTagsPlugin      *m_plugin;
    Ui_CTagsGlobalConfig  m_confUi;
};

#endif

