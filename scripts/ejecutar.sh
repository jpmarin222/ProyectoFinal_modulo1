#!/bin/bash

DIR_CAPTURAS="capturas"
DIR_PREPARADOS="preparados"
DIR_RESULTADOS="resultados"
DIR_RESPALDOS="respaldos"
DIR_FUENTES="fuentes"
DIR_SCRIPTS="scripts"

FUENTE_C="$DIR_FUENTES/sec_engine.c"
BINARIO_C="$DIR_FUENTES/sec_engine"
SCRIPT_CAPTURA="$DIR_SCRIPTS/capturar.sh"
SCRIPT_NORM="$DIR_SCRIPTS/normalizar.sh"

ENTRADA_NORM="$DIR_PREPARADOS/mediciones_sec.txt"
INFORME_FINAL="$DIR_RESULTADOS/informe_seguridad.txt"
TIMESTAMP=$(date "+%Y%m%d_%H%M%S")

echo "=== [1/5] Verificando estructura y archivos necesarios ==="
for d in "$DIR_CAPTURAS" "$DIR_PREPARADOS" "$DIR_RESULTADOS" "$DIR_RESPALDOS" "$DIR_FUENTES" "$DIR_SCRIPTS"; do
    if [ ! -d "$d" ]; then
        echo "Error: No existe el directorio requerido '$d'." >&2
        exit 1
    fi
done

for f in "$FUENTE_C" "$SCRIPT_CAPTURA" "$SCRIPT_NORM"; do
    if [ ! -f "$f" ]; then
        echo "Error: No existe el archivo requerido '$f'." >&2
        exit 1
    fi
done

if [ ! -x "$SCRIPT_CAPTURA" ] || [ ! -x "$SCRIPT_NORM" ]; then
    echo "Error: Los scripts no tienen permisos de ejecucion." >&2
    exit 1
fi

echo "=== [2/5] Compilando analizador en C ==="
gcc -Wall -Wextra -Werror "$FUENTE_C" -o "$BINARIO_C"
if [ $? -ne 0 ]; then
    echo "Error: Fallo la compilacion de $FUENTE_C." >&2
    exit 1
fi
chmod 750 "$BINARIO_C"

echo "=== [3/5] Ejecutando captura y normalizacion ==="
"$SCRIPT_CAPTURA"
if [ $? -ne 0 ]; then
    echo "Error: Fallo la captura de telemetria." >&2
    exit 1
fi

"$SCRIPT_NORM"
if [ $? -ne 0 ]; then
    echo "Error: Fallo la normalizacion de datos." >&2
    exit 1
fi

echo "=== [4/5] Ejecutando motor de analisis en C ==="
"$BINARIO_C" "$ENTRADA_NORM" "$INFORME_FINAL"
if [ $? -ne 0 ]; then
    echo "Error: Fallo el analisis en C." >&2
    exit 1
fi

echo "=== [5/5] Generando respaldo con marca de tiempo ==="
tar -czf "$DIR_RESPALDOS/respaldo_auditoria_${TIMESTAMP}.tar.gz" \
    "$DIR_CAPTURAS/telemetria_raw.log" "$ENTRADA_NORM" "$INFORME_FINAL"
if [ $? -ne 0 ]; then
    echo "Error: No se pudo crear el respaldo." >&2
    exit 1
fi
chmod 640 "$DIR_RESPALDOS/respaldo_auditoria_${TIMESTAMP}.tar.gz"

echo "Flujo completado exitosamente. Informe: $INFORME_FINAL"
exit 0
