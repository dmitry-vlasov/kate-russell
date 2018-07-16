/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Proof.hpp                            */
/* Description:     an proof view for a Russell support                      */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2003 by Massimo Callegari <massimocallegari@yahoo.it>*/
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#pragma once

#include <QWidget>
#include <QPoint>
#include <QTreeWidget>
#include <QTableWidget>
#include <QDomNode>
#include <QMenu>
#include <QAction>

#include <ktexteditor/mainwindow.h>

#include "Enums.hpp"
#include "ProofInfo.hpp"
#include "ui_HypNode.h"

namespace russell {

class HypNode : public QWidget {
Q_OBJECT
public:
	HypNode ();
	virtual ~ HypNode();

	Ui::HypNode ui_;
	QVector<PropInfo> propInfoVector;
};

}
