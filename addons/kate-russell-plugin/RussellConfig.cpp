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

#include "RussellConfig.hpp"
#include "Connection.hpp"

namespace plugin {
namespace kate {
namespace russell {

const QString& default_host() { static QString host = QStringLiteral("localhost"); return host; }
int            default_port() { return 808011; }
const QString& default_invocation() { static QString invoc = QStringLiteral("mdl -d"); return invoc; }
bool           default_autostart() { return false; }

RussellConfig::RussellConfig(QWidget* par, Plugin *plug) : KTextEditor::ConfigPage(par), plugin(plug)
{
	 configUi.setupUi(this);
//    configUi.setupUi(this);
//    configUi.cmdEdit->setText(DEFAULT_CTAGS_CMD);
/*
    configUi.addButton->setToolTip(i18n("Add a directory to index."));
    configUi.addButton->setIcon(QIcon::fromTheme(QStringLiteral("list-add")));

    configUi.delButton->setToolTip(i18n("Remove a directory."));
    configUi.delButton->setIcon(QIcon::fromTheme(QStringLiteral("list-remove")));

    configUi.updateDB->setToolTip(i18n("(Re-)generate the common CTags database."));
    configUi.updateDB->setIcon(QIcon::fromTheme(QStringLiteral("view-refresh")));

    connect(configUi.updateDB,  SIGNAL(clicked()), this, SLOT(updateGlobalDB()));
    connect(configUi.addButton, SIGNAL(clicked()), this, SLOT(addGlobalTagTarget()));
    connect(configUi.delButton, SIGNAL(clicked()), this, SLOT(delGlobalTagTarget()));
*/
	connect(configUi.resetButton, SIGNAL(clicked()), this, SLOT(resetConfig()));
	connect(configUi.startButton, SIGNAL(clicked()), this, SLOT(startDaemon()));
	connect(configUi.stopButton, SIGNAL(clicked()), this, SLOT(stopDaemon()));
	connect(configUi.checkButton, SIGNAL(clicked()), this, SLOT(checkDaemon()));
	connect(configUi.portEdit, SIGNAL(textEdited(QString)), this, SLOT(checkPort()));

/*    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this,    SLOT(updateDone(int,QProcess::ExitStatus)));
*/
    reset();
    if (configUi.autostartCheckBox->isChecked() && configUi.startButton->isEnabled()) {
    	startDaemon();
    }
}

QString RussellConfig::name() const {
    return i18n("Russell");
}

QString RussellConfig::fullName() const {
    return i18n("Russell Settings");
}

QIcon RussellConfig::icon() const {
    return QIcon::fromTheme(QStringLiteral("text-x-csrc"));
}

QString RussellConfig::host() {
	KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("Russell"));
	return config.hasKey(QStringLiteral("DaemonHost")) ? config.readEntry(QStringLiteral("DaemonHost")) : default_host();
}

int RussellConfig::port() {
	KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("Russell"));
	if (config.hasKey(QStringLiteral("DaemonPort"))) {
		QString port_str = config.readEntry(QStringLiteral("DaemonPort"));
		bool ok = true;
		int port = port_str.toInt(&ok);
		if (!ok) {
			KMessageBox::error(0, i18n("Port \"%1\" is not a decimal number", port_str));
		}
		return ok ? port : default_port();
	} else {
		return default_port();
	}
}

void RussellConfig::apply() {
    KConfigGroup config(KSharedConfig::openConfig(), QStringLiteral("Russell"));
    config.writeEntry("DaemonHost", configUi.hostEdit->text());
    config.writeEntry("DaemonPort", configUi.portEdit->text());
    config.writeEntry("DaemonInvocation", configUi.invocationEdit->text());
    config.writeEntry("DaemonAutostart", configUi.autostartCheckBox->isChecked());
    /*
    QString nr;
    for (int i=0; i<configUi.targetList->count(); i++) {
        nr = QStringLiteral("%1").arg(i,3);
        config.writeEntry(QStringLiteral("GlobalTarget_")+nr, configUi.targetList->item(i)->text());
    }*/
    config.sync();
}

void RussellConfig::reset() {
    KConfigGroup config(KSharedConfig::openConfig(), "Russell");
    configUi.hostEdit->setText(config.readEntry(QStringLiteral("DaemonHost"), default_host()));
    configUi.portEdit->setText(config.readEntry(QStringLiteral("DaemonPort"), QString::number(default_port())));
    configUi.invocationEdit->setText(config.readEntry(QStringLiteral("DaemonInvocation"), default_invocation()));
    configUi.autostartCheckBox->setChecked(config.readEntry(QStringLiteral("DaemonAutostart"), default_autostart()));

/*
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
*/
    config.sync();
    checkDaemon();
}

void RussellConfig::defaults() {
	configUi.hostEdit->setText(default_host());
	configUi.portEdit->setText(QString::number(default_port()));
	configUi.invocationEdit->setText(default_invocation());
	configUi.autostartCheckBox->setChecked(default_autostart());
	checkDaemon();
}

void RussellConfig::resetConfig() {
	configUi.hostEdit->setText(default_host());
	configUi.portEdit->setText(QString::number(default_port()));
	configUi.invocationEdit->setText(default_invocation());
	configUi.autostartCheckBox->setChecked(default_autostart());
	checkDaemon();
/*
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
*/
}

void RussellConfig::startDaemon() {
    //delete configUi.targetList->currentItem ();

	//QString command = QStringLiteral("%1 -f %2 %3").arg(configUi.cmdEdit->text()).arg(file).arg(targets) ;
	QString command = configUi.invocationEdit->text();
    process.start(command);

    if(!process.waitForStarted(100)) {
        KMessageBox::error(0, i18n("Failed to run \"%1\". exitStatus = %2", command, process.exitStatus()));
        return;
    }
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    sleep(1);
    QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
    checkDaemon();
}

void RussellConfig::stopDaemon() {
	mdl::Connection::mod().execute(QStringLiteral("exit"));
    /*for (int i=0; i<configUi.targetList->count(); i++) {
        if (configUi.targetList->item(i)->text() == target) {
            return true;
        }
    }
    return false;
    */
	//process.terminate();
	checkDaemon();
}

bool RussellConfig::checkDaemon() {
	bool ret = mdl::Connection::mod().execute(QStringLiteral("status"));
	configUi.aliveEdit->setText(ret ? mdl::Connection::get().messages() : QStringLiteral("is not running"));
	configUi.stopButton->setEnabled(ret);
	configUi.startButton->setEnabled(!ret);
	return ret;
/*
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
 */
}

void RussellConfig::checkPort(QString& port_str) {
	bool ok = true;
	port_str.toInt(&ok);
	if (!ok) {
		KMessageBox::error(0, i18n("Port \"%1\" is not a decimal number", port_str));
	}
}
/*
void RussellConfigPage::updateDone(int exitCode, QProcess::ExitStatus status) {
    if (status == QProcess::CrashExit) {
        KMessageBox::error(this, i18n("The CTags executable crashed."));
    }
    else if (exitCode != 0) {
        KMessageBox::error(this, i18n("The CTags command exited with code %1", exitCode));
    }
    
   // configUi.updateDB->setDisabled(false);
    QApplication::restoreOverrideCursor();
}
*/
}}}

