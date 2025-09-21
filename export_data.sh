#!/bin/bash
set -e  # Arrêter le script en cas d'erreur

# Couleurs pour les messages
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m'

MAX_ATTEMPTS=10
ATTEMPT=0

if 
docker-compose up -d db
while [ $ATTEMPT -lt $MAX_ATTEMPTS ]; do
    if docker-compose exec db pg_isready -U ${DB_USER:-football_user} -d ${DB_NAME:-football_db} >/dev/null 2>&1; then
        break
    fi
    
    ATTEMPT=$((ATTEMPT + 1))
    sleep 2
done

if [ $ATTEMPT -eq $MAX_ATTEMPTS ]; then
    echo -e "${RED}❌ Timeout: PostgreSQL n'est pas prêt après $MAX_ATTEMPTS tentatives${NC}"
    exit 1
fi

echo -e "${YELLOW}📤 Export des données...${NC}"

docker-compose exec db pg_dump -U ${DB_USER:-football_user} -d ${DB_NAME:-football_db} --data-only --inserts > db/init/02-data.sql

echo -e "${GREEN}✅ Export terminé!${NC}"
