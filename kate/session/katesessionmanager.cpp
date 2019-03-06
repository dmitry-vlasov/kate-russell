/* This file is part of the KDE project
 *
 *  Copyright (C) 2005 Christoph Cullmann <cullmann@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this library; see the file COPYING.LIB.  If not, write to
 *  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 */

#include "config.h"

#include "katesessionmanager.h"

#include "katesessionmanagedialog.h"

#include "kateapp.h"
#include "katepluginmanager.h"
#include "katerunninginstanceinfo.h"

#include <KConfigGroup>
#include <KSharedConfig>
#include <KLocalizedString>
#include <KMessageBox>
#include <KIO/CopyJob>
#include <KDesktopFile>
#include <KDirWatch>
#include <KService>
#include <KShell>

#include <QApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QInputDialog>
#include <QScopedPointer>
#include <QUrl>

#ifndef Q_OS_WIN
#include <unistd.h>
#endif

//BEGIN KateSessionManager

KateSessionManager::KateSessionManager(QObject *parent, const QString &sessionsDir)
    : QObject(parent)
{
    if (sessionsDir.isEmpty()) {
        m_sessionsDir = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/kate/sessions");
    } else {
        m_sessionsDir = sessionsDir;
    }

    // create dir if needed
    QDir().mkpath(m_sessionsDir);

    m_dirWatch = new KDirWatch(this);
    m_dirWatch->addDir(m_sessionsDir);
    connect(m_dirWatch, &KDirWatch::dirty, this, &KateSessionManager::updateSessionList);

    updateSessionList();
}

KateSessionManager::~KateSessionManager()
{
    delete m_dirWatch;
}

void KateSessionManager::updateSessionList()
{
    QStringList list;

    // Let's get a list of all session we have atm
    QDir dir(m_sessionsDir, QStringLiteral("*.katesession"), QDir::Time);

    for (unsigned int i = 0; i < dir.count(); ++i) {
        QString name = dir[i];
        name.chop(12); // .katesession
        list << QUrl::fromPercentEncoding(name.toLatin1());
    }

    // write jump list actions to disk in the kate.desktop file
    updateJumpListActions(list);

    bool changed = false;

    // Add new sessions to our list
    for (const QString session : qAsConst(list)) {
        if (!m_sessions.contains(session)) {
            const QString file = sessionFileForName(session);
            m_sessions.insert(session, KateSession::create(file, session));
            changed = true;
        }
    }
    // Remove gone sessions from our list
    for (const QString session : m_sessions.keys()) {
        if ((list.indexOf(session) < 0) && (m_sessions.value(session) != activeSession())) {
            m_sessions.remove(session);
            changed = true;
        }
    }

    if (changed) {
        emit sessionListChanged();
    }
}

bool KateSessionManager::activateSession(KateSession::Ptr session,
        const bool closeAndSaveLast,
        const bool loadNew)
{
    if (activeSession() == session) {
        return true;
    }

    if (!session->isAnonymous()) {
        //check if the requested session is already open in another instance
        KateRunningInstanceMap instances;
        if (!fillinRunningKateAppInstances(&instances)) {
            KMessageBox::error(nullptr, i18n("Internal error: there is more than one instance open for a given session."));
            return false;
        }

        if (instances.contains(session->name())) {
            if (KMessageBox::questionYesNo(nullptr, i18n("Session '%1' is already opened in another kate instance, change there instead of reopening?", session->name()),
                                           QString(), KStandardGuiItem::yes(), KStandardGuiItem::no(), QStringLiteral("katesessionmanager_switch_instance")) == KMessageBox::Yes) {
                instances[session->name()]->dbus_if->call(QStringLiteral("activate"));
                cleanupRunningKateAppInstanceMap(&instances);
                return false;
            }
        }

        cleanupRunningKateAppInstanceMap(&instances);
    }
    // try to close and save last session
    if (closeAndSaveLast) {
        if (KateApp::self()->activeKateMainWindow()) {
            if (!KateApp::self()->activeKateMainWindow()->queryClose_internal()) {
                return true;
            }
        }

        // save last session or not?
        saveActiveSession();

        // really close last
        KateApp::self()->documentManager()->closeAllDocuments();
    }

    // set the new session
    m_activeSession = session;

    // there is one case in which we don't want the restoration and that is
    // when restoring session from session manager.
    // In that case the restore is handled by the caller
    if (loadNew) {
        loadSession(session);
    }

    emit sessionChanged();
    return true;
}

