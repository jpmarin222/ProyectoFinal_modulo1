#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ETIQ 32
#define MAX_CAT 16
#define MAX_LINEA 256

typedef struct {
    char categoria[MAX_CAT];
    int valor;
} Medicion;

typedef struct {
    char nombre[MAX_CAT];
    int cantidad;
    long suma;
    int minimo;
    int maximo;
    double promedio;
    int alertas;
} ResumenCategoria;

int validar_rango(const char *cat, int val) {
    if (strcmp(cat, "DISCO") == 0) {
        return (val >= 0 && val <= 100);
    } else if (strcmp(cat, "PROCESOS") == 0) {
        return (val >= 0 && val <= 2000);
    } else if (strcmp(cat, "PERMISOS") == 0) {
        return (val >= 0 && val <= 200);
    }
    return 0;
}

int leer_y_validar(const char *ruta, char *etiqueta, int *total_reg, Medicion **lista) {
    FILE *fp = fopen(ruta, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: No se pudo abrir el archivo '%s'.\n", ruta);
        return 0;
    }

    char linea[MAX_LINEA];
    char extra[MAX_LINEA];

    /* 1. Leer cantidad de registros y validar intervalo (0 a 200) */
    if (fgets(linea, sizeof linea, fp) == NULL) {
        fprintf(stderr, "Error: Archivo vacio o sin cantidad de registros.\n");
        fclose(fp);
        return 0;
    }
    if (sscanf(linea, "%d %s", total_reg, extra) != 1) {
        fprintf(stderr, "Error: Formato invalido en cantidad de registros.\n");
        fclose(fp);
        return 0;
    }
    if (*total_reg < 0 || *total_reg > 200) {
        fprintf(stderr, "Error: Cantidad fuera de intervalo (0 a 200).\n");
        fclose(fp);
        return 0;
    }

    /* 2. Leer etiqueta y validar longitud (1 a 31) sin espacios */
    if (fgets(linea, sizeof linea, fp) == NULL) {
        fprintf(stderr, "Error: Falta la linea de etiqueta.\n");
        fclose(fp);
        return 0;
    }
    linea[strcspn(linea, "\r\n")] = '\0';

    size_t len = strlen(linea);
    if (len == 0 || len > 31) {
        fprintf(stderr, "Error: La etiqueta debe tener entre 1 y 31 caracteres.\n");
        fclose(fp);
        return 0;
    }
    for (size_t i = 0; i < len; i++) {
        if (isspace((unsigned char)linea[i])) {
            fprintf(stderr, "Error: La etiqueta no debe contener espacios.\n");
            fclose(fp);
            return 0;
        }
    }
    strcpy(etiqueta, linea);

    /* 3. Reservar memoria dinamica segun el estilo del curso */
    *lista = NULL;
    if (*total_reg > 0) {
        *lista = malloc((size_t)(*total_reg) * sizeof **lista);
        if (*lista == NULL) {
            fprintf(stderr, "Error: No se pudo reservar memoria dinamica.\n");
            fclose(fp);
            return 0;
        }
    }

    /* 4. Leer exactamente *total_reg mediciones validas */
    int leidos = 0;
    while (fgets(linea, sizeof linea, fp) != NULL) {
        int vacia = 1;
        for (size_t i = 0; linea[i] != '\0'; i++) {
            if (!isspace((unsigned char)linea[i])) {
                vacia = 0;
                break;
            }
        }
        if (vacia) {
            continue;
        }

        if (leidos >= *total_reg) {
            fprintf(stderr, "Error: Hay mas registros de los declarados (%d).\n", *total_reg);
            free(*lista);
            *lista = NULL;
            fclose(fp);
            return 0;
        }

        char cat_temp[MAX_CAT];
        int val_temp;
        if (sscanf(linea, "%15s %d %s", cat_temp, &val_temp, extra) != 2) {
            fprintf(stderr, "Error: Formato invalido en el registro %d.\n", leidos + 1);
            free(*lista);
            *lista = NULL;
            fclose(fp);
            return 0;
        }

        if (!validar_rango(cat_temp, val_temp)) {
            fprintf(stderr, "Error: Categoria o valor fuera de rango (%s %d).\n", cat_temp, val_temp);
            free(*lista);
            *lista = NULL;
            fclose(fp);
            return 0;
        }

        strcpy((*lista)[leidos].categoria, cat_temp);
        (*lista)[leidos].valor = val_temp;
        leidos++;
    }

    fclose(fp);

    if (leidos < *total_reg) {
        fprintf(stderr, "Error: Faltan registros (declarados: %d, leidos: %d).\n", *total_reg, leidos);
        free(*lista);
        *lista = NULL;
        return 0;
    }

    return 1;
}

