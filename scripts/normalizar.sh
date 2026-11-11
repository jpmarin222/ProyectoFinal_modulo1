#!/bin/bash
LOG_RAW="capturas/telemetria_raw.log"
OUT_NORMALIZADO="preparados/mediciones_sec.txt"
ETIQUETA="AuditoriaCursoC"

if [ ! -f "$LOG_RAW" ]; then
    echo "Error: No existe $LOG_RAW"
    exit 1
fi

REGISTROS=$(sed -r 's/^[^ ]+ [^ ]+ (.*)/\1/' "$LOG_RAW")
TOTAL=$(echo "$REGISTROS" | grep -c -E "^(DISCO|PROCESOS|PERMISOS)")

echo "$TOTAL" > "$OUT_NORMALIZADO"
echo "$ETIQUETA" >> "$OUT_NORMALIZADO"
echo "$REGISTROS" >> "$OUT_NORMALIZADO"
