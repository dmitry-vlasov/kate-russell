/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_ErrorParser.hpp                      */
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

#ifndef PLUGIN_KATE_RUSSELL_ERROR_PARSER_HPP_
#define PLUGIN_KATE_RUSSELL_ERROR_PARSER_HPP_

#include "plugin_kate_russell.dpp"

namespace plugin {
namespace kate {
namespace russell {

class ErrorParser {
public :
	ErrorParser (View*);
	virtual ~ ErrorParser();

	void clear();
	void proceed (const QString&);

	int numErrors() const;
	int numWarnings() const;

private :
	View*     view_;
	//QRegExp   filenameDetector_;
	int       numErrors_;
	int       numWarnings_;
};

}
}
}

#endif /* PLUGIN_KATE_RUSSELL_ERROR_PARSER_HPP_ */

