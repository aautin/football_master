#include "Analyzer.hpp"

#include <string>
#include <QDebug>

// ---------- Public slots ----------
void Analyzer::analyze(const QString& teamName, const QList<Match>& matches) {

	QList<QList<QPointF>> points = { QList<QPointF>(), QList<QPointF>() };
	for (int i = 0; i < matches.size(); ++i) {
		const Match& match = matches[i];
		if (match.team1_name == teamName) {
			points[0].append(QPointF(i + 1, match.score1));
			points[1].append(QPointF(i + 1, match.score2));
		}
		else if (match.team2_name == teamName) {
			points[0].append(QPointF(i + 1, match.score2));
			points[1].append(QPointF(i + 1, match.score1));
		}
	}

	emit analyzed(teamName, points);
}