#include <unistd.h>

#include "RussellConfigPage.hpp"

#include <QFileInfo>
#include <QFileDialog>
#include <QCheckBox>

#include <KConfigGroup>
#include <KSharedConfig>

#include <kactioncollection.h>
#include <kstringhandler.h>
#include <kmessagebox.h>
#include <ktexteditor/editor.h>
//#include <klocalizedstring.h>

#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kaboutdata.h>

#include "Execute.hpp"
#include "Server.hpp"

namespace russell {

const QString& default_russell_host() { static QString host = QStringLiteral("localhost"); return host; }
int            default_russell_port() { return 808011; }
const QString& default_russell_invocation() { static QString invoc = QStringLiteral("mdl -d"); return invoc; }
bool           default_russell_autostart() { return false; }
const QString& default_metamath_invocation() { static QString invoc = QStringLiteral("metamath"); return invoc; }
bool           default_metamath_autostart() { return false; }
inline QString to_string(bool v) { return v ? QStringLiteral("true") : QStringLiteral("false"); }
inline bool    to_bool(const QString& s) { return s == QStringLiteral("true"); }

RussellConfig::RussellConfig() : config(KSharedConfig::openConfig(), QStringLiteral("Russell")) {
}

QString RussellConfig::russellHost() {
	return instance().config.hasKey(QStringLiteral("RussellHost")) ?
		instance().config.readEntry(QStringLiteral("RussellHost")) :
		default_russell_host();
}

int RussellConfig::russlelPort() {
	if (instance().config.hasKey(QStringLiteral("RussellPort"))) {
		QString port_str = instance().config.readEntry(QStringLiteral("RussellPort"));
		bool ok = true;
		int port = port_str.toInt(&ok);
		if (!ok) {
			KMessageBox::error(0, i18n("Port \"%1\" is not a decimal number", port_str));
		}
		return ok ? port : default_russell_port();
	} else {
		return default_russell_port();
	}
}

QString RussellConfig::russellInvocation() {
	return instance().config.hasKey(QStringLiteral("RussellInvocation")) ?
		instance().config.readEntry(QStringLiteral("RussellInvocation")) :
		default_russell_invocation();
}

bool RussellConfig::russellAutostart() {
	return instance().config.hasKey(QStringLiteral("RussellAutostart")) ?
		to_bool(instance().config.readEntry(QStringLiteral("RussellAutostart"))) :
		default_russell_autostart();
}

QString RussellConfig::metamathInvocation() {
	return instance().config.hasKey(QStringLiteral("MetamathInvocation")) ?
		instance().config.readEntry(QStringLiteral("MetamathInvocation")) :
		default_russell_invocation();
}

bool RussellConfig::metamathAutostart() {
	return instance().config.hasKey(QStringLiteral("MetamathAutostart")) ?
		to_bool(instance().config.readEntry(QStringLiteral("MetamathAutostart"))) :
		default_russell_autostart();
}

RussellConfigPage::RussellConfigPage(QWidget* par, Plugin *plug) : KTextEditor::ConfigPage(par), plugin_(plug)
{
	 configUi_.setupUi(this);
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
	connect(configUi_.russellResetButton, SIGNAL(clicked()), this, SLOT(resetRussellConfigSlot()));
	connect(configUi_.russellStartButton, SIGNAL(clicked()), this, SLOT(startRussellSlot()));
	connect(configUi_.russellStopButton, SIGNAL(clicked()), this, SLOT(stopRussellSlot()));
	connect(configUi_.russellKillButton, SIGNAL(clicked()), this, SLOT(killRussellSlot()));
	connect(configUi_.russellCheckButton, SIGNAL(clicked()), this, SLOT(checkRussellSlot()));
	connect(configUi_.russellPortEdit, SIGNAL(textEdited(QString)), this, SLOT(checkPortSlot(QString)));
	connect(&Server::russell().process(), SIGNAL(started()), this, SLOT(startedRussellSlot()));
	connect(
		&Server::russell().process(),
		SIGNAL(finished(int, QProcess::ExitStatus)),
		this,
		SLOT(finishedRussellSlot(int, QProcess::ExitStatus))
	);

	connect(configUi_.metamathStartButton, SIGNAL(clicked()), this, SLOT(startMetamathSlot()));
	connect(configUi_.metamathStopButton, SIGNAL(clicked()), this, SLOT(stopMetamathSlot()));
	connect(configUi_.metamathKillButton, SIGNAL(clicked()), this, SLOT(killMetamathSlot()));
	connect(&Server::metamath().process(), SIGNAL(started()), this, SLOT(startedMetamathSlot()));
	connect(
		&Server::metamath().process(),
		SIGNAL(finished(int, QProcess::ExitStatus)),
		this,
		SLOT(finishedMetamathSlot(int, QProcess::ExitStatus))
	);

/*    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this,    SLOT(updateDone(int,QProcess::ExitStatus)));
*/
    reset();

    bool run = Server::russell().isRunning();
	configUi_.russellAliveEdit->setText(run ? QStringLiteral("running") : QStringLiteral("stopped"));
	configUi_.russellStopButton->setEnabled(run);
	configUi_.russellKillButton->setEnabled(run);
	configUi_.russellStartButton->setEnabled(!run);

	run = Server::metamath().isRunning();
	configUi_.metamathAliveEdit->setText(run ? QStringLiteral("running") : QStringLiteral("stopped"));
	configUi_.metamathStopButton->setEnabled(run);
	configUi_.metamathKillButton->setEnabled(run);
	configUi_.metamathStartButton->setEnabled(!run);
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
    config.writeEntry("RussellHost", configUi_.russellHostEdit->text());
    config.writeEntry("RussellPort", configUi_.russellPortEdit->text());
    config.writeEntry("RussellInvocation", configUi_.russellInvocationEdit->text());
    config.writeEntry("RussellAutostart", configUi_.russellAutostartCheckBox->isChecked() ? "true" : "false");
    config.writeEntry("MetamathInvocation", configUi_.metamathInvocationEdit->text());
    config.writeEntry("MetamathAutostart", configUi_.metamathAutostartCheckBox->isChecked() ? "true" : "false");
    /*
    QString nr;
    for (int i=0; i<configUi.targetList->count(); i++) {
        nr = QStringLiteral("%1").arg(i,3);
        config.writeEntry(QStringLiteral("GlobalTarget_")+nr, configUi.targetList->item(i)->text());
    }*/
    config.sync();
}

void RussellConfigPage::reset() {
    KConfigGroup config(KSharedConfig::openConfig(), "Russell");
    configUi_.russellHostEdit->setText(config.readEntry(QStringLiteral("RussellHost"), default_russell_host()));
    configUi_.russellPortEdit->setText(config.readEntry(QStringLiteral("RussellPort"), QString::number(default_russell_port())));
    configUi_.russellInvocationEdit->setText(config.readEntry(QStringLiteral("RussellInvocation"), default_russell_invocation()));
    configUi_.russellAutostartCheckBox->setChecked(to_bool(config.readEntry(QStringLiteral("RussellAutostart"), to_string(default_russell_autostart()))));
    configUi_.metamathInvocationEdit->setText(config.readEntry(QStringLiteral("MetamathInvocation"), default_metamath_invocation()));
    configUi_.metamathAutostartCheckBox->setChecked(to_bool(config.readEntry(QStringLiteral("MetamathAutostart"), to_string(default_metamath_autostart()))));

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
    checkRussellSlot();
}

void RussellConfigPage::defaults() {
	configUi_.russellHostEdit->setText(default_russell_host());
	configUi_.russellPortEdit->setText(QString::number(default_russell_port()));
	configUi_.russellInvocationEdit->setText(default_russell_invocation());
	configUi_.russellAutostartCheckBox->setChecked(default_russell_autostart());
	configUi_.metamathInvocationEdit->setText(default_metamath_invocation());
	configUi_.metamathAutostartCheckBox->setChecked(default_metamath_autostart());
	checkRussellSlot();
}

void RussellConfigPage::resetRussellConfigSlot() {
	configUi_.russellHostEdit->setText(default_russell_host());
	configUi_.russellPortEdit->setText(QString::number(default_russell_port()));
	configUi_.russellInvocationEdit->setText(default_russell_invocation());
	configUi_.russellAutostartCheckBox->setChecked(default_russell_autostart());
	checkRussellSlot();
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

void RussellConfigPage::startRussellSlot() {
    //delete configUi.targetList->currentItem ();

	//QString command = QStringLiteral("%1 -f %2 %3").arg(configUi.cmdEdit->text()).arg(file).arg(targets) ;
/*
	QString command = configUi_.russellinvocationEdit->text();
    process.start(command);

    if(!process.waitForStarted(100)) {
        KMessageBox::error(0, i18n("Failed to run \"%1\". exitStatus = %2", command, process.exitStatus()));
        return;
    }
*/
	if (!Server::russell().isRunning()) {
		Server::russell().start();
		//QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		//usleep(500);
		//QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
		checkRussellSlot();
	}
}

void RussellConfigPage::stopRussellSlot() {
	if (Server::russell().isRunning()) {
		Execute::russell().execute(QStringLiteral("exit"));
		/*for (int i=0; i<configUi.targetList->count(); i++) {
			if (configUi.targetList->item(i)->text() == target) {
				return true;
			}
		}
		return false;
		*/
		//process.terminate();
		checkRussellSlot();
	}
}

void RussellConfigPage::killRussellSlot() {
	if (Server::russell().isRunning()) {
		Server::russell().stop();
	}
}

bool RussellConfigPage::checkRussellSlot() {
	bool ret = Execute::russell().connection();
	configUi_.russellAliveEdit->setText(ret ? QStringLiteral("running") : QStringLiteral("is not running"));
	configUi_.russellStopButton->setEnabled(ret);
	configUi_.russellStartButton->setEnabled(!ret);
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

void RussellConfigPage::startedRussellSlot() {
	configUi_.russellAliveEdit->setText(i18n("running"));
	configUi_.russellStopButton->setEnabled(true);
	configUi_.russellKillButton->setEnabled(true);
	configUi_.russellStartButton->setEnabled(false);
}

void RussellConfigPage::finishedRussellSlot(int exitCode, QProcess::ExitStatus exitStatus) {
	QString text = i18n("stopped");
	if (exitCode || exitStatus != QProcess::NormalExit) {
		text += i18n(" error code: ") + QString::number(exitCode);
	}
	configUi_.russellAliveEdit->setText(text);
	configUi_.russellStopButton->setEnabled(false);
	configUi_.russellKillButton->setEnabled(false);
	configUi_.russellStartButton->setEnabled(true);
}


void RussellConfigPage::startMetamathSlot() {
	if (!Server::metamath().isRunning()) {
		Server::metamath().start();
		//QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		//usleep(500);
		//QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	}
}

void RussellConfigPage::stopMetamathSlot() {
	if (Server::metamath().isRunning()) {
		Execute::metamath().execute(QStringLiteral("exit\n"));
	}
}

void RussellConfigPage::killMetamathSlot() {
	if (Server::metamath().isRunning()) {
		Server::metamath().stop();
	}
}


void RussellConfigPage::startedMetamathSlot() {
	configUi_.metamathAliveEdit->setText(i18n("running"));
	configUi_.metamathStopButton->setEnabled(true);
	configUi_.metamathKillButton->setEnabled(true);
	configUi_.metamathStartButton->setEnabled(false);
}

void RussellConfigPage::finishedMetamathSlot(int exitCode, QProcess::ExitStatus exitStatus) {
	QString text = i18n("stopped");
	if (exitCode || exitStatus != QProcess::NormalExit) {
		text += i18n(" error code: ") + QString::number(exitCode);
	}
	configUi_.metamathAliveEdit->setText(text);
	configUi_.metamathStopButton->setEnabled(false);
	configUi_.metamathKillButton->setEnabled(false);
	configUi_.metamathStartButton->setEnabled(true);
}



void RussellConfigPage::checkPortSlot(QString port_str) {
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
}
