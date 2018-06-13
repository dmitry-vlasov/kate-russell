#include <unistd.h>

#include "RussellConfigPage.hpp"

#include <QFileInfo>
#include <QFileDialog>
#include <QCheckBox>

#include <KConfigGroup>
#include <KSharedConfig>

#include <KMessageBox>

#include "Execute.hpp"
#include "Launcher.hpp"

namespace russell {

const QString& default_russell_host() { static QString host = QLatin1String("localhost"); return host; }
int            default_russell_port() { return 808011; }
const QString& default_russell_invocation() { static QString invoc = QLatin1String("mdl"); return invoc; }
bool           default_russell_autostart() { return false; }
const QString& default_russell_console_invocation() { static QString s = QLatin1String("mdl"); return s; }
bool           default_russell_console_autostart() { return false; }
const QString& default_metamath_invocation() { static QString invoc = QLatin1String("metamath"); return invoc; }
bool           default_metamath_autostart() { return false; }
const QString& default_russell_runner() { static QString ret = QLatin1String("CONSOLE"); return ret; }

inline QString to_string(bool v) { return v ? QLatin1String("true") : QLatin1String("false"); }
inline bool    to_bool(const QString& s) { return s == QLatin1String("true"); }

RussellConfig::RussellConfig() : config(KSharedConfig::openConfig(), QLatin1String("Russell")) {
}

QString RussellConfig::russellHost() {
	return instance().config.hasKey(QLatin1String("RussellHost")) ?
		instance().config.readEntry(QLatin1String("RussellHost")) :
		default_russell_host();
}

int RussellConfig::russlelPort() {
	if (instance().config.hasKey(QLatin1String("RussellPort"))) {
		QString port_str = instance().config.readEntry(QLatin1String("RussellPort"));
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
	return instance().config.hasKey(QLatin1String("RussellInvocation")) ?
		instance().config.readEntry(QLatin1String("RussellInvocation")) :
		default_russell_invocation();
}

bool RussellConfig::russellAutostart() {
	return instance().config.hasKey(QLatin1String("RussellAutostart")) ?
		to_bool(instance().config.readEntry(QLatin1String("RussellAutostart"))) :
		default_russell_autostart();
}

QString RussellConfig::metamathInvocation() {
	return instance().config.hasKey(QLatin1String("MetamathInvocation")) ?
		instance().config.readEntry(QLatin1String("MetamathInvocation")) :
		default_russell_invocation();
}

bool RussellConfig::metamathAutostart() {
	return instance().config.hasKey(QLatin1String("MetamathAutostart")) ?
		to_bool(instance().config.readEntry(QLatin1String("MetamathAutostart"))) :
		default_russell_autostart();
}

QString RussellConfig::russellConsoleInvocation() {
	return instance().config.hasKey(QLatin1String("RussellConsoleInvocation")) ?
		instance().config.readEntry(QLatin1String("RussellConsoleInvocation")) :
		default_russell_console_invocation();
}

bool RussellConfig::russellConsoleAutostart() {
	return instance().config.hasKey(QLatin1String("RussellConsoleAutostart")) ?
		to_bool(instance().config.readEntry(QLatin1String("RussellConsoleAutostart"))) :
		default_russell_console_autostart();
}

QString RussellConfig::runner() {
	return instance().config.hasKey(QLatin1String("RussellRunner")) ?
		instance().config.readEntry(QLatin1String("RussellRunner")) :
		default_russell_runner();
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
	connect(configUi_.russellConsoleRadioButton, SIGNAL(clicked()), this, SLOT(slotEnableConsole()));
	connect(configUi_.russellDaemonRadioButton, SIGNAL(clicked()), this, SLOT(slotEnableDaemon()));

	connect(configUi_.russellResetButton, SIGNAL(clicked()), this, SLOT(resetRussellConfigSlot()));
	connect(configUi_.russellStartButton, SIGNAL(clicked()), this, SLOT(startRussellSlot()));
	connect(configUi_.russellStopButton, SIGNAL(clicked()), this, SLOT(stopRussellSlot()));
	connect(configUi_.russellKillButton, SIGNAL(clicked()), this, SLOT(killRussellSlot()));
	connect(configUi_.russellCheckButton, SIGNAL(clicked()), this, SLOT(checkRussellSlot()));
	connect(configUi_.russellPortEdit, SIGNAL(textEdited(QString)), this, SLOT(checkPortSlot(QString)));
	connect(&Launcher::russellClient().process(), SIGNAL(started()), this, SLOT(startedRussellSlot()));
	connect(
		&Launcher::russellClient().process(),
		SIGNAL(finished(int, QProcess::ExitStatus)),
		this,
		SLOT(finishedRussellSlot(int, QProcess::ExitStatus))
	);

	connect(configUi_.metamathLookupButton, SIGNAL(clicked()), this, SLOT(lookupMetamathSlot()));
	connect(configUi_.metamathStartButton, SIGNAL(clicked()), this, SLOT(startMetamathSlot()));
	connect(configUi_.metamathStopButton, SIGNAL(clicked()), this, SLOT(stopMetamathSlot()));
	connect(configUi_.metamathKillButton, SIGNAL(clicked()), this, SLOT(killMetamathSlot()));
	connect(configUi_.metamathCheckButton, SIGNAL(clicked()), this, SLOT(checkMetamathSlot()));
	connect(&Launcher::metamath().process(), SIGNAL(started()), this, SLOT(startedMetamathSlot()));
	connect(
		&Launcher::metamath().process(),
		SIGNAL(finished(int, QProcess::ExitStatus)),
		this,
		SLOT(finishedMetamathSlot(int, QProcess::ExitStatus))
	);

	connect(configUi_.russellConsoleLookupButton, SIGNAL(clicked()), this, SLOT(lookupRussellConsoleSlot()));
	connect(configUi_.russellConsoleStartButton, SIGNAL(clicked()), this, SLOT(startRussellConsoleSlot()));
	connect(configUi_.russellConsoleStopButton, SIGNAL(clicked()), this, SLOT(stopRussellConsoleSlot()));
	connect(configUi_.russellConsoleKillButton, SIGNAL(clicked()), this, SLOT(killRussellConsoleSlot()));
	connect(configUi_.russellConsoleCheckButton, SIGNAL(clicked()), this, SLOT(checkRussellConsoleSlot()));
	connect(&Launcher::russellConsole().process(), SIGNAL(started()), this, SLOT(startedRussellConsoleSlot()));
	connect(
		&Launcher::russellConsole().process(),
		SIGNAL(finished(int, QProcess::ExitStatus)),
		this,
		SLOT(finishedRussellConsoleSlot(int, QProcess::ExitStatus))
	);

    reset();

    bool run = Launcher::russellClient().isRunning();
	configUi_.russellAliveEdit->setText(run ? QLatin1String("running") : QLatin1String("stopped"));
	configUi_.russellStopButton->setEnabled(run);
	configUi_.russellKillButton->setEnabled(run);
	configUi_.russellStartButton->setEnabled(!run);

	run = Launcher::metamath().isRunning();
	configUi_.metamathAliveEdit->setText(run ? QLatin1String("running") : QLatin1String("stopped"));
	configUi_.metamathStopButton->setEnabled(run);
	configUi_.metamathKillButton->setEnabled(run);
	configUi_.metamathStartButton->setEnabled(!run);

	run = Launcher::russellConsole().isRunning();
	configUi_.russellConsoleAliveEdit->setText(run ? QLatin1String("running") : QLatin1String("stopped"));
	configUi_.russellConsoleStopButton->setEnabled(run);
	configUi_.russellConsoleKillButton->setEnabled(run);
	configUi_.russellConsoleStartButton->setEnabled(!run);
}

QString RussellConfigPage::name() const {
    return i18n("Russell");
}

QString RussellConfigPage::fullName() const {
    return i18n("Russell Settings");
}

QIcon RussellConfigPage::icon() const {
    return QIcon::fromTheme(QLatin1String("text-x-csrc"));
}

void RussellConfigPage::apply() {
    KConfigGroup config(KSharedConfig::openConfig(), QLatin1String("Russell"));
    config.writeEntry("RussellHost", configUi_.russellHostEdit->text());
    config.writeEntry("RussellPort", configUi_.russellPortEdit->text());
    config.writeEntry("RussellInvocation", configUi_.russellInvocationEdit->text());
    config.writeEntry("RussellAutostart", configUi_.russellAutostartCheckBox->isChecked() ? "true" : "false");
    config.writeEntry("MetamathInvocation", configUi_.metamathInvocationEdit->text());
    config.writeEntry("MetamathAutostart", configUi_.metamathAutostartCheckBox->isChecked() ? "true" : "false");
    config.writeEntry("RussellConsoleInvocation", configUi_.russellConsoleInvocationEdit->text());
    config.writeEntry("RussellConsoleAutostart", configUi_.russellConsoleAutostartCheckBox->isChecked() ? "true" : "false");
    config.writeEntry("RussellRunner", configUi_.russellConsoleRadioButton->isChecked() ? "CONSOLE" : "CLIENT");
    config.sync();
}

void RussellConfigPage::reset() {
    KConfigGroup config(KSharedConfig::openConfig(), "Russell");
    configUi_.russellHostEdit->setText(config.readEntry(QLatin1String("RussellHost"), default_russell_host()));
    configUi_.russellPortEdit->setText(config.readEntry(QLatin1String("RussellPort"), QString::number(default_russell_port())));
    configUi_.russellInvocationEdit->setText(config.readEntry(QLatin1String("RussellInvocation"), default_russell_invocation()));
    configUi_.russellAutostartCheckBox->setChecked(to_bool(config.readEntry(QLatin1String("RussellAutostart"), to_string(default_russell_autostart()))));
    configUi_.metamathInvocationEdit->setText(config.readEntry(QLatin1String("MetamathInvocation"), default_metamath_invocation()));
    configUi_.metamathAutostartCheckBox->setChecked(to_bool(config.readEntry(QLatin1String("MetamathAutostart"), to_string(default_metamath_autostart()))));
    configUi_.russellConsoleInvocationEdit->setText(config.readEntry(QLatin1String("RussellConsoleInvocation"), default_russell_console_invocation()));
    configUi_.russellConsoleAutostartCheckBox->setChecked(to_bool(config.readEntry(QLatin1String("RussellConsoleAutostart"), to_string(default_russell_console_autostart()))));
    if (config.readEntry(QLatin1String("RussellRunner"), default_russell_runner()) == QLatin1String("CONSOLE")) {
    	configUi_.russellConsoleRadioButton->setChecked(true);
    	slotEnableConsole();
    } else {
    	configUi_.russellDaemonRadioButton->setChecked(true);
    	slotEnableDaemon();
    }
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
	configUi_.russellConsoleInvocationEdit->setText(default_russell_console_invocation());
	configUi_.russellConsoleAutostartCheckBox->setChecked(default_russell_console_autostart());
	if (default_russell_runner() == QLatin1String("CONSOLE")) {
    	configUi_.russellConsoleRadioButton->setChecked(true);
    	slotEnableConsole();
    } else {
    	configUi_.russellDaemonRadioButton->setChecked(true);
    	slotEnableDaemon();
    }
	checkRussellSlot();
}

// Russell daemon config

void RussellConfigPage::resetRussellConfigSlot() {
	configUi_.russellHostEdit->setText(default_russell_host());
	configUi_.russellPortEdit->setText(QString::number(default_russell_port()));
	configUi_.russellInvocationEdit->setText(default_russell_invocation());
	configUi_.russellAutostartCheckBox->setChecked(default_russell_autostart());
	checkRussellSlot();
}

void RussellConfigPage::startRussellSlot() {
	if (!Launcher::russellClient().isRunning()) {
		Launcher::russellClient().start();
		//QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		//usleep(500);
		//QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
		checkRussellSlot();
	}
}

void RussellConfigPage::stopRussellSlot() {
	if (Launcher::russellClient().isRunning()) {
		Execute::exec(QStringList() << QLatin1String("rus exit"));
		checkRussellSlot();
	}
}

void RussellConfigPage::killRussellSlot() {
	if (Launcher::russellClient().isRunning()) {
		Launcher::russellClient().stop();
	}
}

bool RussellConfigPage::checkRussellSlot() {
	bool ret = RussellClient::checkConnection();
	configUi_.russellAliveEdit->setText(ret ? QLatin1String("running") : QLatin1String("is not running"));
	configUi_.russellStopButton->setEnabled(ret);
	configUi_.russellStartButton->setEnabled(!ret);
	return ret;
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






// Metamath console config

void RussellConfigPage::lookupMetamathSlot() {
	QString metamath = QFileDialog::getOpenFileName(this, i18n("Metamath executable"), QString());
	configUi_.metamathInvocationEdit->setText(metamath);
}

void RussellConfigPage::startMetamathSlot() {
	if (!Launcher::metamath().isRunning()) {
		Launcher::metamath().start();
		//QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
		//usleep(500);
		//QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
	}
}

bool RussellConfigPage::checkMetamathSlot() {
	bool ret = Launcher::metamath().isRunning();
	configUi_.metamathAliveEdit->setText(ret ? QLatin1String("running") : QLatin1String("is not running"));
	configUi_.metamathStopButton->setEnabled(ret);
	configUi_.metamathStartButton->setEnabled(!ret);
	return ret;
}

void RussellConfigPage::stopMetamathSlot() {
	if (Launcher::metamath().isRunning()) {
		Execute::exec(QStringList() << QLatin1String("metamath exit"));
	}
}

void RussellConfigPage::killMetamathSlot() {
	if (Launcher::metamath().isRunning()) {
		Launcher::metamath().stop();
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



// Russell console config

void RussellConfigPage::lookupRussellConsoleSlot() {
	QString metamath = QFileDialog::getOpenFileName(this, i18n("Russell executable"), QString());
	configUi_.russellConsoleInvocationEdit->setText(metamath);
}

void RussellConfigPage::startRussellConsoleSlot() {
	if (!Launcher::russellConsole().isRunning()) {
		Launcher::russellConsole().start();
	}
}

bool RussellConfigPage::checkRussellConsoleSlot() {
	bool ret = Launcher::russellConsole().isRunning();
	configUi_.russellConsoleAliveEdit->setText(ret ? QLatin1String("running") : QLatin1String("is not running"));
	configUi_.russellConsoleStopButton->setEnabled(ret);
	configUi_.russellConsoleStartButton->setEnabled(!ret);
	return ret;
}

void RussellConfigPage::stopRussellConsoleSlot() {
	if (Launcher::russellConsole().isRunning()) {
		Execute::exec(QStringList() << QLatin1String("rus exit"));
	}
}

void RussellConfigPage::killRussellConsoleSlot() {
	if (Launcher::russellConsole().isRunning()) {
		Launcher::russellConsole().stop();
	}
}

void RussellConfigPage::startedRussellConsoleSlot() {
	configUi_.russellConsoleAliveEdit->setText(i18n("running"));
	configUi_.russellConsoleStopButton->setEnabled(true);
	configUi_.russellConsoleKillButton->setEnabled(true);
	configUi_.russellConsoleStartButton->setEnabled(false);
}

void RussellConfigPage::finishedRussellConsoleSlot(int exitCode, QProcess::ExitStatus exitStatus) {
	QString text = i18n("stopped");
	if (exitCode || exitStatus != QProcess::NormalExit) {
		text += i18n(" error code: ") + QString::number(exitCode);
	}
	configUi_.russellConsoleAliveEdit->setText(text);
	configUi_.russellConsoleStopButton->setEnabled(false);
	configUi_.russellConsoleKillButton->setEnabled(false);
	configUi_.russellConsoleStartButton->setEnabled(true);
}


void RussellConfigPage::slotEnableDaemon() {
	configUi_.russellConfigGroupBox->setEnabled(true);
	configUi_.russellConsoleConfigGroupBox->setEnabled(false);
}

void RussellConfigPage::slotEnableConsole() {
	configUi_.russellConfigGroupBox->setEnabled(false);
	configUi_.russellConsoleConfigGroupBox->setEnabled(true);
}

void RussellConfigPage::checkPortSlot(QString port_str) {
	bool ok = true;
	port_str.toInt(&ok);
	if (!ok) {
		KMessageBox::error(0, i18n("Port \"%1\" is not a decimal number", port_str));
	}
}

}
