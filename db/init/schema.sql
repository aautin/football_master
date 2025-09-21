-- Competitions
CREATE TABLE Competitions (
  id BIGINT PRIMARY KEY,
  name VARCHAR(100) NOT NULL
);

-- Teams
CREATE TABLE Teams (
  id BIGINT PRIMARY KEY,
  name VARCHAR(100) NOT NULL
);

-- Matches (always team1 and team2)
CREATE TABLE Matches (
  id BIGINT PRIMARY KEY,
  competition_id BIGINT NOT NULL,
  team1_id BIGINT NOT NULL,
  team2_id BIGINT NOT NULL,
  score1 INT,
  score2 INT,
  possession1 INT,
  possession2 INT,
  passes1 INT,
  passes2 INT,
  successful_passes1 INT,
  successful_passes2 INT,
  shots1 INT,
  shots2 INT,
  shots_on_target1 INT,
  shots_on_target2 INT,
  FOREIGN KEY (competition_id) REFERENCES Competitions(id),
  FOREIGN KEY (team1_id) REFERENCES Teams(id),
  FOREIGN KEY (team2_id) REFERENCES Teams(id)
);
