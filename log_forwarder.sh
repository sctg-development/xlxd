#!/bin/bash

# Script pour rediriger les logs vers stdout et fichiers
# Usage: log_forwarder.sh <program_name> <log_file>

PROGRAM_NAME=$1
LOG_FILE=$2

# Créer le fichier de log s'il n'existe pas
touch "$LOG_FILE"

# Utiliser tail pour suivre le fichier et préfixer chaque ligne
tail -F "$LOG_FILE" 2>/dev/null | while IFS= read -r line; do
    echo "$(date '+%Y-%m-%d %H:%M:%S') INFO $PROGRAM_NAME stdout: $line"
done
