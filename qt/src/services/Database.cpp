#include <stdexcept>

#include "Database.hpp"

// ---------- Constructors Destructors ----------
Database::Database(QString host, QString user, QString password, QString dbname, int port)
{
	db = QSqlDatabase::addDatabase("QPSQL", "football-master-db");
	
	db.setHostName(host);
	db.setUserName(user);
	db.setPassword(password);
	db.setDatabaseName(dbname);
	db.setPort(port);
	
	if (!db.open())
		throw std::runtime_error("Failed to connect to database");

	setTable<Competition>();
	setTable<Team>();
	setTable<Match>();
}

Database::~Database()
{
	db.close();
	QSqlDatabase::removeDatabase(db.connectionName());
}


// ---------- Public Methods ----------
QString Database::getName(int id) const {
    for (const Competition& comp : competitions) {
        if (comp.id == id) return comp.name;
    }
    
	for (const Team& team : teams) {
		if (team.id == id) return team.name;
	}
	return "Unknown";
}