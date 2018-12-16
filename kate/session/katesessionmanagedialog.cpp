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

#include "katesessionmanagedialog.h"

#include "kateapp.h"
#include "katesessionchooseritem.h"
#include "katesessionmanager.h"

#include <KColorScheme>
#include <KConfigGroup>
#include <KLocalizedString>
#include <KMessageBox>
#include <KSharedConfig>
#include <KStandardGuiItem>

#include <QInputDialog>

KateSessionManageDialog::KateSessionManageDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    setWindowTitle(i18n("Manage Sessions"));
    m_dontAskCheckBox->hide();

    m_sessionList->installEventFilter(this);
    connect(m_sessionList, &QTreeWidget::currentItemChanged, this, &KateSessionManageDialog::selectionChanged);
    connect(m_sessionList, &QTreeWidget::itemDoubleClicked, this, &KateSessionManageDialog::openSession);
    m_sessionList->header()->moveSection(0, 1); // Re-order columns to "Files, Sessions"

    m_filterBox->installEventFilter(this);
    connect(m_filterBox, &QLineEdit::textChanged, this, &KateSessionManageDialog::filterChanged);
    connect(m_sortButton, &QPushButton::clicked, this, &KateSessionManageDialog::changeSortOrder);

    connect(m_newButton, &QPushButton::clicked, this, &KateSessionManageDialog::openNewSession);

    KGuiItem::assign(m_openButton, KStandardGuiItem::open());
    m_openButton->setDefault(true);
    connect(m_openButton, &QPushButton::clicked, this, &KateSessionManageDialog::openSession);

    connect(m_templateButton, &QPushButton::clicked, this, &KateSessionManageDialog::openSessionAsTemplate);

    connect(m_copyButton, &QPushButton::clicked, this, &KateSessionManageDialog::copySession);

    connect(m_renameButton, &QPushButton::clicked, this, &KateSessionManageDialog::editBegin);

    connect(m_deleteButton, &QPushButton::clicked, this, &KateSessionManageDialog::updateDeleteList);

    KGuiItem::assign(m_closeButton, KStandardGuiItem::close());
    connect(m_closeButton, &QPushButton::clicked, this, &KateSessionManageDialog::closeDialog);

    connect(KateApp::self()->sessionManager(), &KateSessionManager::sessionListChanged, this, &KateSessionManageDialog::updateSessionList);

    changeSortOrder(); // Set order to SortAlphabetical, set button text and fill session list
}


KateSessionManageDialog::KateSessionManageDialog(QWidget *parent, const QString &lastSession)
    : KateSessionManageDialog(parent)
{
    setWindowTitle(i18n("Session Chooser"));
    m_dontAskCheckBox->show();
    m_chooserMode = true;
    connect(m_dontAskCheckBox, &QCheckBox::toggled, this, &KateSessionManageDialog::dontAskToggled);

    m_prefferedSession = lastSession;
    changeSortOrder(); // Set order to SortChronological
}


KateSessionManageDialog::~KateSessionManageDialog()
{}


void KateSessionManageDialog::dontAskToggled()
{
    m_templateButton->setEnabled(!m_dontAskCheckBox->isChecked());
}


void KateSessionManageDialog::changeSortOrder()
{
    switch (m_sortOrder) {
        case SortAlphabetical:
            m_sortOrder = SortChronological;
            m_sortButton->setText(i18n("Sort Alphabetical"));
            //m_sortButton->setIcon(QIcon::fromTheme(QStringLiteral("FIXME")));
            break;
        case SortChronological:
            m_sortOrder = SortAlphabetical;
            m_sortButton->setText(i18n("Sort Last Used"));
            //m_sortButton->setIcon(QIcon::fromTheme(QStringLiteral("FIXME")));
            break;
    }

    updateSessionList();
}


void KateSessionManageDialog::filterChanged()
{
    static QPointer<QTimer> delay;

    if (!delay) {
        delay = new QTimer(this); // Should be auto cleard by Qt when we die
        delay->setSingleShot(true);
        delay->setInterval(400);
        connect(delay, &QTimer::timeout, this, &KateSessionManageDialog::updateSessionList);
    }

    delay->start();
}


void KateSessionManageDialog::done(int result)
{
    for (auto session : qAsConst(m_deleteList)) {
        KateApp::self()->sessionManager()->deleteSession(session);
    }
    m_deleteList.clear(); // May not needed, but anyway

    if (ResultQuit == result) {
        QDialog::done(0);
        return;
    }

    if (m_chooserMode && m_dontAskCheckBox->isChecked()) {
        // write back our nice boolean :)
        KConfigGroup generalConfig(KSharedConfig::openConfig(), QStringLiteral("General"));
        switch (result) {
        case ResultOpen:
            generalConfig.writeEntry("Startup Session", "last");
            break;
        case ResultNew:
            generalConfig.writeEntry("Startup Session", "new");
            break;
        default:
            break;
        }
        generalConfig.sync();
    }

    QDialog::done(1);
}


