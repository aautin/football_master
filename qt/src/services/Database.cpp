#include <QSqlQuery>

#include "Database.hpp"

// ---------- Public slots ----------
void Database::initialize(QString host, QString user, QString password, QString dbname, int port) {
	if (isInitialized) return;

	db = QSqlDatabase::addDatabase("QPSQL", "Database");
	db.setHostName(host);
	db.setUserName(user);
	db.setPassword(password);
	db.setPort(port);
	db.setDatabaseName(dbname);
	db.open();
	
	isInitialized = true;
	update();

	for (const auto& slot : pendingSlots) slot();
	pendingSlots.clear();

	emit initialized();
}

void Database::update() {
	if (!isInitialized) {
		pendingSlots.enqueue([this]() { update(); });
		return;
	}

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

void Database::fetchCompetitions() {
	if (!isInitialized) {
		pendingSlots.enqueue([this]() { fetchCompetitions(); });
		return;
	}

	emit competitionsFetched(competitions);
}

void Database::fetchTeams(const QString &competition_name) {
	if (!isInitialized) {
		pendingSlots.enqueue([this, competition_name]() { fetchTeams(competition_name); });
		return;
	}

	if (competition_name == "None") {
		emit teamsFetched(teams);
		return;
	}

	QList<Team> filteredTeams;
	for (const Team& team : teams) {
		if (toName(team.competition_id) == competition_name) {
			filteredTeams.append(team);
		}
	}
	emit teamsFetched(filteredTeams);
}

// void Database::fetchMatches(const QString &competition_name, const QString &team1_name, const QString &team2_name) {
// 	if (!isInitialized) {
// 		pendingSlots.enqueue([this, competition_name, team1_name, team2_name]() { fetchMatches(competition_name, team1_name, team2_name); });
// 		return;
// 	}

// 	QList<Match> filteredMatches;
// 	for (const Match& match : matches) {
// 		if ((competition_name == "None" || toName(match.competition_id) == competition_name)
// 			&& (team1_name == "None" || toName(match.team1_id) == team1_name)
// 			&& (team2_name == "None" || toName(match.team2_id) == team2_name))
// 			filteredMatches.append(match);
// 	}
// 	emit matchesFetched(filteredMatches);
// }

void Database::fetchLastUpdateDate() {
	if (!isInitialized) {
		pendingSlots.enqueue([this]() { fetchLastUpdateDate(); });
		return;
	}

	emit lastUpdateDateFetched(lastUpdate);
}

void Database::destroy() {
	if (!isInitialized) {
		pendingSlots.enqueue([this]() { destroy(); });
		return;
	}
	
	isInitialized = false;

	competitions.clear();
	teams.clear();
	matches.clear();
	pendingSlots.clear();
	db.close();
	QSqlDatabase::removeDatabase(db.connectionName());

	emit destroyed();
}


// ---------- Private methods ----------
int Database::toId(const QString &name) {
	for (const Competition& comp : competitions)
		if (comp.name == name) return comp.id;

	for (const Team& team : teams)
		if (team.name == name) return team.id;

	return -1;
}

QString Database::toName(const int id) {
	for (const Competition& comp : competitions)
		if (comp.id == id) return comp.name;

	for (const Team& team : teams)
		if (team.id == id) return team.name;

	return QString();
}