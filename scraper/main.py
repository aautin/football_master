import requests
from bs4 import BeautifulSoup
import time

import threading
from flask import Flask, jsonify


import psycopg2
import os
from psycopg2 import sql
from psycopg2.extras import RealDictCursor

app = Flask(__name__)
scraping_status = {"running": False, "last_result": None}

def connect_db():
    try:
        connection = psycopg2.connect(
            host=os.getenv('DB_HOST'),
            port=5432,
            database=os.getenv('DB_NAME'),
            user=os.getenv('DB_USER'),
            password=os.getenv('DB_PASSWORD'),
            cursor_factory=RealDictCursor
        )
        print("✅ Connexion à la base de données réussie")
        return connection
    except psycopg2.Error as e:
        print(f"❌ Erreur de connexion à la base de données: {e}")
        return None

def insert_last_update_date(connection):
	try:
		cursor = connection.cursor()
		cursor.execute("""
			UPDATE app_settings SET last_update_date = NOW();
			INSERT INTO app_settings (last_update_date)
			SELECT NOW()
			WHERE NOT EXISTS (SELECT 1 FROM app_settings);
		""")
		connection.commit()
		cursor.close()
		print("✅ Date de mise à jour insérée ou mise à jour")
	except psycopg2.Error as e:
		print(f"❌ Erreur lors de l'insertion/mise à jour de app_settings.last_update_date: {e}")
		connection.rollback()

def insert_competitions(connection, competitions):
    try:
        cursor = connection.cursor()
        for comp in competitions:
            cursor.execute(
                "INSERT INTO Competitions (id, name) VALUES (%s, %s) ON CONFLICT (id) DO NOTHING",
                (comp['id'], comp['name'])
            )
        connection.commit()
        cursor.close()
        print(f"✅ {len(competitions)} compétitions insérées")
    except psycopg2.Error as e:
        print(f"❌ Erreur lors de l'insertion des compétitions: {e}")
        connection.rollback()

def insert_teams(connection, teams):
    try:
        cursor = connection.cursor()
        for team in teams:
            cursor.execute(
                "INSERT INTO Teams (id, name, competition_id) VALUES (%s, %s, %s) ON CONFLICT (id) DO NOTHING",
                (team['id'], team['name'], team['competition_id'])
            )
        connection.commit()
        cursor.close()
        print(f"✅ {len(teams)} équipes insérées")
    except psycopg2.Error as e:
        print(f"❌ Erreur lors de l'insertion des équipes: {e}")
        connection.rollback()

def insert_matches(connection, matches):
	required_fields = [
		"id", "competition_id", "team1_id", "team2_id",
		"score1", "score2", "possession1", "possession2",
		"passes1", "passes2", "successful_passes1", "successful_passes2",
		"shots1", "shots2", "shots_on_target1", "shots_on_target2"
	]
	matches = [
		match for match in matches
		if all(field in match and match[field] is not None for field in required_fields)
	]
	for match in matches: del match["url"]
	try:
		cursor = connection.cursor()
		for match in matches:
			print(f"Inserting/Updating match ID {match['id']}")
			cursor.execute("""
				INSERT INTO Matches (
					id, competition_id, team1_id, team2_id,
					score1, score2, possession1, possession2,
					passes1, passes2, successful_passes1, successful_passes2,
					shots1, shots2, shots_on_target1, shots_on_target2
				) VALUES (
					%s, %s, %s, %s, %s, %s, %s, %s,
					%s, %s, %s, %s, %s, %s, %s, %s
				) ON CONFLICT (id) DO UPDATE SET
					score1 = EXCLUDED.score1,
					score2 = EXCLUDED.score2,
					possession1 = EXCLUDED.possession1,
					possession2 = EXCLUDED.possession2,
					passes1 = EXCLUDED.passes1,
					passes2 = EXCLUDED.passes2,
					successful_passes1 = EXCLUDED.successful_passes1,
					successful_passes2 = EXCLUDED.successful_passes2,
					shots1 = EXCLUDED.shots1,
					shots2 = EXCLUDED.shots2,
					shots_on_target1 = EXCLUDED.shots_on_target1,
					shots_on_target2 = EXCLUDED.shots_on_target2
			""", (
				match['id'], match['competition_id'],
				match['team1_id'], match['team2_id'],
				match.get('score1'), match.get('score2'),
				match.get('possession1'), match.get('possession2'),
				match.get('passes1'), match.get('passes2'),
				match.get('successful_passes1'), match.get('successful_passes2'),
				match.get('shots1'), match.get('shots2'),
				match.get('shots_on_target1'), match.get('shots_on_target2')
			))
		connection.commit()
		cursor.close()
		print(f"✅ {len(matches)} matches insérés")
	except psycopg2.Error as e:
		print(f"❌ Erreur lors de l'insertion des matches: {e}")
		connection.rollback()


