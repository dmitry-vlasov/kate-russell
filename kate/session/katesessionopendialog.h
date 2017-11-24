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

#ifndef __KATE_SESSION_OPEN_DIALOG_H__
#define __KATE_SESSION_OPEN_DIALOG_H__

#include <QDialog>

class QPushButton;
class QTreeWidget;
class QTreeWidgetItem;

#include "katesession.h"

class KateSessionOpenDialog : public QDialog
{
    Q_OBJECT

public:
    KateSessionOpenDialog(QWidget *parent);
    ~KateSessionOpenDialog() override;

    KateSession::Ptr selectedSession();

    enum {
        resultOk,
        resultCancel
    };

protected Q_SLOTS:
    void slotCanceled();
    void slotOpen();

    /**
     * selection has changed
     */
    void selectionChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

private:
    QTreeWidget *m_sessions;
    QPushButton *m_openButton;
};

#endif

