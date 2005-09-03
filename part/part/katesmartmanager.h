/* This file is part of the KDE libraries
   Copyright (C) 2005 Hamish Rodda <rodda@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KATESMARTMANAGER_H
#define KATESMARTMANAGER_H

#include <QObject>
#include <QSet>
#include <QLinkedList>

#include <ktexteditor/range.h>

#include "kateedit.h"

class KateDocument;
class KateSmartCursor;
class KateSmartRange;
class KateSmartGroup;

/**
 * Manages SmartCursors and SmartRanges.
 */
class KateSmartManager : public QObject
{
  Q_OBJECT

  public:
    KateSmartManager(KateDocument* parent);
    virtual ~KateSmartManager();

    KateDocument* doc() const;

    KTextEditor::SmartCursor* newSmartCursor(const KTextEditor::Cursor& position, bool moveOnInsert = true);
    KTextEditor::SmartRange* newSmartRange(const KTextEditor::Range& range, KTextEditor::SmartRange* parent = 0L, int insertBehaviour = KTextEditor::SmartRange::DoNotExpand);

    void requestFeedback(KateSmartRange* range, int previousLevelOfFeedback);

    KateSmartGroup* groupForLine(int line) const;

  private slots:
    void slotTextChanged(KateEditInfo* edit);

  private:
    void addRangeWantingMostSpecificContentFeedback(KateSmartRange* range);
    void removeRangeWantingMostSpecificContentFeedback(KateSmartRange* range);
    void addPositionRange(KateSmartRange* range);
    void removePositionRange(KateSmartRange* range);

    const QSet<KateSmartRange*>& rangesWantingMostSpecificContentFeedback() const;

    void debugOutput() const;

    KateSmartGroup* m_firstGroup;
    QSet<KateSmartRange*> m_specificRanges;
    KateSmartGroup* m_invalidGroup;
};

/**
 * This class holds a ground of cursors and ranges which involve a certain
 * number of lines in a document.  It allows us to optimise away having to
 * iterate though every single cursor and range when anything changes in the
 * document.
 *
 * Needs a comprehensive regression and performance test suite...
 */
class KateSmartGroup
{
  public:
    KateSmartGroup(int startLine, int endLine, KateSmartGroup* previous, KateSmartGroup* next);

    inline int startLine() const { return m_startLine; }
    inline int newStartLine() const { return m_newStartLine; }
    inline int endLine() const { return m_endLine; }
    inline void setEndLine(int endLine) { m_endLine = endLine; }
    inline int length() const { return m_endLine - m_startLine + 1; }
    inline bool containsLine(int line) const { return line >= m_startLine && line <= m_endLine; }

    inline KateSmartGroup* previous() const { return m_previous; }
    inline void setPrevious(KateSmartGroup* previous) { m_previous = previous; }

    inline KateSmartGroup* next() const { return m_next; }
    inline void setNext(KateSmartGroup* next) { m_next = next; }

    void addCursor(KateSmartCursor* cursor);
    void changeCursorFeedback(KateSmartCursor* cursor);
    void removeCursor(KateSmartCursor* cursor);
    // Cursors requiring position change feedback
    const QSet<KateSmartCursor*>& feedbackCursors() const;
    // Cursors not requiring feedback
    const QSet<KateSmartCursor*>& normalCursors() const;

    // Cursor movement!
    /**
     * A cursor has joined this group.
     *
     * The cursor already has its new position.
     */
    void joined(KateSmartCursor* cursor);

    /**
     * A cursor is leaving this group.
     *
     * The cursor still has its old position.
     */
    void leaving(KateSmartCursor* cursor);

    // Ranges traversing this group that need notification of content changes
    void addTraversingRange(KateSmartRange* range);
    void removeTraversingRange(KateSmartRange* range);
    const QSet<KateSmartRange*>& rangesTraversing() const;

    // Ranges starting in this group that need notification of position changes
    void addStartingPositionRange(KateSmartRange* range);
    void removeStartingPositionRange(KateSmartRange* range);
    const QSet<KateSmartRange*>& rangesStaringPosition() const;

    // Merge with the next Smart Group, because this group became too small.
    void merge();

    // First pass for translation
    void translateChanged(const KateEditInfo& edit, QLinkedList<KateSmartRange*>& m_ranges, bool first);
    void translateShifted(const KateEditInfo& edit);

    void translated(const KateEditInfo& edit);

    void debugOutput() const;

  private:
    int m_startLine, m_newStartLine, m_endLine;
    KateSmartGroup* m_next;
    KateSmartGroup* m_previous;

    QSet<KateSmartCursor*> m_feedbackCursors;
    QSet<KateSmartCursor*> m_normalCursors;
    QSet<KateSmartRange*> m_rangesTraversing;
    QSet<KateSmartRange*> m_rangesStartingPosition;
};

#endif