void KateSessionManager::loadSession(const KateSession::Ptr &session) const
{
    // open the new session
    KSharedConfigPtr sharedConfig = KSharedConfig::openConfig();
    KConfig *sc = session->config();
    const bool loadDocs = !session->isAnonymous(); // do not load docs for new sessions

    // if we have no session config object, try to load the default
    // (anonymous/unnamed sessions)
    // load plugin config + plugins
    KateApp::self()->pluginManager()->loadConfig(sc);

    if (loadDocs) {
        KateApp::self()->documentManager()->restoreDocumentList(sc);
    }

    // window config
    KConfigGroup c(sharedConfig, "General");

    if (c.readEntry("Restore Window Configuration", true)) {
        KConfig *cfg = sc;
        bool delete_cfg = false;
        // a new, named session, read settings of the default session.
        if (! sc->hasGroup("Open MainWindows")) {
            delete_cfg = true;
            cfg = new KConfig(anonymousSessionFile(), KConfig::SimpleConfig);
        }

        int wCount = cfg->group("Open MainWindows").readEntry("Count", 1);

        for (int i = 0; i < wCount; ++i) {
            if (i >= KateApp::self()->mainWindowsCount()) {
                KateApp::self()->newMainWindow(cfg, QStringLiteral("MainWindow%1").arg(i));
            } else {
                KateApp::self()->mainWindow(i)->readProperties(KConfigGroup(cfg, QStringLiteral("MainWindow%1").arg(i)));
            }

            KateApp::self()->mainWindow(i)->restoreWindowConfig(KConfigGroup(cfg, QStringLiteral("MainWindow%1 Settings").arg(i)));
        }

        if (delete_cfg) {
            delete cfg;
        }

        // remove mainwindows we need no longer...
        if (wCount > 0) {
            while (wCount < KateApp::self()->mainWindowsCount()) {
                delete KateApp::self()->mainWindow(KateApp::self()->mainWindowsCount() - 1);
            }
        }
    }
}

bool KateSessionManager::activateSession(const QString &name, const bool closeAndSaveLast, const bool loadNew)
{
    return activateSession(giveSession(name), closeAndSaveLast, loadNew);
}

bool KateSessionManager::activateAnonymousSession()
{
    return activateSession(QString(), false);
}

KateSession::Ptr KateSessionManager::giveSession(const QString &name)
{
    if (name.isEmpty()) {
        return KateSession::createAnonymous(anonymousSessionFile());
    }

    if (m_sessions.contains(name)) {
        return m_sessions.value(name);
    }

    KateSession::Ptr s = KateSession::create(sessionFileForName(name), name);
    saveSessionTo(s->config());
    m_sessions[name] = s;
    // Due to this add to m_sessions will updateSessionList() no signal emit,
    // but it's importand to add. Otherwise could it be happen that m_activeSession
    // is not part of m_sessions but a double
    emit sessionListChanged();

    return s;
}

bool KateSessionManager::deleteSession(KateSession::Ptr session)
{
    if (sessionIsActive(session->name())) {
        return false;
    }

    QFile::remove(session->file());
    m_sessions.remove(session->name());
    // Due to this remove from m_sessions will updateSessionList() no signal emit,
    // but this way is there no delay between deletion and information
    emit sessionListChanged();

    return true;
}

QString KateSessionManager::copySession(const KateSession::Ptr &session, const QString &newName)
{
    const QString name = askForNewSessionName(session, newName);

    if (name.isEmpty()) {
        return name;
    }

    const QString newFile = sessionFileForName(name);

    KateSession::Ptr ns = KateSession::createFrom(session, newFile, name);
    ns->config()->sync();

    return name;
}

QString KateSessionManager::renameSession(KateSession::Ptr session, const QString &newName)
{
    const QString name = askForNewSessionName(session, newName);

    if (name.isEmpty()) {
        return name;
    }

    const QString newFile = sessionFileForName(name);

    session->config()->sync();

    const QUrl srcUrl = QUrl::fromLocalFile(session->file());
    const QUrl dstUrl = QUrl::fromLocalFile(newFile);
    KIO::CopyJob *job = KIO::move(srcUrl, dstUrl, KIO::HideProgressInfo);

    if (!job->exec()) {
        KMessageBox::sorry(QApplication::activeWindow(),
                           i18n("The session could not be renamed to \"%1\". Failed to write to \"%2\"", newName, newFile),
                           i18n("Session Renaming"));
        return QString();
    }

    m_sessions[newName] = m_sessions.take(session->name());
    session->setName(newName);
    session->setFile(newFile);
    session->config()->sync();
    // updateSessionList() will this edit not notice, so force signal
    emit sessionListChanged();

    if (session == activeSession()) {
        emit sessionChanged();
    }

    return name;
}

