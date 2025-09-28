#include <QStringList>

#include "data.hpp"

QStringList extractNames(const QList<Competition> &items) {
	QStringList names;
	for (const Competition& item : items)
		names << item.name;
	return names;
}

QStringList extractNames(const QList<Team> &items) {
	QStringList names;
	for (const Team& item : items)
		names << item.name;
	return names;
}