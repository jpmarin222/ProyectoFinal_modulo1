                                                                                                                                                    Marin Tinoco Juan Pablo

                                                                            Proyecto Final Modulo 1

Preguntas a responder de la etepa 1:

1. ¿Por qué los permisos adecuados para un directorio pueden ser distintos de los necesarios para un archivo de datos? 
En los directorios se requiere el bit de ejecución (x) para atravesar o buscar dentro de ellos, mientras que en un archivo de datos solo se requiere lectura/escritura (rw-) para evitar ejecuciones accidentales. 
2. ¿Qué diferencia existe entre el archivo original y su enlace duro?
No existe diferencia física a nivel de sistema de archivos ya que ambos apuntan al mismo inodo y comparten bloques de datos. Renombrar o mover el original no afecta al enlace duro.
3. ¿Cuál es la utilidad de establecer enlaces simbólicos en el desarrollo de un sistema? 
Permiten crear accesos directos modulables y flexibles a carpetas de trabajo sin duplicar almacenamiento ni alterar la jerarquía de rutas relativas o absolutas. 
4. ¿Qué información de los metadatos permite reconocer cambios en un archivo? 
Las marcas de tiempo (timestamps: mtime de modificación de contenido y ctime de cambio de metadatos), el tamaño en bytes y el número de inodo. 

Preguntas a responder de la etapa 2:

1. ¿Qué transformación necesita la salida de cada utilería para convertirse en un registro del archivo normalizado? 
Mediante tuberías se filtra el texto adicional para dejar solo el entero limpio: de df -P se toma la última línea y se extrae el número sin % usando sed -r; de ps y find se cuentan las líneas con wc -l. Finalmente, normalizar.sh elimina fecha y hora con sed -r y agrega el encabezado. 
2. ¿Por qué un valor cero puede ser válido y no debe confundirse con una captura fallida? 
Un 0 es una medición real (por ejemplo, PERMISOS 0 indica que no hay archivos inseguros) y debe promediarse. Una captura fallida ocurre cuando un comando falla ($? != 0) o no entrega un número (^[0-9]+$); sustituir un fallo por 0 distorsionaría las estadísticas, por lo que el script debe abortar. 
3. ¿Cómo comprobarán que la cantidad del encabezado corresponde a los registros conservados? 
Contando las líneas válidas del flujo con grep -c -E "^(DISCO|PROCESOS|PERMISOS)", comparando ese total contra las líneas reales (wc -l) para asegurar que coincidan (y estén entre 0 y 200), y escribiendo ese número exacto en la primera línea del archivo. 
4. ¿Por qué el orden de las instrucciones dentro del flujo de sed es estrictamente determinante al habilitar directivas y eliminar comentarios?  
Porque sed ejecuta las órdenes secuencialmente. Si se eliminan primero los comentarios, se borrarían las líneas #Port y #HostKey antes de poder activarlas; por eso primero se descomentan y sustituyen las directivas, y al final se descartan los comentarios restantes. 
5. ¿Qué estrategia se empleó para aislar el puerto sin limitar obtener correctamente los cuatro octetos de la IP?  
Se ancló el patrón al inicio (^IP ) para evitar que .* recortara el primer octeto por codicia, y se capturó la dirección completa ([0-9a-fA-F:.]+) hasta el último punto literal (\.) que separa el puerto (([0-9]+)). 

Preguntas a respoder de la etapa 3:

1. ¿Qué diferencia existe entre almacenar una medición y almacenar el resumen de una categoría? 
Almacenar una medición guarda cada registro individual (categoría y valor) en el arreglo dinámico; almacenar el resumen agrupa todos los registros de una misma categoría en contadores y acumuladores (cantidad, suma, mínimo, máximo, promedio y alertas). 
2. ¿Cómo evitarán una división entre cero o utilizar un mínimo que todavía no se ha inicializado? 
Comprobando que cantidad > 0 antes de calcular el promedio (de lo contrario se imprime "Sin datos"), e inicializando mínimo y máximo con el valor de la primera medición válida cuando cantidad == 0. 
3. Si ocurre un error después de reservar memoria y abrir un archivo, ¿qué recursos deben liberar o cerrar? 
Se debe liberar la memoria dinámica con free() (asignando NULL al puntero) y cerrar el archivo abierto con fclose() antes de salir con error. 

Preguntas a responder de la etapa 4:

1. ¿Cómo decide el script si puede continuar después de ejecutar el programa C? 
Porque si falla una etapa previa, el archivo de entrada (mediciones_sec.txt) podría no existir, estar vacío o contener datos incompletos; detener el flujo con $? evita analizar información corrupta o sobrescribir un informe válido con datos erróneos. 
2. ¿Cómo se distinguirá un informe nuevo de uno conservado de una ejecución anterior? 
Revisando con ls -la y asignando 750 (rwxr-x---) a directorios, scripts y ejecutables para que solo el propietario y su grupo puedan ejecutarlos, y 640 (rw-r-----) a códigos fuente, logs e informes para impedir su ejecución directa y bloquear el acceso a otros usuarios (---). 
3. ¿Qué evidencia demuestra que el informe se obtuvo mediante el flujo completo y no se elaboró manualmente? 
Permite mantener trazabilidad histórica de cada auditoría sin sobrescribir ejecuciones anteriores, facilitando comparar métricas entre distintas fechas y recuperar el estado exacto de entradas e informes ante cualquier incidente. 

