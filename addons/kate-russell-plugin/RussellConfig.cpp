#include "RussellConfig.hpp"

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

#define DEFAULT_CTAGS_CMD QStringLiteral("AAAA")

namespace plugin {
namespace kate {
namespace russell {

RussellConfigPage::RussellConfigPage(QWidget* par, Plugin *plug) : KTextEditor::ConfigPage(par), plugin(plug)
{
    configUi.setupUi(this);
    configUi.cmdEdit->setText(DEFAULT_CTAGS_CMD);

    configUi.addButton->setToolTip(i18n("Add a directory to index."));
    configUi.addButton->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));

    configUi.delButton->setToolTip(i18n("Remove a directory."));
    configUi.delButton->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));

    configUi.updateDB->setToolTip(i18n("(Re-)generate the common CTags database."));
    configUi.updateDB->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));

    connect(configUi.updateDB,  SIGNAL(clicked()), this, SLOT(updateGlobalDB()));
    connect(configUi.addButton, SIGNAL(clicked()), this, SLOT(addGlobalTagTarget()));
    connect(configUi.delButton, SIGNAL(clicked()), this, SLOT(delGlobalTagTarget()));

    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this,    SLOT(updateDone(int,QProcess::ExitStatus)));
    
    reset();
}

QString RussellConfigPage::name() const {
    return i18n("Russell");
}

QString RussellConfigPage::fullName() const {
    return i18n("Russell Settings");
}

QIcon RussellConfigPage::icon() const {
    return QIcon::fromTheme(QStringLiteral("text-x-csrc"));
}

void RussellConfigPage::apply() {
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("Russell"));
    config.writeEntry("GlobalCommand", configUi.cmdEdit->text());

    config.writeEntry("GlobalNumTargets", configUi.targetList->count());
    
    QString nr;
    for (int i=0; i<configUi.targetList->count(); i++) {
        nr = QStringLiteral("%1").arg(i,3);
        config.writeEntry(QStringLiteral("GlobalTarget_")+nr, configUi.targetList->item(i)->text());
    }
    config.sync();
}

void RussellConfigPage::reset() {
    KConfigGroup config(KSharedConfig::openConfig(), "Russell");
    configUi.cmdEdit->setText(config.readEntry(QStringLiteral("GlobalCommand"), DEFAULT_CTAGS_CMD));

    int numEntries = config.readEntry(QStringLiteral("GlobalNumTargets"), 0);
    QString nr;
    QString target;
    for (int i=0; i<numEntries; i++) {
        nr = QStringLiteral("%1").arg(i,3);
        target = config.readEntry(QStringLiteral("GlobalTarget_")+nr, QString());
        if (!listContains(target)) {
            new QListWidgetItem(target, configUi.targetList);
        }
    }
    config.sync();
}

void RussellConfigPage::addGlobalTagTarget() {
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    //dialog.setMode(KFile::Directory | KFile::Files | KFile::ExistingOnly | KFile::LocalOnly);

    QString dir;
    if (configUi.targetList->currentItem()) {
        dir = configUi.targetList->currentItem()->text();
    }
    else if (configUi.targetList->item(0)) {
        dir = configUi.targetList->item(0)->text();
    }
    dialog.setDirectory(dir);

    // i18n("CTags Database Location"));
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QStringList urls = dialog.selectedFiles();

    for (int i=0; i<urls.size(); i++) {
        if (!listContains(urls[i])) {
            new QListWidgetItem(urls[i], configUi.targetList);
        }
    }

}

void RussellConfigPage::delGlobalTagTarget() {
    delete configUi.targetList->currentItem ();
}

bool RussellConfigPage::listContains(const QString &target) {
    for (int i=0; i<configUi.targetList->count(); i++) {
        if (configUi.targetList->item(i)->text() == target) {
            return true;
        }
    }
    return false;
}

void RussellConfigPage::updateGlobalDB() {
    if (process.state() != QProcess::NotRunning) {
        return;
    }

    QString targets;
    QString target;
    for (int i=0; i<configUi.targetList->count(); i++) {
        target = configUi.targetList->item(i)->text();
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

    QString command = QStringLiteral("%1 -f %2 %3").arg(configUi.cmdEdit->text()).arg(file).arg(targets) ;
    process.start(command);

    if(!process.waitForStarted(500)) {
        KMessageBox::error(0, i18n("Failed to run \"%1\". exitStatus = %2", command, process.exitStatus()));
        return;
    }
    configUi.updateDB->setDisabled(true);
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
}

void RussellConfigPage::updateDone(int exitCode, QProcess::ExitStatus status) {
    if (status == QProcess::CrashExit) {
        KMessageBox::error(this, i18n("The CTags executable crashed."));
    }
    else if (exitCode != 0) {
        KMessageBox::error(this, i18n("The CTags command exited with code %1", exitCode));
    }
    
    configUi.updateDB->setDisabled(false);
    QApplication::restoreOverrideCursor();
}

}}}