void KateSessionManageDialog::selectionChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    Q_UNUSED(previous);

    if (m_editByUser) {
        editDone(); // Field was left unchanged, no need to apply
        return;
    }

    if (!current) {
        m_openButton->setEnabled(false);
        m_templateButton->setEnabled(false);
        m_copyButton->setEnabled(false);
        m_renameButton->setEnabled(false);
        m_deleteButton->setEnabled(false);
        return;
    }

    const KateSession::Ptr activeSession = KateApp::self()->sessionManager()->activeSession();
    const bool notActiveSession = !KateApp::self()->sessionManager()->sessionIsActive(currentSelectedSession()->name());

    m_deleteButton->setEnabled(notActiveSession);

    if (m_deleteList.contains(currentSelectedSession())) {
        m_deleteButton->setText(i18n("Restore"));
        m_openButton->setEnabled(false);
        m_templateButton->setEnabled(false);
        m_copyButton->setEnabled(true); // Looks a little strange but is OK
        m_renameButton->setEnabled(false);
    } else {
        KGuiItem::assign(m_deleteButton, KStandardGuiItem::del());
        m_openButton->setEnabled(currentSelectedSession() != activeSession);
        m_templateButton->setEnabled(true);
        m_copyButton->setEnabled(true);
        m_renameButton->setEnabled(true);
    }
}


void KateSessionManageDialog::disableButtons()
{
    m_openButton->setEnabled(false);
    m_newButton->setEnabled(false);
    m_templateButton->setEnabled(false);
    m_dontAskCheckBox->setEnabled(false);
    m_copyButton->setEnabled(false);
    m_renameButton->setEnabled(false);
    m_deleteButton->setEnabled(false);
    m_closeButton->setEnabled(false);
    m_sortButton->setEnabled(false);
    m_filterBox->setEnabled(false);
}


void KateSessionManageDialog::editBegin()
{
    if (m_editByUser) {
        return;
    }

    KateSessionChooserItem *item = currentSessionItem();

    if (!item) {
        return;
    }

    disableButtons();

    item->setFlags(item->flags() | Qt::ItemIsEditable);
    m_sessionList->clearSelection();
    m_sessionList->editItem(item, 0);

    // Always apply changes user did, like Dolphin
    connect(m_sessionList, &QTreeWidget::itemChanged, this, &KateSessionManageDialog::editApply);
    connect(m_sessionList->itemWidget(item, 0), &QObject::destroyed, this, &KateSessionManageDialog::editApply);

    m_editByUser = item; // Do it last to block eventFilter() actions until we are ready
}


void KateSessionManageDialog::editDone()
{
    m_editByUser = nullptr;
    disconnect(m_sessionList, &QTreeWidget::itemChanged, this, &KateSessionManageDialog::editApply);
    updateSessionList();

    m_newButton->setEnabled(true);
    m_dontAskCheckBox->setEnabled(true);
    m_closeButton->setEnabled(true);
    m_sortButton->setEnabled(true);
    m_filterBox->setEnabled(true);

    m_sessionList->setFocus();
}


void KateSessionManageDialog::editApply()
{
    if (!m_editByUser) {
        return;
    }

    KateApp::self()->sessionManager()->renameSession(m_editByUser->session, m_editByUser->text(0));
    editDone();
}


void KateSessionManageDialog::copySession()
{
    KateSessionChooserItem *item = currentSessionItem();

    if (!item) {
        return;
    }

    m_prefferedSession = KateApp::self()->sessionManager()->copySession(item->session);
    m_sessionList->setFocus(); // Only needed when user abort
}


void KateSessionManageDialog::openSession()
{
    KateSessionChooserItem *item = currentSessionItem();

    if (!item) {
        return;
    }

    hide();
    KateApp::self()->sessionManager()->activateSession(item->session);
    done(ResultOpen);
}


void KateSessionManageDialog::openSessionAsTemplate()
{
    KateSessionChooserItem *item = currentSessionItem();

    if (!item) {
        return;
    }

    hide();

    KateSessionManager *sm = KateApp::self()->sessionManager();
    KateSession::Ptr ns = KateSession::createAnonymousFrom(item->session, sm->anonymousSessionFile());
    sm->activateSession(ns);

    done(ResultOpen);
}


void KateSessionManageDialog::openNewSession()
{
    hide();
    KateApp::self()->sessionManager()->sessionNew();
    done(ResultNew);
}


void KateSessionManageDialog::updateDeleteList()
{
    KateSessionChooserItem *item = currentSessionItem();

    if (!item) {
        return;
    }

    const KateSession::Ptr session = item->session;
    if (m_deleteList.contains(session)) {
        m_deleteList.remove(session);
        item->setForeground(0, QBrush(KColorScheme(QPalette::Active).foreground(KColorScheme::NormalText).color()));
        item->setIcon(0, QIcon());
        item->setToolTip(0, QString());
    } else {
        m_deleteList.insert(session);
        markItemAsToBeDeleted(item);
    }

    // To ease multiple deletions, move the selection
    QTreeWidgetItem *newItem = m_sessionList->itemBelow(item) ? m_sessionList->itemBelow(item) : m_sessionList->topLevelItem(0);
    m_sessionList->setCurrentItem(newItem);
    m_sessionList->setFocus();
}


