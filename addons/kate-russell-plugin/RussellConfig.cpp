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

#include <QFileInfo>
#include <QFileDialog>
#include <QCheckBox>

#include <KConfigGroup>
#include <KSharedConfig>

#include <kactioncollection.h>
#include <kstringhandler.h>
#include <kmessagebox.h>
#include <ktexteditor/editor.h>
#include <klocalizedstring.h>

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kaboutdata.h>
#include "RussellConfig.h"

K_PLUGIN_FACTORY_WITH_JSON (KateCTagsPluginFactory, "katectagsplugin.json", registerPlugin<KateCTagsPlugin>();)

/******************************************************************/
KateCTagsPlugin::KateCTagsPlugin(QObject* parent, const QList<QVariant>&):
KTextEditor::Plugin (parent), m_view(0)
{
    // FIXME KF5
    //KGlobal::locale()->insertCatalog("kate-ctags-plugin");
}

/******************************************************************/
QObject *KateCTagsPlugin::createView(KTextEditor::MainWindow *mainWindow)
{
    m_view = new KateCTagsView(this, mainWindow);
    return m_view;
}


/******************************************************************/
KTextEditor::ConfigPage *KateCTagsPlugin::configPage (int number, QWidget *parent)
{
  if (number != 0) return 0;
  return new KateCTagsConfigPage(parent, this);
}

/******************************************************************/
void KateCTagsPlugin::readConfig()
{
}




/******************************************************************/
KateCTagsConfigPage::KateCTagsConfigPage( QWidget* parent, KateCTagsPlugin *plugin )
: KTextEditor::ConfigPage( parent )
, m_plugin( plugin )
{
    m_confUi.setupUi(this);
    m_confUi.cmdEdit->setText(DEFAULT_CTAGS_CMD);

    m_confUi.addButton->setToolTip(i18n("Add a directory to index."));
    m_confUi.addButton->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));

    m_confUi.delButton->setToolTip(i18n("Remove a directory."));
    m_confUi.delButton->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));

    m_confUi.updateDB->setToolTip(i18n("(Re-)generate the common CTags database."));
    m_confUi.updateDB->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));

    connect(m_confUi.updateDB,  SIGNAL(clicked()), this, SLOT(updateGlobalDB()));
    connect(m_confUi.addButton, SIGNAL(clicked()), this, SLOT(addGlobalTagTarget()));
    connect(m_confUi.delButton, SIGNAL(clicked()), this, SLOT(delGlobalTagTarget()));

    connect(&m_proc, SIGNAL(finished(int,QProcess::ExitStatus)), 
            this,    SLOT(updateDone(int,QProcess::ExitStatus)));
    
    reset();
}

/******************************************************************/
QString KateCTagsConfigPage::name() const
{
    return i18n("CTags");
}

/******************************************************************/
QString KateCTagsConfigPage::fullName() const
{
    return i18n("CTags Settings");
}

/******************************************************************/
QIcon KateCTagsConfigPage::icon() const
{
    return QIcon::fromTheme(QStringLiteral("text-x-csrc"));
}

/******************************************************************/
void KateCTagsConfigPage::apply()
{
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("CTags"));
    config.writeEntry("GlobalCommand", m_confUi.cmdEdit->text());

    config.writeEntry("GlobalNumTargets", m_confUi.targetList->count());
    
    QString nr;
    for (int i=0; i<m_confUi.targetList->count(); i++) {
        nr = QStringLiteral("%1").arg(i,3);
        config.writeEntry(QStringLiteral("GlobalTarget_")+nr, m_confUi.targetList->item(i)->text());
    }
    config.sync();
}

/******************************************************************/
void KateCTagsConfigPage::reset()
{
    KConfigGroup config(KSharedConfig::openConfig(), "CTags");
    m_confUi.cmdEdit->setText(config.readEntry(QStringLiteral("GlobalCommand"), DEFAULT_CTAGS_CMD));

    int numEntries = config.readEntry(QStringLiteral("GlobalNumTargets"), 0);
    QString nr;
    QString target;
    for (int i=0; i<numEntries; i++) {
        nr = QStringLiteral("%1").arg(i,3);
        target = config.readEntry(QStringLiteral("GlobalTarget_")+nr, QString());
        if (!listContains(target)) {
            new QListWidgetItem(target, m_confUi.targetList);
        }
    }
    config.sync();
}


/******************************************************************/
void KateCTagsConfigPage::addGlobalTagTarget()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    //dialog.setMode(KFile::Directory | KFile::Files | KFile::ExistingOnly | KFile::LocalOnly);

    QString dir;
    if (m_confUi.targetList->currentItem()) {
        dir = m_confUi.targetList->currentItem()->text();
    }
    else if (m_confUi.targetList->item(0)) {
        dir = m_confUi.targetList->item(0)->text();
    }
    dialog.setDirectory(dir);

    // i18n("CTags Database Location"));
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QStringList urls = dialog.selectedFiles();

    for (int i=0; i<urls.size(); i++) {
        if (!listContains(urls[i])) {
            new QListWidgetItem(urls[i], m_confUi.targetList);
        }
    }

}


/******************************************************************/
void KateCTagsConfigPage::delGlobalTagTarget()
{
    delete m_confUi.targetList->currentItem ();
}


/******************************************************************/
bool KateCTagsConfigPage::listContains(const QString &target)
{
    for (int i=0; i<m_confUi.targetList->count(); i++) {
        if (m_confUi.targetList->item(i)->text() == target) {
            return true;
        }
    }
    return false;
}

/******************************************************************/
void KateCTagsConfigPage::updateGlobalDB()
{
    if (m_proc.state() != QProcess::NotRunning) {
        return;
    }

    QString targets;
    QString target;
    for (int i=0; i<m_confUi.targetList->count(); i++) {
        target = m_confUi.targetList->item(i)->text();
        if (target.endsWith(QLatin1Char('/')) || target.endsWith(QLatin1Char('\\'))) {
            target = target.left(target.size() - 1);
        }
        targets += target + QLatin1Char(' ');
    }

    QString file = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QLatin1String("/katectags");
    QDir().mkpath(file);
    file += QLatin1String("/common_db");

    if (targets.isEmpty()) {
        QFile::remove(file);
        return;
    }

    QString command = QStringLiteral("%1 -f %2 %3").arg(m_confUi.cmdEdit->text()).arg(file).arg(targets) ;
    m_proc.start(command);

    if(!m_proc.waitForStarted(500)) {
        KMessageBox::error(0, i18n("Failed to run \"%1\". exitStatus = %2", command, m_proc.exitStatus()));
        return;
    }
    m_confUi.updateDB->setDisabled(true);
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
}

/******************************************************************/
void KateCTagsConfigPage::updateDone(int exitCode, QProcess::ExitStatus status)
{
    if (status == QProcess::CrashExit) {
        KMessageBox::error(this, i18n("The CTags executable crashed."));
    }
    else if (exitCode != 0) {
        KMessageBox::error(this, i18n("The CTags command exited with code %1", exitCode));
    }
    
    m_confUi.updateDB->setDisabled(false);
    QApplication::restoreOverrideCursor();
}

#include "kate_ctags_plugin.moc"

