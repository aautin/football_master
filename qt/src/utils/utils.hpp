#pragma once

#include <QString>
#include <QLineSeries>

void			debug(const QString& text, bool _debug);
bool			isDebug(char** envp);
QLineSeries*	createLineSeries(const QList<QPointF>& points, const QString& name);