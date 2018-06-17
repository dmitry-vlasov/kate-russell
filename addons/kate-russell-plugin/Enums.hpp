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
#include <QMap>

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
	PROVING_INTERACTIVELY,
	PROVING_AUTOMATICALLY,
	TRANSLATING,
	VERIFYING,
	LEARNING,
	LOOKING_DEFINITION,
	OPENING_DEFINITION,
	MINING_OUTLINE,
	MINING_STRUCTURE,
	MINING_TYPE_SYSTEM
};

enum class Lang { RUS, MM, OTHER };

inline Lang file_type(const QString& file) {
	if (file.endsWith(QLatin1String(".rus"))) return Lang::RUS;
	if (file.endsWith(QLatin1String(".mm")))  return Lang::MM;
	return Lang::OTHER;
}

inline QString trim_ext(const QString& file) {
	switch (file_type(file)) {
	case Lang::RUS: return file.mid(0, file.length() - 4);
	case Lang::MM:  return file.mid(0, file.length() - 3);
	default:        return file;
	}
}

inline QString lang_string(Lang lang) {
	switch (lang) {
	case Lang::RUS: return QLatin1String("rus");
	case Lang::MM:  return QLatin1String("mm");
	default:        return QLatin1String("");
	}
}

inline QString change_file_lang(const QString& file, Lang lang) {
	return trim_ext(file) + QLatin1String(".") + lang_string(lang);
}

enum class ActionScope { PROJ, FILE };
enum class ProvingMode { AUTO, INTERACTIVE };
enum class Program { MDL, METAMATH };

struct Task {
	Task(): lang(Lang::OTHER), state(State::WAITING), program(Program::MDL) { }
	Lang    lang;
	State   state;
	Program program;
	QString action;
	QMap<QString, QString> options;
};

Task parse_task(const QString& command);

}

