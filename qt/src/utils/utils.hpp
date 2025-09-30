#pragma once

#include <QString>
#include <QLineSeries>
#include <QChartView>


void			debug(const QString& text, bool _debug);
bool			isDebug(char** envp);
QLineSeries*	createLineSeries(const QList<QPointF>& points, const QString& name,
	const int lineSize, const QColor color);
void			clearChart(QChartView* chartView);