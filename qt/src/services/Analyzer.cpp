#include "Analyzer.hpp"

#include <string>
#include <QDebug>

// ---------- Public slots ----------
void Analyzer::analyze(const QString& teamName, const QList<Match>& matches) {
	QList<QString> opponents;
	QList<QList<QPointF>> points = {
		QList<QPointF>(), QList<QPointF>(),
		QList<QPointF>(), QList<QPointF>(),
		QList<QPointF>(), QList<QPointF>(),
		QList<QPointF>(), QList<QPointF>()
	};
	for (int i = 0; i < matches.size(); ++i) {
		const Match& match = matches[i];
		if (match.team1_name == teamName) {
			points[0].append(QPointF(i + 1, match.score1));
			points[1].append(QPointF(i + 1, match.score2));
			points[2].append(QPointF(i + 1, match.possession1));
			points[4].append(QPointF(i + 1, match.shots1));
			points[5].append(QPointF(i + 1, match.shots_on_target1));
			points[6].append(QPointF(i + 1, match.passes1));
			points[7].append(QPointF(i + 1, match.successful_passes1));
			opponents.append(match.team2_name);
		}
		else if (match.team2_name == teamName) {
			points[0].append(QPointF(i + 1, match.score2));
			points[1].append(QPointF(i + 1, match.score1));
			points[2].append(QPointF(i + 1, match.possession2));
			points[4].append(QPointF(i + 1, match.shots2));
			points[5].append(QPointF(i + 1, match.shots_on_target2));
			points[6].append(QPointF(i + 1, match.passes2));
			points[7].append(QPointF(i + 1, match.successful_passes2));
			opponents.append(match.team1_name);
		}
	}

	qDebug() << "Analyzer analyzed" << teamName << points << opponents;

	// opponents[0] = "1234";
	emit analyzed(teamName, points, opponents);
}