void KateSessionManager::saveSessionTo(KConfig *sc) const
{
    // Clear the session file to avoid to accumulate outdated entries
    for (auto group : sc->groupList()) {
        sc->deleteGroup(group);
    }

    // save plugin configs and which plugins to load
    KateApp::self()->pluginManager()->writeConfig(sc);

    // save document configs + which documents to load
    KateApp::self()->documentManager()->saveDocumentList(sc);

    sc->group("Open MainWindows").writeEntry("Count", KateApp::self()->mainWindowsCount());

    // save config for all windows around ;)
    bool saveWindowConfig = KConfigGroup(KSharedConfig::openConfig(), "General").readEntry("Restore Window Configuration", true);
    for (int i = 0; i < KateApp::self()->mainWindowsCount(); ++i) {
        KConfigGroup cg(sc, QStringLiteral("MainWindow%1").arg(i));
        KateApp::self()->mainWindow(i)->saveProperties(cg);
        if (saveWindowConfig) {
            KateApp::self()->mainWindow(i)->saveWindowConfig(KConfigGroup(sc, QStringLiteral("MainWindow%1 Settings").arg(i)));
        }
    }

    sc->sync();

    /**
     * try to sync file to disk
     */
    QFile fileToSync(sc->name());
    if (fileToSync.open(QIODevice::ReadOnly)) {
#ifndef Q_OS_WIN
        // ensure that the file is written to disk
#ifdef HAVE_FDATASYNC
        fdatasync(fileToSync.handle());
#else
        fsync(fileToSync.handle());
#endif
#endif
    }
}

bool KateSessionManager::saveActiveSession(bool rememberAsLast)
{
    if (!activeSession()) {
        return false;
    }

    KConfig *sc = activeSession()->config();

    saveSessionTo(sc);

    if (rememberAsLast && !activeSession()->isAnonymous()) {
        KSharedConfigPtr c = KSharedConfig::openConfig();
        c->group("General").writeEntry("Last Session", activeSession()->name());
        c->sync();
    }
    return true;
}

bool KateSessionManager::chooseSession()
{
    const KConfigGroup c(KSharedConfig::openConfig(), "General");

    // get last used session, default to default session
    const QString lastSession(c.readEntry("Last Session", QString()));
    const QString sesStart(c.readEntry("Startup Session", "manual"));

    // uhh, just open last used session, show no chooser
    if (sesStart == QStringLiteral("last")) {
        activateSession(lastSession, false);
        return true;
    }

    // start with empty new session or in case no sessions exist
    if (sesStart == QStringLiteral("new") || sessionList().size() == 0) {
        activateAnonymousSession();
        return true;
    }

    return QScopedPointer<KateSessionManageDialog>(new KateSessionManageDialog(nullptr, lastSession))->exec();
}

void KateSessionManager::sessionNew()
{
    activateSession(giveSession(QString()));
}

void KateSessionManager::sessionSave()
{
    if (activeSession() && activeSession()->isAnonymous()) {
        sessionSaveAs();
    } else {
        saveActiveSession();
    }
}

void KateSessionManager::sessionSaveAs()
{
    const QString newName = askForNewSessionName(activeSession());

    if (newName.isEmpty()) {
        return;
    }

    activeSession()->config()->sync();

    KateSession::Ptr ns = KateSession::createFrom(activeSession(), sessionFileForName(newName), newName);
    m_activeSession = ns;
    saveActiveSession();

    emit sessionChanged();
}

QString KateSessionManager::askForNewSessionName(KateSession::Ptr session, const QString &newName)
{
    if (session->name() == newName && !session->isAnonymous()) {
        return QString();
    }

    const QString messagePrompt = i18n("Session name:");
    const KLocalizedString messageExist = ki18n("There is already an existing session with your chosen name: %1\n"
                                                "Please choose a different one.");
    const QString messageEmpty = i18n("To save a session, you must specify a name.");

    QString messageTotal = messagePrompt;
    QString name = newName;

    while (true) {
        QString preset = name;

        if (name.isEmpty()) {
            preset = suggestNewSessionName(session->name());
            messageTotal = messageEmpty + QStringLiteral("\n\n") + messagePrompt;

        } else if (QFile::exists(sessionFileForName(name))) {
            preset = suggestNewSessionName(name);
            if (preset.isEmpty()) {
                // Very unlikely, but as fall back we keep users input
                preset = name;
            }
            messageTotal = messageExist.subs(name).toString() + QStringLiteral("\n\n") + messagePrompt;

        } else {
            return name;
        }

        QInputDialog dlg(KateApp::self()->activeKateMainWindow());
        dlg.setInputMode(QInputDialog::TextInput);
        if (session->isAnonymous()) {
            dlg.setWindowTitle(i18n("Specify a name for this session"));
        } else {
            dlg.setWindowTitle(i18n("Specify a new name for session: %1", session->name()));
        }
        dlg.setLabelText(messageTotal);
        dlg.setTextValue(preset);
        dlg.resize(900,100);  // FIXME Calc somehow a proper size
        bool ok = dlg.exec();
        name = dlg.textValue();

        if (!ok) {
            return QString();
        }
    }
}

