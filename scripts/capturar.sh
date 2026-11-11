#!/bin/bash
LOG_RAW="capturas/telemetria_raw.log"
FECHA=$(date "+%Y-%m-%d %H:%M:%S")

DISCO_VAL=$(df -P "$HOME" | tail -1 | sed -r 's/.*[[:space:]]([0-9]+)%.*$/\1/')
PROCESOS_VAL=$(ps -u "$USER" --no-headers | wc -l | tr -d ' ')
PERMISOS_VAL=$(find . -maxdepth 3 -type f \( -perm -020 -o -perm -002 \) | wc -l | tr -d ' ')

[[ ! "$DISCO_VAL" =~ ^[0-9]+$ ]] && { echo "Error captura DISCO"; exit 1; }
[[ ! "$PROCESOS_VAL" =~ ^[0-9]+$ ]] && { echo "Error captura PROCESOS"; exit 1; }
[[ ! "$PERMISOS_VAL" =~ ^[0-9]+$ ]] && { echo "Error captura PERMISOS"; exit 1; }

echo "$FECHA DISCO $DISCO_VAL" >> "$LOG_RAW"
echo "$FECHA PROCESOS $PROCESOS_VAL" >> "$LOG_RAW"
echo "$FECHA PERMISOS $PERMISOS_VAL" >> "$LOG_RAW"
