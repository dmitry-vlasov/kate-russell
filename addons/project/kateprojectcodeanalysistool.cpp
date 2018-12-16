/*  This file is part of the Kate project.
 *
 *  Copyright (C) 2017 Héctor Mesa Jiménez <hector@lcc.uma.es>
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

#include "kateprojectcodeanalysistool.h"

KateProjectCodeAnalysisTool::KateProjectCodeAnalysisTool(QObject *parent)
    : QObject(parent)
    , m_project(nullptr)
{
}

KateProjectCodeAnalysisTool::~KateProjectCodeAnalysisTool()
{
}

void KateProjectCodeAnalysisTool::setProject(KateProject *project)
{
    m_project = project;
}

bool KateProjectCodeAnalysisTool::isSuccessfulExitCode(int exitCode) const
{
    return exitCode == 0;
}

int KateProjectCodeAnalysisTool::getActualFilesCount() const
{
    return m_filesCount;
}

void KateProjectCodeAnalysisTool::setActualFilesCount(int count)
{
    m_filesCount = count;
}