def fetch_url(url, session=None, competition_mode=False):
	if competition_mode: time.sleep(5)
	else: time.sleep(6)

	try:
		headers = {
			'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:143.0) Gecko/20100101 Firefox/143.0',
			'Accept': 'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
			'Accept-Language': 'en-GB,en;q=0.5',
			'Accept-Encoding': 'gzip, deflate, br, zstd',
			'Connection': 'keep-alive',
			'Upgrade-Insecure-Requests': '1',
			'Sec-Fetch-Dest': 'document',
			'Sec-Fetch-Mode': 'navigate',
			'Sec-Fetch-Site': 'same-origin',
			'Sec-Fetch-User': '?1',
			'Cache-Control': 'no-cache',
			'Pragma': 'no-cache'
		}
		
		if session: response = session.get(url, headers=headers)
		else: response = requests.get(url, headers=headers)

		response.raise_for_status()
		return response.text
	except requests.RequestException as e:
		print(f"Error fetching {url}: {e}")
		return None

def getCompetitionsAttributes(competition_matches_entries):
	competitions = []
	for i in range(len(competition_matches_entries)):
		competitions.append({
			"id": int(competition_matches_entries[i].split("/")[3]),
			"name": competition_matches_entries[i].split("/")[5].split("-Scores")[0]
		})
	return competitions

def getTeamsAttributes(soups, competitions):
	teams = []
	save = []

	for i, soup in enumerate(soups):
		teams_1 = soup.select('td[data-stat="home_team"] a[href]')
		teams_2 = soup.select('td[data-stat="away_team"] a[href]')
	
		for team1, team2 in zip(teams_1, teams_2):
			name1 = team1['href'].split("/")[-1].split("-Stats")[0]
			name2 = team2['href'].split("/")[-1].split("-Stats")[0]
	
			if (name1 in save) or (name2 in save):
				break
			save.append(name1)
			save.append(name2)

			id1 = int(team1['href'].split("/")[3], 16)
			id2 = int(team2['href'].split("/")[3], 16)

			teams.append({"name" : name1, "id" : id1, "competition_id": competitions[i]["id"]})
			teams.append({"name" : name2, "id" : id2, "competition_id": competitions[i]["id"]})
	return teams

def getMatchTeams(entry, teams):
	team_names = []
	for team in teams:
		index = entry.find(team["name"])
		if index != -1:
			team_names.append({"id": team["id"], "index": index})
		if len(team_names) == 2:
			break
	if len(team_names) == 2 and team_names[0]["index"] > team_names[1]["index"]:
		team_names[0], team_names[1] = team_names[1], team_names[0]
	return team_names[0]["id"], team_names[1]["id"]

def getMatchesBasicAttributes(soups, competitions, teams):
	matches = []
	for i in range(len(soups)):
		for td in soups[i].select('td.left[data-stat="match_report"]'):
			try:
				a = td.select(':scope > a[href]')
				for it in a:
					if it is not None and it["href"].find("/en/matches/") != -1:
						team1, team2 = getMatchTeams(it["href"], teams)
						if team1 is not None and team2 is not None:
							matches.append({
								"url": it["href"],
								"id": int(it["href"].split("/")[3], 16), 
								"competition_id": competitions[i]["id"], 
								"team1_id": int(team1), 
								"team2_id": int(team2)
							})
						else:
							print(f"Warning: Could not find both teams in '{it['href']}' for match")
			except Exception as e:
				print(f"Error processing match entry: {it['href']} - {e}")
	return matches

