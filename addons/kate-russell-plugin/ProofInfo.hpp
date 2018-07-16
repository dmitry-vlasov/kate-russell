#pragma once

#include <QString>

struct NodeInfo {
	bool hint = false;
	int index;
	int parent;
};

struct PropInfo : public NodeInfo {
	QString name;
	QString assertion;
	QString substitution;
	QVector<int> premises;
};

struct HypInfo : public NodeInfo {
	QString expression;
	QString types;
	QVector<int> variants;
};
