#include <QMessageBox>
#include <QValueAxis>

#include "utils.hpp"

void debug(const QString& text, bool _debug) {
	if (!_debug)
		return;

	QMessageBox msgBox;
	msgBox.setText(text);
	msgBox.exec();
}

bool isDebug(char** envp) {
	while (*envp) {
		if (std::string(*envp) == "DEBUG_FOOTBALL_MASTER=TRUE")
			return true;
		envp++;
	}
	return false;
}

QLineSeries* createLineSeries(const QList<QPointF>& points, const QString& name, const int lineSize, const QColor color) {
	auto* series = new QLineSeries();
	series->setName(name);
	for (const QPointF& pt : points) {
		series->append(pt);
		series->setPointsVisible(true);
	}
	series->setPen(QPen(color, lineSize));
	return series;
}

void clearChart(QChartView* chartView) {
	chartView->chart()->removeAllSeries();
	chartView->chart()->setTitle("");
	while (chartView->chart()->axes(Qt::Horizontal).size() > 0)
		chartView->chart()->removeAxis(chartView->chart()->axes(Qt::Horizontal).first());
	while (chartView->chart()->axes(Qt::Vertical).size() > 0)
		chartView->chart()->removeAxis(chartView->chart()->axes(Qt::Vertical).first());
}