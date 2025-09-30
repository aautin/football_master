include .env
export $(shell sed 's/=.*//' .env)

# Variables détectées automatiquement (pas dans .env)
export DISPLAY := $(shell echo $$DISPLAY)
export XDG_RUNTIME_DIR := $(shell echo $$XDG_RUNTIME_DIR)
export UID := $(shell id -u)
export GID := $(shell id -g)

.PHONY: up down down-v build run enter-qt enter-db enter-scraper logs save debug

up:
	@echo "🚀 Démarrage des services..."
	@echo "📺 DISPLAY: $(DISPLAY)"
	@echo "🗂️  XDG_RUNTIME_DIR: $(XDG_RUNTIME_DIR)"
	docker-compose up -d --no-recreate --build

down:
	@echo "🛑 Arrêt des services..."
	docker-compose down --timeout 2
down-v:
	@echo "🛑 Arrêt des services et suppression des volumes..."
	docker-compose down -v --timeout 2

build: up
	@echo "🔨 Build du projet Qt..."
	docker-compose exec qt cmake -S /app -B /app/build
	docker-compose exec qt cmake --build /app/build

run: build
	@echo "🖥️  Lancement de l'application Qt..."
	@xhost +local:docker 2>/dev/null || echo "⚠️ xhost non disponible"
	@docker-compose exec qt /app/build/$(EXECUTABLE) > last_run.log 2>&1

enter-qt:
	docker-compose exec qt bash -c "cd /app && exec bash"
enter-db:
	docker-compose exec db psql -U $${DB_USER:-football_user} -d $${DB_NAME:-football_db}
enter-scraper:
	docker-compose exec scraper bash -c "cd /app && exec bash"

logs:
	docker-compose logs -f

save: up
	docker-compose exec db pg_dump -U $${DB_USER:-football_user} -d $${DB_NAME:-football_db} --data-only --inserts > db/init/02-data.sql

debug: build
	@echo "🖥️ Lancement de l'application Qt en mode debug..."
	@xhost +local:docker 2>/dev/null || echo "⚠️ xhost non disponible"
	@docker-compose exec -e DEBUG_FOOTBALL_MASTER=TRUE qt /app/build/$(EXECUTABLE) > last_run.log 2>&1