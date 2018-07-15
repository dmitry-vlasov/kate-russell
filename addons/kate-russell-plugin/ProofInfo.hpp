#pragma once

#include <QString>

struct PropInfo {
	QString name;
	bool hint = false;
	int index;
	QString assertion;
	QString substitution;
	QVector<int> premises;
	int parent;
};

struct HypInfo {
	QString expression;
	int index;
	QVector<int> variants;
	int parent;
};
