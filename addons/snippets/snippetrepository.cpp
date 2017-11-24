/*  This file is part of the Kate project.
 *  Based on the snippet plugin from KDevelop 4.
 *
 *  Copyright (C) 2007 Robert Gruber <rgruber@users.sourceforge.net> 
 *  Copyright (C) 2010 Milian Wolff <mail@milianw.de>
 *  Copyright (C) 2012 Christoph Cullmann <cullmann@kde.org>
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

#include "snippetrepository.h"

#include "snippet.h"

#include <QTimer>
#include <QFile>
#include <QFileInfo>
#include <QAction>

#include <QDomDocument>
#include <QDomElement>

#include <KMessageBox>
#include <KLocalizedString>
#include <QApplication>
#include <QDebug>
#include <QDir>

#include <KColorScheme>

#include <KUser>

#include "snippetstore.h"

static const QString defaultScript = QStringLiteral("\
function fileName() { return document.fileName(); }\n\
function fileUrl() { return document.url(); }\n\
function encoding() { return document.encoding(); }\n\
function selection() { return view.selectedText(); }\n\
function year() { return new Date().getFullYear(); }\n\
function upper(x) { return x.toUpperCase(); }\n\
function lower(x) { return x.toLowerCase(); }\n"
);

SnippetRepository::SnippetRepository(const QString& file)
 : QStandardItem(i18n("<empty repository>")), m_file(file), m_script(defaultScript)
{
    setIcon( QIcon::fromTheme(QLatin1String("folder")) );
    const auto& config = SnippetStore::self()->getConfig();
    bool activated = config.readEntry<QStringList>("enabledRepositories", QStringList()).contains(file);
    setCheckState(activated ? Qt::Checked : Qt::Unchecked);

    if ( QFile::exists(file) ) {
        // Tell the new repository to load it's snippets
        QTimer::singleShot(0, this, SLOT(slotParseFile()));
    }

    qDebug() << "created new snippet repo" << file << this;
}

SnippetRepository::~SnippetRepository()
{
    // remove all our children from both the model and our internal data structures
    removeRows( 0, rowCount() );
}

QDir SnippetRepository::dataPath()
{
    auto dir = QDir(QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation));
    const auto& subdir = QLatin1String("ktexteditor_snippets/data/");
    bool success = dir.mkpath(dir.absoluteFilePath(subdir));
    Q_ASSERT(success);
    dir.setPath(dir.path() + QLatin1String("/") + subdir);
    return dir;
}

SnippetRepository* SnippetRepository::createRepoFromName(const QString& name)
{
    QString cleanName = name;
    cleanName.replace(QLatin1Char('/'), QLatin1Char('-'));

    const auto& dir = dataPath();
    const auto& path = dir.absoluteFilePath(cleanName + QLatin1String(".xml"));
    qDebug() << "repo path:" << path << cleanName;

    SnippetRepository* repo = new SnippetRepository(path);
    repo->setText(name);
    repo->setCheckState(Qt::Checked);
    KUser user;
    repo->setAuthors(user.property(KUser::FullName).toString());
    SnippetStore::self()->appendRow(repo);
    return repo;
}

const QString& SnippetRepository::file() const
{
    return m_file;
}

QString SnippetRepository::authors() const
{
    return m_authors;
}

void SnippetRepository::setAuthors(const QString& authors)
{
    m_authors = authors;
}

QStringList SnippetRepository::fileTypes() const
{
    return m_filetypes;
}

void SnippetRepository::setFileTypes(const QStringList& filetypes)
{
    if ( filetypes.contains(QLatin1String("*")) ) {
        m_filetypes.clear();
    } else {
        m_filetypes = filetypes;
    }
}

QString SnippetRepository::license() const
{
    return m_license;
}

void SnippetRepository::setLicense(const QString& license)
{
    m_license = license;
}

QString SnippetRepository::completionNamespace() const
{
    return m_namespace;
}

void SnippetRepository::setCompletionNamespace(const QString& completionNamespace)
{
    m_namespace = completionNamespace;
}

QString SnippetRepository::script() const
{
    return m_script;
}

void SnippetRepository::setScript(const QString& script)
{
    m_script = script;
}

void SnippetRepository::remove()
{
    QFile::remove(m_file);
    setCheckState(Qt::Unchecked);
    model()->invisibleRootItem()->removeRow(row());
}

///copied code from snippets_tng/lib/completionmodel.cpp
///@copyright 2009 Joseph Wenninger <jowenn@kde.org>
static void addAndCreateElement(QDomDocument& doc, QDomElement& item, const QString& name, const QString &content)
{
    QDomElement element=doc.createElement(name);
    element.appendChild(doc.createTextNode(content));
    item.appendChild(element);
}

void SnippetRepository::save()
{
    qDebug() << "*** called";
    ///based on the code from snippets_tng/lib/completionmodel.cpp
    ///@copyright 2009 Joseph Wenninger <jowenn@kde.org>
    /*
    <snippets name="Testsnippets" filetype="*" authors="Joseph Wenninger" license="BSD" namespace="test::">
        <script>
            JavaScript
        </script>
        <item>
            <displayprefix>prefix</displayprefix>
            <match>test1</match>
            <displaypostfix>postfix</displaypostfix>
            <displayarguments>(param1, param2)</displayarguments>
            <fillin>This is a test</fillin>
        </item>
        <item>
            <match>testtemplate</match>
            <fillin>This is a test ${WHAT} template</fillin>
        </item>
    </snippets>
    */
    QDomDocument doc;

    QDomElement root = doc.createElement(QLatin1String("snippets"));
    root.setAttribute(QLatin1String("name"), text());
    root.setAttribute(QLatin1String("filetypes"), m_filetypes.isEmpty() ? QLatin1String("*") : m_filetypes.join(QLatin1String(";")));
    root.setAttribute(QLatin1String("authors"), m_authors);
    root.setAttribute(QLatin1String("license"), m_license);
    root.setAttribute(QLatin1String("namespace"), m_namespace);

    doc.appendChild(root);

    addAndCreateElement(doc, root, QLatin1String("script"), m_script);

    for ( int i = 0; i < rowCount(); ++i ) {
        Snippet* snippet = dynamic_cast<Snippet*>(child(i));
        if ( !snippet ) {
            continue;
        }
        QDomElement item = doc.createElement(QLatin1String("item"));
        addAndCreateElement(doc, item, QLatin1String("match"), snippet->text());
        addAndCreateElement(doc, item, QLatin1String("fillin"), snippet->snippet());
        root.appendChild(item);
    }
    //KMessageBox::information(0,doc.toString());
    QFileInfo fi(m_file);
    QDir dir = dataPath();
    QString outname = dir.absoluteFilePath(fi.fileName());

    if (m_file != outname) {
        QFileInfo fiout(outname);
        // there could be cases that new new name clashes with a global file, but I guess it is not that often.
        int i = 0;
        while (QFile::exists(outname)) {
            i++;
            outname = dir.absoluteFilePath(QString::number(i) + fi.fileName());
        }
        KMessageBox::information(QApplication::activeWindow(),
            i18n("You have edited a data file not located in your personal data directory; as such, a renamed clone of the original data file has been created within your personal data directory."));
    }

    QFile outfile(outname);
    if (!outfile.open(QIODevice::WriteOnly)) {
        KMessageBox::error(nullptr, i18n("Output file '%1' could not be opened for writing", outname));
        return;
    }
    outfile.write(doc.toByteArray());
    outfile.close();
    m_file = outname;

    // save shortcuts
    KConfigGroup config = SnippetStore::self()->getConfig().group(QLatin1String("repository ") + m_file);
    for ( int i = 0; i < rowCount(); ++i ) {
        Snippet* snippet = dynamic_cast<Snippet*>(child(i));
        if ( !snippet ) {
            continue;
        }

        QStringList shortcuts;

        foreach ( const QKeySequence &keys, snippet->action()->shortcuts() ) {
          shortcuts << keys.toString();
        }

        config.writeEntry( QLatin1String("shortcut ") + snippet->text(), shortcuts );
    }
    config.sync();
}

