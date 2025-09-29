#pragma once

#include <QString>

struct Competition {
	int		id;
	QString	name;
};

struct Team {
	int		id;
	int		competition_id;
	QString	name;
};

struct Match {
	int	id;

	int		competition_id;
	QString	competition_name;

	int		team1_id;
	int		team2_id;
	QString	team1_name;
	QString	team2_name;

	int	score1;
	int	score2;

	int	possession1;
	int	possession2;

	int	passes1;
	int	passes2;

	int	successful_passes1;
	int	successful_passes2;

	int	shots1;
	int	shots2;

	int	shots_on_target1;
	int	shots_on_target2;
};

QStringList	extractNames(const QList<Competition> &items);
QStringList	extractNames(const QList<Team> &items);