def getMatchesAttributes(soups, matches):
	for index in range(len(soups)):
		# Scores
		scorebox = soups[index].select_one('div.scorebox').select('div.score')
		score1 = scorebox[0].get_text(strip=True)
		score2 = scorebox[1].get_text(strip=True)

		# Possession, shots, shots on target, passes, passes success, 
		tr_elements = soups[index].select_one('table[cellpadding="0"][cellspacing="8"][width="100%"][border="0"]').select('tr')
		# Dictionnaire pour stocker les stats
		stats = {}
		
		# Parcourir les lignes pour trouver les statistiques
		for i, tr in enumerate(tr_elements):
			th_elements = tr.select('th')
			
			# Si c'est une ligne de titre avec une seule colonne
			if len(th_elements) == 1 and th_elements[0].get('colspan') == '2':
				stat_name = th_elements[0].get_text(strip=True)
				
				# Vérifier s'il y a une ligne de données après
				if i + 1 < len(tr_elements):
					data_tr = tr_elements[i + 1]
					td_elements = data_tr.select('td')
					
					if len(td_elements) == 2:
						# Extraire les valeurs selon le type de statistique
						if stat_name == "Possession":
							# Format: <strong>67%</strong>
							team1_val = td_elements[0].select_one('strong')
							team2_val = td_elements[1].select_one('strong')
							if team1_val and team2_val:
								stats['possession1'] = team1_val.get_text(strip=True).replace('%', '')
								stats['possession2'] = team2_val.get_text(strip=True).replace('%', '')
						
						elif stat_name == "Passing Accuracy":
							# Format équipe 1: "555 of 628 — <strong>88%</strong>"
							# Format équipe 2: "<strong>77%</strong> — 241 of 313"
							team1_text = td_elements[0].get_text(strip=True)
							team2_text = td_elements[1].get_text(strip=True)
							
							# Équipe 1: format "X of Y — Z%"
							if '—' in team1_text:
								parts = team1_text.split('—')
								if len(parts) == 2:
									pass_part = parts[0].strip()
									if ' of ' in pass_part:
										pass_split = pass_part.split(' of ')
										if len(pass_split) == 2:
											stats['successful_passes1'] = pass_split[0].strip()
											stats['passes1'] = pass_split[1].strip()
							
							# Équipe 2: format "Z% — X of Y"
							if '—' in team2_text:
								parts = team2_text.split('—')
								if len(parts) == 2:
									pass_part = parts[1].strip()
									if ' of ' in pass_part:
										pass_split = pass_part.split(' of ')
										if len(pass_split) == 2:
											stats['successful_passes2'] = pass_split[0].strip()
											stats['passes2'] = pass_split[1].strip()
						
						elif stat_name == "Shots on Target":
							# Format équipe 1: "5 of 16 — <strong>31%</strong>"
							# Format équipe 2: "<strong>20%</strong> — 2 of 10"
							team1_text = td_elements[0].get_text(strip=True)
							team2_text = td_elements[1].get_text(strip=True)
							
							# Équipe 1: format "X of Y — Z%"
							if '—' in team1_text:
								parts = team1_text.split('—')
								if len(parts) == 2:
									shots_part = parts[0].strip()
									if ' of ' in shots_part:
										shots_split = shots_part.split(' of ')
										if len(shots_split) == 2:
											stats['shots_on_target1'] = shots_split[0].strip()
											stats['shots1'] = shots_split[1].strip()
							
							# Équipe 2: format "Z% — X of Y"
							if '—' in team2_text:
								parts = team2_text.split('—')
								if len(parts) == 2:
									shots_part = parts[1].strip()
									if ' of ' in shots_part:
										shots_split = shots_part.split(' of ')
										if len(shots_split) == 2:
											stats['shots_on_target2'] = shots_split[0].strip()
											stats['shots2'] = shots_split[1].strip()

		matches[index].update({
			"score1": int(score1),
			"score2": int(score2),
			"possession1": int(stats.get('possession1', 0)),
			"possession2": int(stats.get('possession2', 0)),
			"passes1": int(stats.get('passes1', 0)),
			"successful_passes1": int(stats.get('successful_passes1', 0)),
			"passes2": int(stats.get('passes2', 0)),
			"successful_passes2": int(stats.get('successful_passes2', 0)),
			"shots1": int(stats.get('shots1', 0)),
			"shots_on_target1": int(stats.get('shots_on_target1', 0)),
			"shots2": int(stats.get('shots2', 0)),
			"shots_on_target2": int(stats.get('shots_on_target2', 0))
		})

	return matches