void SnippetRepository::slotParseFile()
{
    ///based on the code from snippets_tng/lib/completionmodel.cpp
    ///@copyright 2009 Joseph Wenninger <jowenn@kde.org>

    QFile f(m_file);

    if ( !f.open(QIODevice::ReadOnly) ) {
        KMessageBox::error( QApplication::activeWindow(), i18n("Cannot open snippet repository %1.", m_file) );
        return;
    }

    QDomDocument doc;
    QString errorMsg;
    int line, col;
    bool success = doc.setContent(&f, &errorMsg, &line, &col);
    f.close();

    if (!success) {
        KMessageBox::error( QApplication::activeWindow(),
                           i18n("<qt>The error <b>%4</b><br /> has been detected in the file %1 at %2/%3</qt>",
                                m_file, line, col, i18nc("QXml", errorMsg.toUtf8().data())) );
        return;
    }

    // parse root item
    const QDomElement& docElement = doc.documentElement();
    if (docElement.tagName() != QLatin1String("snippets")) {
        KMessageBox::error( QApplication::activeWindow(), i18n("Invalid XML snippet file: %1", m_file) );
        return;
    }
    setLicense(docElement.attribute(QLatin1String("license")));
    setAuthors(docElement.attribute(QLatin1String("authors")));
    setFileTypes(docElement.attribute(QLatin1String("filetypes")).split(QLatin1Char(';'), QString::SkipEmptyParts));
    setText(docElement.attribute(QLatin1String("name")));
    setCompletionNamespace(docElement.attribute(QLatin1String("namespace")));

    // load shortcuts
    KConfigGroup config = SnippetStore::self()->getConfig().group(QLatin1String("repository ") + m_file);

    // parse children, i.e. <item>'s
    const QDomNodeList& nodes = docElement.childNodes();
    for(int i = 0; i < nodes.size(); ++i ) {
        const QDomNode& node = nodes.at(i);
        if ( !node.isElement() ) {
            continue;
        }
        const QDomElement& item = node.toElement();
        if ( item.tagName() == QLatin1String("script") ) {
            setScript(item.text());
        }
        if ( item.tagName() != QLatin1String("item") ) {
            continue;
        }
        Snippet* snippet = new Snippet;
        const QDomNodeList& children = node.childNodes();
        for(int j = 0; j < children.size(); ++j) {
            const QDomNode& childNode = children.at(j);
            if ( !childNode.isElement() ) {
                continue;
            }
            const QDomElement& child = childNode.toElement();
            if ( child.tagName() == QLatin1String("match") ) {
                snippet->setText(child.text());
            } else if ( child.tagName() == QLatin1String("fillin") ) {
                snippet->setSnippet(child.text());
            }
        }
        // require at least a non-empty name and snippet
        if ( snippet->text().isEmpty() || snippet->snippet().isEmpty() ) {
            delete snippet;
            continue;
        } else {
            const QStringList shortcuts = config.readEntry(QLatin1String("shortcut ") + snippet->text(), QStringList());

            QList<QKeySequence> sequences;

            foreach ( const QString &shortcut, shortcuts ) {
              sequences << QKeySequence::fromString( shortcut );
            }

            snippet->action()->setShortcuts( sequences );

            appendRow(snippet);
        }
    }
}