QString KateSessionManager::suggestNewSessionName(const QString &target)
{
    if (target.isEmpty()) {
        // Here could also a default name set or the current session name used
        return QString();
    }

    const QString mask = QStringLiteral("%1 (%2)");
    QString name;

    for (int i = 2; i < 1000000; i++) { // Should be enough to get an unique name
        name = mask.arg(target).arg(i);

        if (!QFile::exists(sessionFileForName(name))) {
            return name;
        }
    }

    return QString();
}

void KateSessionManager::sessionManage()
{
    QScopedPointer<KateSessionManageDialog>(new KateSessionManageDialog(KateApp::self()->activeKateMainWindow()))->exec();
}


bool KateSessionManager::sessionIsActive(const QString &session)
{
    // Try to avoid unneed action
    if (activeSession() && activeSession()->name() == session) {
        return true;
    }

    QDBusConnectionInterface *i = QDBusConnection::sessionBus().interface();
    if (!i) {
        return false;
    }

    // look up all running kate instances and there sessions
    QDBusReply<QStringList> servicesReply = i->registeredServiceNames();
    QStringList services;
    if (servicesReply.isValid()) {
        services = servicesReply.value();
    }

    for (const QString &s : qAsConst(services)) {
        if (!s.startsWith(QStringLiteral("org.kde.kate-"))) {
            continue;
        }

        KateRunningInstanceInfo rii(s);
        if (rii.valid && rii.sessionName == session) {
            return true;
        }
    }

    return false;
}


QString KateSessionManager::anonymousSessionFile() const
{
    const QString file = m_sessionsDir + QStringLiteral("/../anonymous.katesession");
    return QDir().cleanPath(file);
}

QString KateSessionManager::sessionFileForName(const QString &name) const
{
    Q_ASSERT(!name.isEmpty());
    const QString sname = QString::fromLatin1(QUrl::toPercentEncoding(name, QByteArray(), QByteArray(".")));
    return m_sessionsDir + QStringLiteral("/") + sname + QStringLiteral(".katesession");
}

KateSessionList KateSessionManager::sessionList()
{
    return m_sessions.values();
}

void KateSessionManager::updateJumpListActions(const QStringList &sessionList)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    KService::Ptr service = KService::serviceByStorageId(qApp->desktopFileName());
    if (!service) {
        return;
    }

    QScopedPointer<KDesktopFile> df(new KDesktopFile(service->entryPath()));

    QStringList newActions = df->readActions();

    // try to keep existing custom actions intact, only remove our "Session" actions and add them back later
    newActions.erase(std::remove_if(newActions.begin(), newActions.end(), [](const QString &action) {
        return action.startsWith(QLatin1String("Session "));
    }), newActions.end());

    // Limit the number of list entries we like to offer
    const int maxEntryCount = std::min(sessionList.count(), 10);

    // sessionList is ordered by time, but we like it alphabetical to avoid even more a needed update
    QStringList sessionSubList = sessionList.mid(0, maxEntryCount);
    sessionSubList.sort();

    // we compute the new group names in advance so we can tell whether we changed something
    // and avoid touching the desktop file leading to an expensive ksycoca recreation
    QStringList sessionActions;
    sessionActions.reserve(maxEntryCount);

    for (int i = 0; i < maxEntryCount; ++i) {
        sessionActions << QStringLiteral("Session %1").arg(QString::fromLatin1(QCryptographicHash::hash(sessionSubList.at(i).toUtf8()
                                                                             , QCryptographicHash::Md5).toHex()));
    }

    newActions += sessionActions;

    // nothing to do
    if (df->readActions() == newActions) {
        return;
    }

    const QString &localPath = service->locateLocal();
    if (service->entryPath() != localPath) {
        df.reset(df->copyTo(localPath));
    }

    // remove all Session action groups first to not leave behind any cruft
    for (const QString &action : df->readActions()) {
        if (action.startsWith(QLatin1String("Session "))) {
            // TODO is there no deleteGroup(KConfigGroup)?
            df->deleteGroup(df->actionGroup(action).name());
        }
    }

    for (int i = 0; i < maxEntryCount; ++i) {
        const QString &action = sessionActions.at(i); // is a transform of sessionSubList, so count and order is identical
        const QString &session = sessionSubList.at(i);

        KConfigGroup grp = df->actionGroup(action);
        grp.writeEntry(QStringLiteral("Name"), session);
        grp.writeEntry(QStringLiteral("Exec"), QStringLiteral("kate -s %1").arg(KShell::quoteArg(session))); // TODO proper executable name?
    }

    df->desktopGroup().writeXdgListEntry("Actions", newActions);
#endif
}

//END KateSessionManager

