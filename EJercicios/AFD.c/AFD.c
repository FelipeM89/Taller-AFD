#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ESTADOS 64
#define MAX_NOMBRE  32
#define MAX_LINEA   256

// --- Almacenamiento del AFD ---
static char nombres_estados[MAX_ESTADOS][MAX_NOMBRE];
static int  num_estados = 0;
static int  inicial = -1;
static int  es_final[MAX_ESTADOS];
static int  transicion[MAX_ESTADOS][2];   // alfabeto {0,1}; -1 si no hay transición

// ---------- Utilidades ----------
static void recortar_derecha(char *s) {
    int n = (int)strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || isspace((unsigned char)s[n-1])))
        s[--n] = '\0';
}
static void recortar_izquierda(char *s) {
    int i = 0; while (isspace((unsigned char)s[i])) i++;
    if (i) memmove(s, s+i, strlen(s+i)+1);
}
static void recortar(char *s) { recortar_derecha(s); recortar_izquierda(s); }

static int indice_estado(const char *nombre) {
    for (int i = 0; i < num_estados; i++)
        if (strcmp(nombres_estados[i], nombre) == 0) return i;
    return -1;
}

static void agregar_estado(const char *nombre) {
    if (indice_estado(nombre) != -1) return;
    if (num_estados >= MAX_ESTADOS) {
        fprintf(stderr, "Demasiados estados (>%d)\n", MAX_ESTADOS);
        exit(1);
    }
    strncpy(nombres_estados[num_estados], nombre, MAX_NOMBRE-1);
    nombres_estados[num_estados][MAX_NOMBRE-1] = '\0';
    es_final[num_estados] = 0;
    transicion[num_estados][0] = transicion[num_estados][1] = -1;
    num_estados++;
}

static void esperar(int condicion, const char *mensaje) {
    if (!condicion) { fprintf(stderr, "Error de configuración: %s\n", mensaje); exit(1); }
}

// ---------- Parseo de Conf.txt ----------
static void cargar_config(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    esperar(f != NULL, "no se pudo abrir Conf.txt");

    // inicializar
    num_estados = 0; inicial = -1;
    for (int i = 0; i < MAX_ESTADOS; i++) es_final[i] = 0;
    for (int i = 0; i < MAX_ESTADOS; i++) transicion[i][0] = transicion[i][1] = -1;

    enum {NINGUNA, ESTADOS, INICIAL, FINALES, TRANSICIONES} seccion = NINGUNA;
    char linea[MAX_LINEA];

    while (fgets(linea, sizeof(linea), f)) {
        recortar(linea);
        if (linea[0] == '\0') continue;                 // vacío
        if (linea[0] == '#') {                          // encabezado de sección
            if (strstr(linea, "Estados de aceptación")) seccion = FINALES;
            else if (strstr(linea, "Estado inicial"))   seccion = INICIAL;
            else if (strstr(linea, "Transiciones"))     seccion = TRANSICIONES;
            else if (strstr(linea, "Estados"))          seccion = ESTADOS;
            else seccion = NINGUNA;
            continue;
        }

        if (seccion == ESTADOS) {
            // lista de estados en una o varias líneas (se permiten espacios)
            char *tok = strtok(linea, " \t");
            while (tok) { agregar_estado(tok); tok = strtok(NULL, " \t"); }
        }
        else if (seccion == INICIAL) {
            char nombre[MAX_NOMBRE]; sscanf(linea, "%31s", nombre);
            agregar_estado(nombre);                           // por si no apareció arriba
            inicial = indice_estado(nombre);
        }
        else if (seccion == FINALES) {
            char *tok = strtok(linea, " \t");
            while (tok) {
                agregar_estado(tok);
                int idx = indice_estado(tok);
                esperar(idx != -1, "estado final no declarado");
                es_final[idx] = 1;
                tok = strtok(NULL, " \t");
            }
        }
        else if (seccion == TRANSICIONES) {
            // formato: origen <espacio> simbolo <espacio> destino
            char a[MAX_NOMBRE], b[MAX_NOMBRE]; char c;
            if (sscanf(linea, "%31s %c %31s", a, &c, b) == 3) {
                esperar(c == '0' || c == '1', "símbolo distinto de 0/1");
                agregar_estado(a); agregar_estado(b);
                int i = indice_estado(a), j = indice_estado(b);
                esperar(i!=-1 && j!=-1, "estado en transición no declarado");
                transicion[i][c - '0'] = j;
            }
        }
    }
    fclose(f);

    esperar(num_estados > 0, "no se definieron estados");
    esperar(inicial != -1, "no se definió estado inicial");

    // Validar que cada estado tenga definición para 0 y 1
    for (int i = 0; i < num_estados; i++) {
        esperar(transicion[i][0] != -1, "falta transición con 0");
        esperar(transicion[i][1] != -1, "falta transición con 1");
    }
}

// ---------- Simulación ----------
static int ejecutar(const char *w) {
    int q = inicial;
    for (int i = 0; w[i]; i++) {
        if (w[i] == '0' || w[i] == '1') {
            q = transicion[q][w[i]-'0'];
        } else {
            // símbolo inválido: rechazamos
            return 0;
        }
    }
    return es_final[q];
}

static void mostrar_tabla(void) {
    printf("\n== AFD cargado ==\n");
    printf("Estados: ");
    for (int i = 0; i < num_estados; i++) {
        printf("%s%s%s ", (i==inicial)?"[":"", nombres_estados[i], es_final[i]?"+":"]");
        if (i!=inicial && !es_final[i]) {} // solo para suprimir warning
    }
    printf("\nTransiciones (q,0)-> | (q,1)->\n");
    for (int i = 0; i < num_estados; i++) {
        printf("%-8s  %-8s | %-8s\n",
            nombres_estados[i],
            nombres_estados[ transicion[i][0] ],
            nombres_estados[ transicion[i][1] ]);
    }
    printf("\n");
}

int main(void) {
    cargar_config("Conf.txt");
    mostrar_tabla();  // comenta esta línea si no quieres imprimir la tabla

    FILE *g = fopen("Cadenas.txt", "r");
    if (!g) { perror("Cadenas.txt"); return 1; }

    char linea[MAX_LINEA];
    while (fgets(linea, sizeof(linea), g)) {
        recortar(linea);
        if (linea[0] == '\0') continue;   // líneas vacías
        // Permitir 'E' como cadena vacía
        if (strcmp(linea, "E") == 0) linea[0] = '\0';
        printf("%s -> %s\n", (linea[0]? linea : "ε"), ejecutar(linea) ? "Acepta" : "NO acepta");
    }
    fclose(g);
    return 0;
}


