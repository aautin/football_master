#pragma once

#include <QString>
#include <QObject>
#include <QPointF>
#include <QList>
#include <QQueue>

#include "data.hpp"

class Database;
class Analyzer : public QObject {
	Q_OBJECT

	public slots:
		void	analyze(const QString& teamName, const QList<Match>& matches);

	signals:
		void	analyzed(const QString& teamName, const QList<QList<QPointF>>& points);
};