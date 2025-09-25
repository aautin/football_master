#pragma once

#include <QString>
#include <QSqlDatabase>
#include <QList>
#include <QDateTime>

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
	int	competition_id;

	int	team1_id;
	int	team2_id;

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

class Database : public QObject {
	Q_OBJECT

	public slots:
		void	initialize(QString host, QString user, QString password, QString dbname, int port);
		void	update();
		void	fetch();
		void	destroy();
		void	fetchName(int id);

	signals:
		void	initialized();
		void	updated();
		void	fetched(const QList<Competition>&, const QList<Team>&, const QList<Match>&, const QDateTime&);
		void	destroy();
		void	nameFetched(const QString&);

	private:
		QSqlDatabase		db;

		QList<Competition>	competitions;
		QList<Team>			teams;
		QList<Match>		matches;
		QDateTime			lastUpdate;
};