QVariant SnippetRepository::data(int role) const
{
    if ( role == Qt::ToolTipRole ) {
        if ( checkState() != Qt::Checked ) {
            return i18n("Repository is disabled, the contained snippets will not be shown during code-completion.");
        }
        if ( m_filetypes.isEmpty() ) {
            return i18n("Applies to all filetypes");
        } else {
            return i18n("Applies to the following filetypes: %1", m_filetypes.join(QLatin1String(", ")));
        }
    } else if ( role == Qt::ForegroundRole && checkState() != Qt::Checked ) {
        ///TODO: make the selected items also "disalbed" so the toggle action is seen directly
        KColorScheme scheme(QPalette::Disabled, KColorScheme::View);
        QColor c = scheme.foreground(KColorScheme::NormalText).color();
        return QVariant(c);
    }
    return QStandardItem::data(role);
}

void SnippetRepository::setData(const QVariant& value, int role)
{
    if ( role == Qt::CheckStateRole ) {
        const int state = value.toInt();
        if ( state != checkState() ) {
            KConfigGroup config = SnippetStore::self()->getConfig();
            QStringList currentlyEnabled = config.readEntry("enabledRepositories", QStringList());
            bool shouldSave = false;
            if ( state == Qt::Checked && !currentlyEnabled.contains(m_file) ) {
                currentlyEnabled << m_file;
                shouldSave = true;
            } else if ( state == Qt::Unchecked && currentlyEnabled.contains(m_file) ) {
                currentlyEnabled.removeAll(m_file);
                shouldSave = true;
            }

            if ( shouldSave ) {
                config.writeEntry("enabledRepositories", currentlyEnabled);
                config.sync();
            }
        }
    }
    QStandardItem::setData(value, role);
}

