# Football Master


<img src="./assets/demo.gif" alt="Demo" width="650" />

> Aperçu animé de l'application Qt Football Master

<img src="./assets/ui_diagram.png" alt="UI Diagram" width="350"/>

> Schéma de l'interface utilisateur principale


## Architecture & Interactions

```mermaid
flowchart TD
    subgraph Docker Network [Docker Network: football-net]
        DB[(PostgreSQL DB)]
        Scraper[Python Scraper]
        QtApp[Qt C++ App]
    end

    User((User)) -->|Lance l'app Qt| QtApp
    QtApp -->|Lit les données| DB
    QtApp -->|Demande un scraping| Scraper
    Scraper -->|Récupère des données| ExternalAPIs[(Football APIs)]
    Scraper -->|Insère dans DB| DB
    DB -->|Fournit les données| QtApp
    QtApp -->|Affiche l'UI| User

    style DB fill:#f9f,stroke:#333,stroke-width:2px
    style Scraper fill:#bbf,stroke:#333,stroke-width:2px
    style QtApp fill:#bfb,stroke:#333,stroke-width:2px
    style User fill:#fff,stroke:#333,stroke-width:2px
    style ExternalAPIs fill:#eee,stroke:#333,stroke-width:2px
```

**Fonctionnement global :**
- Démarrage des services Docker (DB, Scraper, Qt)
- Le Scraper collecte et insère les données dans la DB
- L'application Qt lit, analyse et affiche les données


## Quick Start

1. Configurez votre fichier `.env` (voir exemple ci-dessous)
2. Démarrez tous les services :
	```sh
	make up
	```
3. Build l'application Qt :
	```sh
	make build
	```
4. Lancez l'application Qt (avec X11 forwarding si besoin) :
	```sh
	make run
	```

## Structure du projet

- `db/` : Initialisation et données de la base PostgreSQL
- `scraper/` : Scraper Python (Flask, requests, BeautifulSoup, psycopg2)
- `qt/` : Application Qt/C++ (analyse et visualisation)

## Exemple de fichier .env

```
DB_USER=football_user
DB_PASSWORD=football_pass
DB_NAME=football_db
PROJECT_NAME=football_master
EXECUTABLE=football_master
```


## Requirements

- Docker
- Make
