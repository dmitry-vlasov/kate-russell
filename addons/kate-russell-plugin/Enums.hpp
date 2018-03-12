/*****************************************************************************/
/* Project name:    Kate plugin for Russell prover integration               */
/* File Name:       plugin_kate_russell_Kind.dpp                             */
/* Description:     enum for kinds of structures in source files             */
/* Copyright:       (c) 2011 Dmitri Vlasov                                   */
/* Author:          Dmitri Yurievich Vlasov, Novosibirsk, Russia             */
/* Email:           vlasov at academ.org                                     */
/* Based on:        (C) 2006-2011 by Kåre Särs <kare.sars@iki.fi>            */
/* Based on:        (C) 2011 by Ian Wakeling <ian.wakeling@ntlworld.com>     */
/* URL:             http://mathdevlanguage.sourceforge.net                   */
/* Modified by:                                                              */
/* License:         GNU General Public License Version 3                     */
/*****************************************************************************/

#pragma once

#include <QString>

namespace russell {

enum Kind {
	SOURCE,
	IMPORT,
	THEORY,
	CONTENTS,
	CONSTANT,
	RULE,
	AXIOM,
	DEFINITION,
	THEOREM,
	PROBLEM,
	PROOF,
	TYPE
};

enum class State {
	WAITING,
	READING,
	PROVING,
	PROVING_INTERACTIVELY,
	TRANSLATING,
	VERIFYING,
	LEARNING,
	LOOKING_DEFINITION,
	OPENING_DEFINITION,
	MINING_OUTLINE,
	MINING_STRUCTURE,
	MINING_TYPE_SYSTEM
};

enum class Lang { RUS, SMM, MM, OTHER };

inline Lang file_type(const QString& file) {
	if (file.endsWith(QStringLiteral(".rus"))) return Lang::RUS;
	if (file.endsWith(QStringLiteral(".smm"))) return Lang::SMM;
	if (file.endsWith(QStringLiteral(".mm")))  return Lang::MM;
	return Lang::OTHER;
}

inline QString trim_ext(const QString& file) {
	switch (file_type(file)) {
	case Lang::RUS: return file.mid(0, file.length() - 4);
	case Lang::SMM: return file.mid(0, file.length() - 4);
	case Lang::MM:  return file.mid(0, file.length() - 3);
	default:        return file;
	}
}

inline QString lang_string(Lang lang) {
	switch (lang) {
	case Lang::RUS: return QStringLiteral("rus");
	case Lang::SMM: return QStringLiteral("smm");
	case Lang::MM:  return QStringLiteral("mm");
	default:        return QStringLiteral("");
	}
}

inline QString change_file_lang(const QString& file, Lang lang) {
	return trim_ext(file) + QStringLiteral(".") + lang_string(lang);
}

enum class ActionScope { PROJ, FILE };
enum class ProvingMode { AUTO, INTERACTIVE };

}

