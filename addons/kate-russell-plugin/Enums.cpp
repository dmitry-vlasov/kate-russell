#include <QMessageBox>

#include "Enums.hpp"

namespace russell {

Task parse_task(const QString& command) {
	QStringList strings = command.split(QLatin1Char(' '));
	Task task;
	if (strings.size()) {
		QString lang = strings.takeFirst();
		if (lang == QLatin1String("mm")) {
			task.lang = Lang::MM;
		} else if (lang == QLatin1String("rus")) {
			task.lang = Lang::RUS;
		} else if (lang == QLatin1String("metamath")) {
			task.lang = Lang::MM;
			task.program = Program::METAMATH;
		}
		if (strings.size()) {
			task.action = strings.takeFirst();
			for (auto s : strings) {
				if (s.size()) {
					auto eq_sides = s.split(QLatin1Char('='));
					if (eq_sides.size() == 1) {
						task.options[s] = QString();
					} else {
						task.options[eq_sides[0]] = eq_sides[1];
					}
				}
			}
			if (task.action == QLatin1String("read")) {
				task.state = State::READING;
			} else if (task.action == QLatin1String("prove")) {
				task.state = State::PROVING_AUTOMATICALLY;
			} else if (task.action == QLatin1String("transl")) {
				task.state = State::TRANSLATING;
			} else if (task.action == QLatin1String("verify")) {
				task.state = State::VERIFYING;
			} else if (task.action == QLatin1String("learn")) {
				task.state = State::LEARNING;
			} else if (task.action == QLatin1String("lookup")) {
				if (task.options.count(QLatin1String("what"))) {
					auto what = task.options[QLatin1String("what")];
					if (what == QLatin1String("loc")) {
						task.state = State::OPENING_DEFINITION;
					} else if (what == QLatin1String("def")) {
						task.state = State::LOOKING_DEFINITION;
					} else {
						QMessageBox::warning(nullptr, QLatin1String("lookup 'what' is illegal"), QLatin1String("lookup 'what' is illegal: ") + what);
					}

				} else {
					QMessageBox::warning(nullptr, QLatin1String("lookup doesn't have 'what' parameter"), QLatin1String("lookup doesn't have 'what' parameter: ") + command);
				}
			} else if (task.action == QLatin1String("struct")) {
				task.state = State::MINING_STRUCTURE;
			} else if (task.action == QLatin1String("outline")) {
				task.state = State::MINING_OUTLINE;
			} else if (task.action == QLatin1String("types")) {
				task.state = State::MINING_TYPE_SYSTEM;
			} else if (task.action.startsWith(QLatin1String("prove_"))) {
				task.state = State::PROVING_INTERACTIVELY;
			}
		}
	}
	return task;
}

}
