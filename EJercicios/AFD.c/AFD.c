#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_STATES 64
#define MAX_NAME   32
#define MAX_LINE   256

// --- Almacenamiento del AFD ---
static char state_names[MAX_STATES][MAX_NAME];
static int  num_states = 0;
static int  initial = -1;
static int  is_final[MAX_STATES];
static int  delta[MAX_STATES][2];   // alfabeto {0,1}; -1 si no hay transición

// ---------- Utilidades ----------
static void rtrim(char *s) {
    int n = (int)strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || isspace((unsigned char)s[n-1])))
        s[--n] = '\0';
}
static void ltrim(char *s) {
    int i = 0; while (isspace((unsigned char)s[i])) i++;
    if (i) memmove(s, s+i, strlen(s+i)+1);
}
static void trim(char *s) { rtrim(s); ltrim(s); }

static int index_of_state(const char *name) {
    for (int i = 0; i < num_states; i++)
        if (strcmp(state_names[i], name) == 0) return i;
    return -1;
}

static void add_state(const char *name) {
    if (index_of_state(name) != -1) return;
    if (num_states >= MAX_STATES) {
        fprintf(stderr, "Demasiados estados (>%d)\n", MAX_STATES);
        exit(1);
    }
    strncpy(state_names[num_states], name, MAX_NAME-1);
    state_names[num_states][MAX_NAME-1] = '\0';
    is_final[num_states] = 0;
    delta[num_states][0] = delta[num_states][1] = -1;
    num_states++;
}

static void expect(int cond, const char *msg) {
    if (!cond) { fprintf(stderr, "Error de configuración: %s\n", msg); exit(1); }
}

// ---------- Parseo de Conf.txt ----------
static void load_config(const char *path) {
    FILE *f = fopen(path, "r");
    expect(f != NULL, "no se pudo abrir Conf.txt");

    // inicializar
    num_states = 0; initial = -1;
    for (int i = 0; i < MAX_STATES; i++) is_final[i] = 0;
    for (int i = 0; i < MAX_STATES; i++) delta[i][0] = delta[i][1] = -1;

    enum {NONE, STATES, INITIAL, FINALS, TRANSITIONS} section = NONE;
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), f)) {
        trim(line);
        if (line[0] == '\0') continue;                 // vacío
        if (line[0] == '#') {                          // encabezado de sección
            if (strstr(line, "Estados de aceptación")) section = FINALS;
            else if (strstr(line, "Estado inicial"))   section = INITIAL;
            else if (strstr(line, "Transiciones"))     section = TRANSITIONS;
            else if (strstr(line, "Estados"))          section = STATES;
            else section = NONE;
            continue;
        }

        if (section == STATES) {
            // lista de estados en una o varias líneas (se permiten espacios)
            char *tok = strtok(line, " \t");
            while (tok) { add_state(tok); tok = strtok(NULL, " \t"); }
        }
        else if (section == INITIAL) {
            char name[MAX_NAME]; sscanf(line, "%31s", name);
            add_state(name);                           // por si no apareció arriba
            initial = index_of_state(name);
        }
        else if (section == FINALS) {
            char *tok = strtok(line, " \t");
            while (tok) {
                add_state(tok);
                int idx = index_of_state(tok);
                expect(idx != -1, "estado final no declarado");
                is_final[idx] = 1;
                tok = strtok(NULL, " \t");
            }
        }
        else if (section == TRANSITIONS) {
            // formato: origen <espacio> simbolo <espacio> destino
            char a[MAX_NAME], b[MAX_NAME]; char c;
            if (sscanf(line, "%31s %c %31s", a, &c, b) == 3) {
                expect(c == '0' || c == '1', "símbolo distinto de 0/1");
                add_state(a); add_state(b);
                int i = index_of_state(a), j = index_of_state(b);
                expect(i!=-1 && j!=-1, "estado en transición no declarado");
                delta[i][c - '0'] = j;
            }
        }
    }
    fclose(f);

    expect(num_states > 0, "no se definieron estados");
    expect(initial != -1, "no se definió estado inicial");

    // Validar que cada estado tenga definición para 0 y 1
    for (int i = 0; i < num_states; i++) {
        expect(delta[i][0] != -1, "falta transición con 0");
        expect(delta[i][1] != -1, "falta transición con 1");
    }
}

// ---------- Simulación ----------
static int run(const char *w) {
    int q = initial;
    for (int i = 0; w[i]; i++) {
        if (w[i] == '0' || w[i] == '1') {
            q = delta[q][w[i]-'0'];
        } else {
            // símbolo inválido: rechazamos
            return 0;
        }
    }
    return is_final[q];
}

static void dump_table(void) {
    printf("\n== AFD cargado ==\n");
    printf("Estados: ");
    for (int i = 0; i < num_states; i++) {
        printf("%s%s%s ", (i==initial)?"[":"", state_names[i], is_final[i]?"+":"]");
        if (i!=initial && !is_final[i]) {} // solo para suprimir warning
    }
    printf("\nTransiciones (q,0)-> | (q,1)->\n");
    for (int i = 0; i < num_states; i++) {
        printf("%-8s  %-8s | %-8s\n",
            state_names[i],
            state_names[ delta[i][0] ],
            state_names[ delta[i][1] ]);
    }
    printf("\n");
}

int main(void) {
    load_config("Conf.txt");
    dump_table();  // comenta esta línea si no quieres imprimir la tabla

    FILE *g = fopen("Cadenas.txt", "r");
    if (!g) { perror("Cadenas.txt"); return 1; }

    char line[MAX_LINE];
    while (fgets(line, sizeof(line), g)) {
        trim(line);
        if (line[0] == '\0') continue;   // líneas vacías
        // Permitir 'E' como cadena vacía
        if (strcmp(line, "E") == 0) line[0] = '\0';
        printf("%s -> %s\n", (line[0]? line : "ε"), run(line) ? "Acepta" : "NO acepta");
    }
    fclose(g);
    return 0;
}