void KateSessionManageDialog::markItemAsToBeDeleted(QTreeWidgetItem *item)
{
    item->setForeground(0, QBrush(KColorScheme(QPalette::Active).foreground(KColorScheme::InactiveText).color()));
    item->setIcon(0, QIcon::fromTheme(QStringLiteral("emblem-warning")));
    item->setToolTip(0, i18n("Session will be deleted on dialog close"));
}


void KateSessionManageDialog::closeDialog()
{
    done(ResultQuit);
}


void KateSessionManageDialog::updateSessionList()
{
    if (m_editByUser) {
        // Don't crash accidentally an ongoing edit
        return;
    }

    KateSession::Ptr currSelSession = currentSelectedSession();
    KateSession::Ptr activeSession = KateApp::self()->sessionManager()->activeSession();

    m_sessionList->clear();

    KateSessionList slist = KateApp::self()->sessionManager()->sessionList();
    switch (m_sortOrder) {
        case SortAlphabetical: std::sort (slist.begin(), slist.end(), KateSession::compareByName); break;
        case SortChronological: std::sort (slist.begin(), slist.end(), KateSession::compareByTimeDesc); break;
    }

    KateSessionChooserItem *prefferedItem = nullptr;
    KateSessionChooserItem *currSessionItem = nullptr;
    KateSessionChooserItem *activeSessionItem= nullptr;

    for (const KateSession::Ptr &session : qAsConst(slist)) {
        if (!m_filterBox->text().isEmpty()) {
            if (!session->name().contains(m_filterBox->text(), Qt::CaseInsensitive)) {
                continue;
            }
        }

        KateSessionChooserItem *item = new KateSessionChooserItem(m_sessionList, session);
        if (session == currSelSession) {
            currSessionItem = item;
        } else if (session == activeSession) {
            activeSessionItem = item;
        } else if (session->name() == m_prefferedSession) {
            prefferedItem = item;
            m_prefferedSession.clear();
        }

        if (m_deleteList.contains(session)) {
            markItemAsToBeDeleted(item);
        }
    }

    m_sessionList->resizeColumnToContents(1); // Minimize "Files" column

    if (!prefferedItem) {
        prefferedItem = currSessionItem ? currSessionItem : activeSessionItem;
    }

    if (prefferedItem) {
        m_sessionList->setCurrentItem(prefferedItem);
        m_sessionList->scrollToItem(prefferedItem);
    } else if (m_sessionList->topLevelItemCount() > 0) {
        m_sessionList->setCurrentItem(m_sessionList->topLevelItem(0));
    }

    if (m_filterBox->hasFocus()){
        return;
    }

    if (m_sessionList->topLevelItemCount() == 0) {
        m_newButton->setFocus();
    } else {
        m_sessionList->setFocus();
    }
}


KateSessionChooserItem *KateSessionManageDialog::currentSessionItem() const
{
    return static_cast<KateSessionChooserItem *>(m_sessionList->currentItem());
}


KateSession::Ptr KateSessionManageDialog::currentSelectedSession() const
{
    KateSessionChooserItem *item = currentSessionItem();

    if (!item) {
        return KateSession::Ptr();
    }

    return item->session;
}


bool KateSessionManageDialog::eventFilter(QObject *object, QEvent *event)
{
    QKeyEvent *ke = static_cast<QKeyEvent*>(event);

    if (object == m_sessionList) {
        if (!m_editByUser) { // No need for further action
            return false;
        }

        if (event->type() == QEvent::KeyPress) {
            switch (ke->key()) {
                // Avoid to apply changes with untypical keys/don't left edit field this way
                case Qt::Key_Up :
                case Qt::Key_Down :
                case Qt::Key_PageUp :
                case Qt::Key_PageDown :
                    return true;
                default:
                    break;
            }

        } else if (event->type() == QEvent::KeyRelease) {
            switch (ke->key()) {
                case Qt::Key_Escape :
                    editDone(); // Abort edit
                    break;
                case Qt::Key_Return :
                    editApply();
                    break;
                default:
                    break;
            }
        }

    } else if (object == m_filterBox) {
        // Catch Return key to avoid to finish the dialog
        if (event->type() == QEvent::KeyPress && (ke->key() == Qt::Key_Return || ke->key() == Qt::Key_Enter)) {
            updateSessionList();
            m_sessionList->setFocus();
            return true;
        }
    }

    return false;
}


void KateSessionManageDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);

    if (m_editByUser) {
        // We must catch closeEvent here due to connected signal of QLineEdit::destroyed->editApply()->crash!
        editDone(); // editApply() don't work, m_editByUser->text(0) will not updated from QLineEdit
    }
}
