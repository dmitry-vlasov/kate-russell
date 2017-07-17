/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_ErrorParser.cpp                      */
/* Description:     an error parser for a Russell support plugin for Kate    */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#include "ErrorParser.hpp"

namespace plugin {
namespace kate {
namespace russell {

	/****************************
	 *	Public members
	 ****************************/

	ErrorParser :: ErrorParser (View* view):
	view_ (view),
	//filenameDetector_ ("([a-np-zA-Z]:[\\\\/])?[a-zA-Z0-9_\\.\\-/\\\\]+\\.[a-zA-Z0-9]+:[0-9]+"),
	numErrors_ (0),
	numWarnings_ (0) {
	}
	ErrorParser :: ~ ErrorParser() {
	}

	void 
	ErrorParser ::  clear()
	{
		numErrors_ = 0;
		numWarnings_ = 0;
	}

	void 
	ErrorParser :: proceed (const QString&)
	{
	}

	int 
	ErrorParser :: numErrors() const {
		return numErrors_;
	}
	int 
	ErrorParser :: numWarnings() const {
		return numWarnings_;
	}


	/****************************
	 *	Private members
	 ****************************/

	/*void 
	View :: processLine (const QString &line)
	{

	}
	void 
	View :: addError 
	(
		const QString &fileconfigName_, 
		const QString &line,
		const QString &column, 
		const QString &message
	)
	{
		QTreeWidgetItem* item = new QTreeWidgetItem (proveUi_.errTreeWidget);
		item->setBackground(1, Qt::gray);
		// The strings are twice in case kate is translated but not make.
		if (message.contains("error") ||
		message.contains(i18nc("The same word as 'make' uses to mark an error.","error")) ||
		message.contains("undefined reference") ||
		message.contains(i18nc("The same word as 'ld' uses to mark an ...","undefined reference")))
		{
			item->setForeground(1, Qt::red);
			numErrors_++;
		}
		if (message.contains("warning") ||
		message.contains(i18nc("The same word as 'make' uses to mark a warning.","warning")))
		{
			item->setForeground(1, Qt::yellow);
			numWarnings_++;
		}
		item->setTextAlignment(1, Qt::AlignRight);

		// visible text
		//remove path from visible file configName_
		KUrl file (fileconfigName_);
		item->setText (0, file.fileName());
		item->setText (1, line);
		item->setText (2, message.trimmed());

		// used to read from when activating an item
		item->setData (0, Qt :: UserRole, fileconfigName_);
		item->setData (1, Qt :: UserRole, line);
		item->setData (2, Qt :: UserRole, column);

		// add tooltips in all columns
		// The enclosing <qt>...</qt> enables word-wrap for long error messages
		item->setData (0, Qt :: ToolTipRole, fileconfigName_);
		item->setData (1, Qt :: ToolTipRole, "<qt>" + message + "</qt>");
		item->setData (2, Qt :: ToolTipRole, "<qt>" + message + "</qt>");
	}*/
}
}
}
