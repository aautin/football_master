#pragma once

#include <QString>
#include <QSqlDatabase>
#include <QList>
#include <QSqlQuery>
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

class Database {
	public:
		Database(QString host, QString user, QString password, QString dbname, int port);
		~Database();

		template <typename T> QList<T>	getTable();
		template <typename T> QList<T>	setTable();
		void							updateDate() const;
		QDateTime						getDate() const;
		QString							getName(int id) const;

	private:
		QSqlDatabase	db;

		QList<Competition>	competitions;
		QList<Team>			teams;
		QList<Match>		matches;
};


// ---------- Setters ----------
template<>
inline QList<Competition> Database::setTable() {
	QSqlQuery query(db);
	if (query.exec("SELECT id, name FROM competitions")) {
		competitions.clear();
		while (query.next()) {
			Competition comp;
			comp.id = query.value(0).toInt();
			comp.name = query.value(1).toString();
			competitions.append(comp);
		}
	}
	return competitions;
}

template<>
inline QList<Team> Database::setTable() {
	QSqlQuery query(db);
	if (query.exec("SELECT id, name, competition_id FROM teams")) {
		teams.clear();
		while (query.next()) {
			Team team;
			team.id = query.value(0).toInt();
			team.name = query.value(1).toString();
			team.competition_id = query.value(2).toInt();
			teams.append(team);
		}
	}
	return teams;
}

template<>
inline QList<Match> Database::setTable() {
	QSqlQuery query(db);
	if (query.exec("SELECT id, competition_id, team1_id, team2_id, score1, score2, "
		"possession1, possession2, passes1, passes2, successful_passes1, "
		"successful_passes2, shots1, shots2, shots_on_target1, shots_on_target2 "
		"FROM Matches")) {
		matches.clear();
		while (query.next()) {
			Match match;
			match.id = query.value(0).toInt();
			match.competition_id = query.value(1).toInt();
			match.team1_id = query.value(2).toInt();
			match.team2_id = query.value(3).toInt();
			match.score1 = query.value(4).toInt();
			match.score2 = query.value(5).toInt();
			match.possession1 = query.value(6).toInt();
			match.possession2 = query.value(7).toInt();
			match.passes1 = query.value(8).toInt();
			match.passes2 = query.value(9).toInt();
			match.successful_passes1 = query.value(10).toInt();
			match.successful_passes2 = query.value(11).toInt();
			match.shots1 = query.value(12).toInt();
			match.shots2 = query.value(13).toInt();
			match.shots_on_target1 = query.value(14).toInt();
			match.shots_on_target2 = query.value(15).toInt();
			matches.append(match);
		}
	}
	return matches;
}


// ---------- Getters ----------
template<>
inline QList<Competition> Database::getTable<Competition>() {
	return competitions;
}

template<>
inline QList<Team> Database::getTable<Team>() {
	return teams;
}

template<>
inline QList<Match> Database::getTable<Match>() {
	return matches;
}