#pragma once

#include <QString>
#include <QSqlDatabase>
#include <QList>
#include <QQueue>
#include <QDateTime>

#include "Data.hpp"

class Database : public QObject {
	Q_OBJECT

	public slots:
		void	initialize(QString host, QString user, QString password, QString dbname, int port);
		void	update();

		void	fetchCompetitions();
		void	fetchTeams(const QString &competition_name = "None");
		// void	fetchMatches(const QString &competition_name = "None", const QString &team1_name = "None", const QString &team2_name = "None");
		void	fetchLastUpdateDate();

		void	destroy();

	signals:
		void	initialized();
		void	updated();

		void	competitionsFetched(const QList<Competition>&);
		void	teamsFetched(const QList<Team>&);
		void	matchesFetched(const QList<Match>&);
		void	lastUpdateDateFetched(const QDateTime&);

		void	destroyed();

	private:
		int		toId(const QString &name);
		QString	toName(const int id);

		QSqlDatabase		db;

		QList<Competition>	competitions;
		QList<Team>			teams;
		QList<Match>		matches;
		QDateTime			lastUpdate;

		bool							isInitialized = false;
		QQueue<std::function<void()>>	pendingSlots;
};