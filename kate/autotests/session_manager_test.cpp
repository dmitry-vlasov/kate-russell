/* This file is part of the KDE project
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

#include "session_manager_test.h"
#include "katesessionmanager.h"
#include "kateapp.h"

#include <KConfig>
#include <KConfigGroup>

#include <QtTestWidgets>
#include <QTemporaryDir>
#include <QCommandLineParser>

QTEST_MAIN(KateSessionManagerTest)

void KateSessionManagerTest::initTestCase()
{
    m_app = new KateApp(QCommandLineParser()); // FIXME: aaaah, why, why, why?!
}

void KateSessionManagerTest::cleanupTestCase()
{
    delete m_app;
}

void KateSessionManagerTest::init()
{
    m_tempdir = new QTemporaryDir;
    QVERIFY(m_tempdir->isValid());

    m_manager = new KateSessionManager(this, m_tempdir->path());
}

void KateSessionManagerTest::cleanup()
{
    delete m_manager;
    delete m_tempdir;
}

void KateSessionManagerTest::basic()
{
    QCOMPARE(m_manager->sessionsDir(), m_tempdir->path());
    QCOMPARE(m_manager->sessionList().size(), 0);
    QCOMPARE(m_manager->activeSession()->isAnonymous(), true);
}

void KateSessionManagerTest::activateNewNamedSession()
{
    const QString sessionName = QStringLiteral("hello_world");

    QVERIFY(m_manager->activateSession(sessionName, false, false));
    QCOMPARE(m_manager->sessionList().size(), 1);

    KateSession::Ptr s = m_manager->activeSession();
    QCOMPARE(s->name(), sessionName);
    QCOMPARE(s->isAnonymous(), false);

    const QString sessionFile = m_tempdir->path() + QLatin1Char('/') + sessionName + QLatin1String(".katesession");
    QCOMPARE(s->config()->name(), sessionFile);
}

void KateSessionManagerTest::anonymousSessionFile()
{
    const QString anonfile = QDir().cleanPath(m_tempdir->path() + QLatin1String("/../anonymous.katesession"));
    QVERIFY(m_manager->activeSession()->isAnonymous());
    QCOMPARE(m_manager->activeSession()->config()->name(), anonfile);
}

void KateSessionManagerTest::urlizeSessionFile()
{
    const QString sessionName = QStringLiteral("hello world/#");

    m_manager->activateSession(sessionName, false, false);
    KateSession::Ptr s = m_manager->activeSession();

    const QString sessionFile = m_tempdir->path() + QLatin1String("/hello%20world%2F%23.katesession");
    QCOMPARE(s->config()->name(), sessionFile);
}

void KateSessionManagerTest::deleteSession()
{
    m_manager->activateSession(QStringLiteral("foo"));
    KateSession::Ptr s = m_manager->activeSession();

    m_manager->activateSession(QStringLiteral("bar"));

    QCOMPARE(m_manager->sessionList().size(), 2);

    m_manager->deleteSession(s);
    QCOMPARE(m_manager->sessionList().size(), 1);
}

void KateSessionManagerTest::deleteActiveSession()
{
    m_manager->activateSession(QStringLiteral("foo"));
    KateSession::Ptr s = m_manager->activeSession();

    QCOMPARE(m_manager->sessionList().size(), 1);
    m_manager->deleteSession(s);
    QCOMPARE(m_manager->sessionList().size(), 1);
}

void KateSessionManagerTest::renameSession()
{
    m_manager->activateSession(QStringLiteral("foo"));
    KateSession::Ptr s = m_manager->activeSession();

    QCOMPARE(m_manager->sessionList().size(), 1);

    const QString newName = QStringLiteral("bar");
    m_manager->renameSession(s, newName); // non-collision path
    QCOMPARE(s->name(), newName);
    QCOMPARE(m_manager->sessionList().size(), 1);
    QCOMPARE(m_manager->sessionList().first(), s);
}

void KateSessionManagerTest::saveActiveSessionWithAnynomous()
{
    QVERIFY(m_manager->activeSession()->isAnonymous());
    QVERIFY(m_manager->sessionList().size() == 0);

    QCOMPARE(m_manager->saveActiveSession(), true);
    QCOMPARE(m_manager->activeSession()->isAnonymous(), true);
    QCOMPARE(m_manager->activeSession()->name(), QString());
    QCOMPARE(m_manager->sessionList().size(), 0);
}

void KateSessionManagerTest::deletingSessionFilesUnderRunningApp()
{
    m_manager->activateSession(QStringLiteral("foo"));
    m_manager->activateSession(QStringLiteral("bar"));

    QVERIFY(m_manager->sessionList().size() == 2);
    QVERIFY(m_manager->activeSession()->name() == QLatin1String("bar"));

    const QString file = m_tempdir->path() + QLatin1String("/foo.katesession");
    QVERIFY(QFile(file).remove());

    QTRY_COMPARE_WITH_TIMEOUT(m_manager->sessionList().size(), 1, 1000); // that should be enough for KDirWatch to kick in
    QCOMPARE(m_manager->activeSession()->name(), QLatin1String("bar"));
}

void KateSessionManagerTest::startNonEmpty()
{
    m_manager->activateSession(QStringLiteral("foo"));
    m_manager->activateSession(QStringLiteral("bar"));

    KateSessionManager m(this, m_tempdir->path());
    QCOMPARE(m.sessionList().size(), 2);
}


