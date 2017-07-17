/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_x_include.cpp                        */
/* Description:     include for all libraries used in Russell plugin         */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#ifndef PLUGIN_KATE_RUSSELL_X_INCLUDE_HPP_
#define PLUGIN_KATE_RUSSELL_X_INCLUDE_HPP_

//#include <sys/socket.h>

#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <unistd.h>

#include <QString>
#include <QStack>
#include <QPixmap>
#include <QDialog>
#include <QIcon>
#include <QFlags>
#include <QMenu>
#include <QResizeEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QList>
#include <QListWidget>
#include <QListWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QApplication>
#include <QCompleter>
#include <QDirModel>
#include <QScrollBar>
#include <QDirModel>
#include <QSizePolicy>
#include <QTextEdit>
#include <QLineEdit>
#include <QProcess>

#include <QComboBox>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <QWidget>

#include <qevent.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qinputdialog.h>
#include <qregexp.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qpalette.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qaction.h>
#include <qdebug.h>
#include <qlocale.h>

#include <kprocess.h>
#include <klineedit.h>
#include <kcombobox.h>
#include <kactioncollection.h>
#include <kcursor.h>
#include <kmessagebox.h>

#include <QStandardPaths>

#include <KConfig>
#include <kpluginfactory.h>
#include <kpluginloader.h>
#include <kaboutdata.h>
#include <kstringhandler.h>

#include <ktexteditor/view.h>
#include <ktexteditor/document.h>

#include <KXMLGUIClient>
#include <KActionMenu>
#include <KConfigGroup>
#include <QFileDialog>

#include <KTextEditor/Plugin>
#include <KTextEditor/MainWindow>
#include <KTextEditor/View>
#include <KTextEditor/SessionConfigInterface>

#endif /* PLUGIN_KATE_RUSSELL_X_INCLUDE_HPP_ */

