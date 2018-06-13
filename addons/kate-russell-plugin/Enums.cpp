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
			task.state = State::PROVING;
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
				} else if (what == QLatin1String("loc")) {
					task.state = State::LOOKING_DEFINITION;
				} else {
					QMessageBox::warning(nullptr, QLatin1String("lookup 'what' is illegal"), what);
				}

			} else {
				QMessageBox::warning(nullptr, QLatin1String("lookup doesn't have 'what' parameter"), command);
			}
		}
	}
	return task;
}

}
