# Football Master



<img src="./assets/demo.gif" alt="Demo" width="650" />

> Animated preview of the Qt Football Master application


<img src="./assets/ui_diagram.png" alt="UI Diagram" width="350"/>

> Main user interface diagram



## Architecture & Interactions

```mermaid
flowchart TD
    subgraph Docker Network [Docker Network: football-net]
        DB[(PostgreSQL DB)]
        Scraper[Python Scraper]
        QtApp[Qt C++ App]
    end

    User((User)) -->|Launches Qt app| QtApp
    QtApp -->|Reads data| DB
    QtApp -->|Requests scraping| Scraper
    Scraper -->|Fetches data| ExternalAPIs[(Football APIs)]
    Scraper -->|Inserts into DB| DB
    DB -->|Provides data| QtApp
    QtApp -->|Displays UI| User

    style DB fill:#f9f,stroke:#333,stroke-width:2px
    style Scraper fill:#bbf,stroke:#333,stroke-width:2px
    style QtApp fill:#bfb,stroke:#333,stroke-width:2px
    style User fill:#fff,stroke:#333,stroke-width:2px
    style ExternalAPIs fill:#eee,stroke:#333,stroke-width:2px
```

**Global workflow:**
- Start Docker services (DB, Scraper, Qt)
- Scraper collects and inserts data into the DB
- The Qt application reads, analyzes, and displays the data



## Quick Start

1. Configure your `.env` file (see example below)
2. Start all services:
    ```sh
    make up
    ```
3. Build the Qt application:
    ```sh
    make build
    ```
4. Run the Qt application (with X11 forwarding if needed):
    ```sh
    make run
    ```


## Project Structure

- `db/` : PostgreSQL initialization and data
- `scraper/` : Python scraper (Flask, requests, BeautifulSoup, psycopg2)
- `qt/` : Qt/C++ application (analysis and visualization)

## Example .env file

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
