/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_LatexToUnicode.hpp                   */
/* Description:     translator from latex to unicode                         */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        Copyright (c) 2010 Sven Kreiss <sk@svenkreiss.com>,      */ 
/* Based on:        Copyright (c) 2010 Kyle Cranmer <kyle.cranmer@nyu.edu>   */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#pragma once

#include <QString>

namespace plugin {
namespace kate {
namespace russell {

class LatexToUnicode {
public:
	LatexToUnicode (const QString&);
	virtual ~ LatexToUnicode();

	const QString& translate();

private :
	class Font_;
	
	void translateFont (const Font_&);
	QString translateFont (const QChar, const Font_&);

	const QString& latexString_;
	QString  unicodeString_;

	struct Pair_ {
		Pair_ (const char*, const char*);
		const QString source_;
		const QString image_;
	};
	struct Exception_ {
		Exception_ (const char, const char*);
		const char    char_;
		const QString image_;
	};
	struct Font_ {
		Font_ (const QString&, const uint, const uint/*, const int, const Exception_ []*/);
		const QString keyword_;
		const uint lowerShift_;
		const uint upperShift_;
		//const int exceptionCount_;
		//const Exception_* exceptions_;
	};
	enum {
		FONT_COUNT = 4
	};

	static const Font_ fonts_ [FONT_COUNT];
	static const Exception_ exceptions_[] [FONT_COUNT];

	static const int replacementsCount_;
	static const int combiningMarksCount_;
	static const int subSuperScriptsCount_;

	static const Pair_ replacements_ [];
	static const Pair_ combiningMarks_ [];
	static const Pair_ subSuperScripts_ [];
};

}
}
}
