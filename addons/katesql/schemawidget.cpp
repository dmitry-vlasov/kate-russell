/*
   Copyright (C) 2010  Marco Mentasti  <marcomentasti@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "schemawidget.h"
#include "sqlmanager.h"

#include <ktexteditor/editor.h>
#include <ktexteditor/application.h>
#include <ktexteditor/mainwindow.h>
#include <ktexteditor/view.h>
#include <klocalizedstring.h>

#include <qvariant.h>
#include <qstringlist.h>
#include <qevent.h>
#include <qsqldatabase.h>
#include <qsqlrecord.h>
#include <qsqlindex.h>
#include <qsqlfield.h>
#include <qmenu.h>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

SchemaWidget::SchemaWidget(QWidget *parent, SQLManager *manager)
: QTreeWidget(parent)
, m_manager(manager)
{
  m_tablesLoaded = false;
  m_viewsLoaded = false;

  setHeaderLabels(QStringList() << i18nc("@title:column", "Database schema"));

  setContextMenuPolicy(Qt::CustomContextMenu);
  setDragDropMode(QAbstractItemView::DragOnly);
  setDragEnabled(true);
  setAcceptDrops(false);

  connect(this, &SchemaWidget::customContextMenuRequested, this, &SchemaWidget::slotCustomContextMenuRequested);
  connect(this, &SchemaWidget::itemExpanded, this, &SchemaWidget::slotItemExpanded);
}


SchemaWidget::~SchemaWidget()
{
}


bool SchemaWidget::isConnectionValidAndOpen()
{
  return m_manager->isValidAndOpen(m_connectionName);
}


void SchemaWidget::deleteChildren(QTreeWidgetItem *item)
{
  QList<QTreeWidgetItem *> items = item->takeChildren();

  foreach (QTreeWidgetItem *i, items)
    delete i;
}


void SchemaWidget::buildTree(const QString &connection)
{
  m_connectionName = connection;

  clear();

  m_tablesLoaded = false;
  m_viewsLoaded = false;

  if (!m_connectionName.isEmpty())
    buildDatabase(new QTreeWidgetItem(this));
}


void SchemaWidget::refresh()
{
  buildTree(m_connectionName);
}


void SchemaWidget::buildDatabase(QTreeWidgetItem * databaseItem)
{
  QSqlDatabase db = QSqlDatabase::database(m_connectionName);
  QString dbname = (db.isValid() ? db.databaseName() : m_connectionName);

  databaseItem->setText(0, dbname);
  databaseItem->setIcon(0, QIcon::fromTheme(QLatin1String("server-database")));

  QTreeWidgetItem *tablesItem = new QTreeWidgetItem(databaseItem, TablesFolderType);
  tablesItem->setText(0, i18nc("@title Folder name", "Tables"));
  tablesItem->setIcon(0, QIcon::fromTheme(QLatin1String("folder")));
  tablesItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

  QTreeWidgetItem *viewsItem = new QTreeWidgetItem(databaseItem, ViewsFolderType);
  viewsItem->setText(0, i18nc("@title Folder name", "Views"));
  viewsItem->setIcon(0, QIcon::fromTheme(QLatin1String("folder")));
  viewsItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

  databaseItem->setExpanded(true);
}


void SchemaWidget::buildTables(QTreeWidgetItem * tablesItem)
{
  if (!isConnectionValidAndOpen())
    return;

  QTreeWidgetItem *systemTablesItem = new QTreeWidgetItem(tablesItem, SystemTablesFolderType);
  systemTablesItem->setText(0, i18nc("@title Folder name", "System Tables"));
  systemTablesItem->setIcon(0, QIcon::fromTheme(QLatin1String("folder")));
  systemTablesItem->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);

  QSqlDatabase db = QSqlDatabase::database(m_connectionName);
  QStringList tables = db.tables(QSql::SystemTables);

  foreach(const QString& table, tables)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(systemTablesItem, SystemTableType);
    item->setText(0, table);
    item->setIcon(0, QIcon(QLatin1String(":/katesql/pics/16-actions-sql-table.png")));
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
  }

  tables = db.tables(QSql::Tables);

  foreach(const QString& table, tables)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(tablesItem, TableType);
    item->setText(0, table);
    item->setIcon(0, QIcon(QLatin1String(":/katesql/pics/16-actions-sql-table.png")));
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
  }

  m_tablesLoaded = true;
}


void SchemaWidget::buildViews(QTreeWidgetItem * viewsItem)
{
  if (!isConnectionValidAndOpen())
    return;

  QSqlDatabase db = QSqlDatabase::database(m_connectionName);

  const QStringList views = db.tables(QSql::Views);

  foreach(const QString& view, views)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(viewsItem, ViewType);
    item->setText(0, view);
    item->setIcon(0, QIcon(QLatin1String(":/katesql/pics/16-actions-sql-view.png")));
    item->setChildIndicatorPolicy(QTreeWidgetItem::ShowIndicator);
  }

  m_viewsLoaded = true;
}


void SchemaWidget::buildFields(QTreeWidgetItem * tableItem)
{
  if (!isConnectionValidAndOpen())
    return;

  QSqlDatabase db = QSqlDatabase::database(m_connectionName);

  QString tableName = tableItem->text(0);

  QSqlIndex pk = db.primaryIndex(tableName);
  QSqlRecord rec = db.record(tableName);

  for (int i = 0; i < rec.count(); ++i)
  {
    QSqlField f = rec.field(i);

    QString fieldName = f.name();

    QTreeWidgetItem *item = new QTreeWidgetItem(tableItem, FieldType);
    item->setText(0, fieldName);

    if (pk.contains(fieldName))
      item->setIcon(0, QIcon(QLatin1String(":/katesql/pics/16-actions-sql-field-pk.png")));
    else
      item->setIcon(0, QIcon(QLatin1String(":/katesql/pics/16-actions-sql-field.png")));
  }
}


void SchemaWidget::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
    m_dragStartPosition = event->pos();
  QTreeWidget::mousePressEvent(event);
}

void SchemaWidget::mouseMoveEvent(QMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton))
    return;
  if ((event->pos() - m_dragStartPosition).manhattanLength()
    < QApplication::startDragDistance())
    return;

//   QTreeWidgetItem *item = currentItem();
  QTreeWidgetItem *item = itemAt(event->pos());

  if (!item)
    return;

  if (item->type() != SchemaWidget::SystemTableType &&
      item->type() != SchemaWidget::TableType &&
      item->type() != SchemaWidget::ViewType &&
      item->type() != SchemaWidget::FieldType)
    return;

  QDrag *drag = new QDrag(this);
  QMimeData *mimeData = new QMimeData;

  if (item->type() == SchemaWidget::FieldType)
    mimeData->setText(QString::fromLatin1("%1.%2").arg(item->parent()->text(0)).arg(item->text(0)));
  else
    mimeData->setText(item->text(0));

  drag->setMimeData(mimeData);
  drag->exec(Qt::CopyAction);

  QTreeWidget::mouseMoveEvent(event);
}


void SchemaWidget::slotItemExpanded(QTreeWidgetItem *item)
{
  if (!item)
    return;

  switch(item->type())
  {
    case SchemaWidget::TablesFolderType:
    {
      if (!m_tablesLoaded)
        buildTables(item);
    }
    break;

    case SchemaWidget::ViewsFolderType:
    {
      if (!m_viewsLoaded)
        buildViews(item);
    }
    break;

    case SchemaWidget::TableType:
    case SchemaWidget::SystemTableType:
    case SchemaWidget::ViewType:
    {
      if (item->childCount() == 0)
        buildFields(item);
    }
    break;

    default:
    break;
  }
}


void SchemaWidget::slotCustomContextMenuRequested(const QPoint &pos)
{
  QMenu menu;

  menu.addAction(QIcon::fromTheme(QLatin1String("view-refresh")), i18nc("@action:inmenu Context menu", "Refresh"), this, SLOT(refresh()));

  QTreeWidgetItem *item = itemAt(pos);

  if (item)
  {
    if (item->type() == SchemaWidget::SystemTableType
    ||  item->type() == SchemaWidget::TableType
    ||  item->type() == SchemaWidget::ViewType
    ||  item->type() == SchemaWidget::FieldType)
    {
      menu.addSeparator();
      QMenu *submenu = menu.addMenu(QIcon::fromTheme(QLatin1String("tools-wizard")), i18nc("@action:inmenu Submenu title", "Generate"));

      submenu->addAction(i18n ("SELECT"), this, SLOT(generateSelect()));
      submenu->addAction(i18n ("UPDATE"), this, SLOT(generateUpdate()));
      submenu->addAction(i18n ("INSERT"), this, SLOT(generateInsert()));
      submenu->addAction(i18n ("DELETE"), this, SLOT(generateDelete()));
    }
  }

  menu.exec(QCursor::pos());
}


void SchemaWidget::generateStatement(QSqlDriver::StatementType statementType)
{
  if (!isConnectionValidAndOpen())
    return;

  QSqlDatabase db = QSqlDatabase::database(m_connectionName);

  QSqlDriver *drv = db.driver();

  if (!drv)
    return;

  QTreeWidgetItem *item = currentItem();

  if (!item)
    return;

  QString statement;

  switch (item->type())
  {
    case TableType:
    case SystemTableType:
    case ViewType:
    {
      QString tableName = item->text(0);

      QSqlRecord rec = db.record(tableName);

      // set all fields to a value (NULL)
      // values are needed to generate update and insert statements
      if (statementType == QSqlDriver::UpdateStatement || statementType == QSqlDriver::InsertStatement)
        for (int i = 0, n = rec.count(); i < n; ++i)
          rec.setNull(i);

      statement = drv->sqlStatement(statementType, tableName, rec, false);
    }
    break;

    case FieldType:
    {
      QString tableName = item->parent()->text(0);
      QSqlRecord rec = db.record(tableName);

      // get the selected column...
      QSqlField field = rec.field(item->text(0));

      // ...and set its value to NULL
      field.clear();

      // clear all columns and re-append the selected one
      rec.clear();
      rec.append(field);

      statement = drv->sqlStatement(statementType, tableName, rec, false);

      if (statementType == QSqlDriver::DeleteStatement)
        statement += QLatin1String (" ") + drv->sqlStatement(QSqlDriver::WhereStatement, tableName, rec, false).replace(QLatin1String (" IS NULL"), QLatin1String ("=?"));
    }
    break;
  }

  KTextEditor::MainWindow *mw = KTextEditor::Editor::instance()->application()->activeMainWindow();
  KTextEditor::View *kv = mw->activeView();

  // replace NULL with a more generic '?'
  statement = statement.replace(QLatin1String ("NULL"), QLatin1String ("?"));

  if (kv)
  {
    // paste statement in the active view
    kv->insertText(statement);
    kv->setFocus();
  }

  qDebug() << "Generated statement:" << statement;
}


void SchemaWidget::generateSelect()
{
  generateStatement(QSqlDriver::SelectStatement);
}


void SchemaWidget::generateUpdate()
{
  generateStatement(QSqlDriver::UpdateStatement);
}


void SchemaWidget::generateInsert()
{
  generateStatement(QSqlDriver::InsertStatement);
}


void SchemaWidget::generateDelete()
{
  generateStatement(QSqlDriver::DeleteStatement);
}
