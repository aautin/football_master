#include <QMessageBox>
#include <QValueAxis>
#include <QRegularExpression>

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

QColor qcolorFromRgbaString(const QString& rgba) {
	QRegularExpression re("rgba\\s*\\(\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*(\\d+)\\s*,\\s*([\\d\\.]+)\\s*\\)");
	auto match = re.match(rgba);
	if (match.hasMatch()) {
		int r = match.captured(1).toInt();
		int g = match.captured(2).toInt();
		int b = match.captured(3).toInt();
		double a = match.captured(4).toDouble();
		int alpha = static_cast<int>(a * 255);
		return QColor(r, g, b, alpha);
	}
	// fallback: return black if parsing fails
	return QColor(0, 0, 0, 255);
}