void procesar_categorias(const Medicion lista[], int total_reg, ResumenCategoria res[], int *total_alertas) {
    const char *nombres[3] = {"DISCO", "PROCESOS", "PERMISOS"};
    *total_alertas = 0;

    for (int c = 0; c < 3; c++) {
        strcpy(res[c].nombre, nombres[c]);
        res[c].cantidad = 0;
        res[c].suma = 0;
        res[c].minimo = 0;
        res[c].maximo = 0;
        res[c].promedio = 0.0;
        res[c].alertas = 0;
    }

    for (int i = 0; i < total_reg; i++) {
        int idx = -1;
        if (strcmp(lista[i].categoria, "DISCO") == 0) {
            idx = 0;
        } else if (strcmp(lista[i].categoria, "PROCESOS") == 0) {
            idx = 1;
        } else if (strcmp(lista[i].categoria, "PERMISOS") == 0) {
            idx = 2;
        }

        if (idx != -1) {
            int val = lista[i].valor;
            if (res[idx].cantidad == 0) {
                res[idx].minimo = val;
                res[idx].maximo = val;
            } else {
                if (val < res[idx].minimo) res[idx].minimo = val;
                if (val > res[idx].maximo) res[idx].maximo = val;
            }

            res[idx].cantidad++;
            res[idx].suma += val;

            if ((idx == 0 && val >= 80) ||
                (idx == 1 && val >= 30) ||
                (idx == 2 && val > 0)) {
                res[idx].alertas++;
                (*total_alertas)++;
            }
        }
    }

    for (int c = 0; c < 3; c++) {
        if (res[c].cantidad > 0) {
            res[c].promedio = (double)res[c].suma / res[c].cantidad;
        }
    }
}

int generar_informe(const char *ruta_salida, const char *etiqueta, int total_reg, const ResumenCategoria res[], int total_alertas) {
    FILE *out = fopen(ruta_salida, "w");
    if (out == NULL) {
        fprintf(stderr, "Error: No se pudo crear el archivo '%s'.\n", ruta_salida);
        return 0;
    }

    fprintf(out, "INFORME DE MONITOREO\n");
    fprintf(out, "Etiqueta: %s\n", etiqueta);
    fprintf(out, "Registros: %d\n\n", total_reg);

    for (int c = 0; c < 3; c++) {
        fprintf(out, "Categoría %s\n", res[c].nombre);
        if (res[c].cantidad == 0) {
            fprintf(out, "Sin datos\n\n");
        } else {
            fprintf(out, "Cantidad: %d\n", res[c].cantidad);
            fprintf(out, "Suma: %ld\n", res[c].suma);
            fprintf(out, "Mínimo: %d\n", res[c].minimo);
            fprintf(out, "Máximo: %d\n", res[c].maximo);
            fprintf(out, "Promedio: %.2f\n", res[c].promedio);
            fprintf(out, "Alertas: %d\n\n", res[c].alertas);
        }
    }

    fprintf(out, "Observaciones que cumplen condiciones de alerta: %d.\n", total_alertas);
    fprintf(out, "Umbrales utilizados con fines didácticos.\n");

    if (fflush(out) != 0 || ferror(out)) {
        fprintf(stderr, "Error: Fallo al escribir el informe.\n");
        fclose(out);
        return 0;
    }

    if (fclose(out) != 0) {
        fprintf(stderr, "Error: Fallo al cerrar el archivo de informe.\n");
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <archivo_entrada> <archivo_salida>\n", argv[0]);
        return 1;
    }

    char etiqueta[MAX_ETIQ];
    int total_reg = 0;
    Medicion *lista = NULL;

    if (!leer_y_validar(argv[1], etiqueta, &total_reg, &lista)) {
        return 1;
    }

    ResumenCategoria resumen[3];
    int total_alertas = 0;
    procesar_categorias(lista, total_reg, resumen, &total_alertas);

    int ok = generar_informe(argv[2], etiqueta, total_reg, resumen, total_alertas);

    free(lista);
    lista = NULL;

    if (!ok) {
        return 1;
    }

    printf("Informe generado correctamente en %s\n", argv[2]);
    return 0;
}
