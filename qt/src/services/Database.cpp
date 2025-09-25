#include <QSqlQuery>

#include "Database.hpp"

// ---------- Public slots ----------
void Database::initialize(QString host, QString user, QString password, QString dbname, int port) {
	db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(host);
	db.setUserName(user);
	db.setPassword(password);
	db.setDatabaseName(dbname);
	db.setPort(port);

	update();

	emit initialized();
}

void Database::update() {
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

	if (query.exec("SELECT last_update_date FROM app_settings") && query.next())
		lastUpdate = query.value(0).toDateTime();
	
	emit updated();
}

void Database::fetch() {
	emit fetched(competitions, teams, matches, lastUpdate);
}

void Database::destroy() {
	db.close();
	QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);

	emit destroyed();
}

void Database::fetchName(int id) {
	for (const Competition& comp : competitions) {
		if (comp.id == id) {
			emit nameFetched(comp.name);
			return;
		}
	}

	for (const Team& team : teams) {
		if (team.id == id) {
			emit nameFetched(team.name);
			return;
		}
	}
}