def getOnlyNewMatches(matches, connection):
	new_matches = []
	try:
		cursor = connection.cursor()
		match_ids = [match['id'] for match in matches]
		query = sql.SQL("SELECT id FROM Matches WHERE id = ANY(%s)")
		cursor.execute(query, (match_ids,))
		existing_ids = {row['id'] for row in cursor.fetchall()}
		new_matches = [match for match in matches if match['id'] not in existing_ids]
		cursor.close()

	except psycopg2.Error as e:
		print(f"❌ Erreur lors de la vérification des matches existants: {e}")
		connection.rollback()

	return new_matches

def run_scraper():
	global scraping_status
	scraping_status["running"] = True
	base_url = "https://fbref.com"

	try:
		timestamp_start = time.time()

		session = requests.Session()
		session.get(base_url)

		db_connection = connect_db()

		# Competitions : entries + soups
		competition_entries = [
			"/en/comps/11/schedule/Serie-A-Scores-and-Fixtures",
			"/en/comps/9/schedule/Premier-League-Scores-and-Fixtures",
			"/en/comps/13/schedule/Ligue-1-Scores-and-Fixtures",
			"/en/comps/20/schedule/Bundesliga-Scores-and-Fixtures"
		]
		competition_soups = [BeautifulSoup(fetch_url(base_url + entry, session, True), "html.parser") for entry in competition_entries]
		# ---------------

		competitions = getCompetitionsAttributes(competition_entries)
		teams = getTeamsAttributes(competition_soups, competitions)

		insert_last_update_date(db_connection)

		insert_competitions(db_connection, competitions)
		insert_teams(db_connection, teams)

		matches = getMatchesBasicAttributes(competition_soups, competitions, teams)
		matches = getOnlyNewMatches(matches, db_connection)

		print(f"Total matches found: {len(matches)}", flush=True)
		# Matches : entries + soups
		match_entries = [match["url"] for match in matches]
		match_soups = [BeautifulSoup(fetch_url(base_url + entry, session), "html.parser") for entry in match_entries]
		# ---------------

		print(f"Found {len(matches)} new matches to process", flush=True)
		matches = getMatchesAttributes(match_soups, matches)
		insert_matches(db_connection, matches)

		db_connection.close()

		timestamp_end = time.time()
		result = {
			"success": True,
			"message": f"✅ Scraping terminé",
			"duration": timestamp_end - timestamp_start,
		}
		scraping_status["last_result"] = result
	except Exception as e:
		result = {
			"success": False,
			"error": f"❌ Erreur lors du scraping: {e}"
		}
		scraping_status["last_result"] = result
	finally:
		scraping_status["running"] = False
		if 'db_connection' in locals() and db_connection:
			db_connection.close()


@app.route('/do-it', methods=['POST'])
def trigger_scraping():
	if scraping_status["running"]:
		return jsonify({"error": "Scraping déjà en cours"}), 409
	
	# Lancer dans un thread séparé
	thread = threading.Thread(target=run_scraper)
	thread.start()
	
	return jsonify({"message": "Scraping démarré", "status": "started"}), 200

@app.route('/status', methods=['GET'])
def get_status():
	return jsonify({
		"running": scraping_status["running"],
		"last_result": scraping_status["last_result"]
	})

if __name__ == "__main__":
    print(f"🌐 Serveur Flask démarré sur le port 8000")
    app.run(host='0.0.0.0', port=8000, debug=False)