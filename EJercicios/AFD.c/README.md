# Simulador de AFD en C

Este proyecto implementa un **simulador de Autómatas Finitos Deterministas (AFD)** en lenguaje **C**.  
La configuración del autómata (estados, transiciones, inicial y finales) se lee desde un archivo de texto (`afd.txt`) y las cadenas a evaluar desde otro (`cadenas.txt`).  
De esta forma, el código es **flexible** y no está “quemado” en el `.c`.

---


##  Estructura de `Conf.txt`

Ejemplo de definición de un AFD:


<img width="379" height="344" alt="Screenshot From 2025-08-20 12-55-02" src="https://github.com/user-attachments/assets/449e9a17-dd2b-4834-8fec-3017a9a65cfd" />

---

- **Estados:** lista de todos los estados del autómata.  
- **Inicial:** el estado de inicio.  
- **Acepta:** lista de estados de aceptación.  
- **Transiciones:** cada línea indica `(estado_origen, símbolo, estado_destino)`.

---

##  Estructura de `cadenas.txt`

Contiene las cadenas a evaluar:

<img width="381" height="274" alt="image" src="https://github.com/user-attachments/assets/2e36b895-69cd-406a-ae85-ecc3f651ad49" />

---

---

## Explicación del código `AFD.c`

### 1. **Librerías incluidas**
    ```c
    #include <stdio.h>   // Manejo de entrada/salida: printf, scanf, fopen, fgets
    #include <stdlib.h>  // Manejo de memoria dinámica, exit()
    #include <string.h>  // Manejo de cadenas: strcmp, strtok, strcpy
- stdio.h: se usa para leer archivos (fopen, fgets) y escribir en consola (printf).

- stdlib.h: se usa para terminar el programa si hay un error (exit) y para memoria.

- string.h: se usa para comparar (strcmp) y copiar cadenas (strcpy, strtok).

## 2. **Estructuras globales**
El programa guarda la configuración del AFD en arreglos globales:
    
    ```c
    char estados[20][10];        // Lista de estados (máx. 20 estados, nombre máx. 10 chars)
    char inicial[10];            // Estado inicial
    char acepta[20][10];         // Estados de aceptación
    char transiciones[50][3][10];// Transiciones: [origen][símbolo][destino]
    int numEstados, numAcepta, numTransiciones;

- Se usan arreglos de cadenas porque los estados se llaman con nombres como "q1", "q2".

- transiciones[i][0] = estado origen,
  transiciones[i][1] = símbolo,
  transiciones[i][2] = estado destino.
  
## 3.Función cargarAFD(FILE *f)
    ```c
    void cargarAFD(FILE *f) {
       char linea[100];
       while(fgets(linea, sizeof(linea), f)) {
           if(strncmp(linea, "Estados:", 8) == 0) { ... }
           else if(strncmp(linea, "Inicial:", 8) == 0) { ... }
           else if(strncmp(linea, "Acepta:", 7) == 0) { ... }
           else if(strncmp(linea, "Transiciones:", 13) == 0) { ... }
       }
    }

- Usa fgets para leer línea por línea del archivo afd.txt.
- Con strncmp detecta si la línea define estados, inicial, acepta o transiciones.
- Usa strtok para dividir las palabras separadas por espacio.
- Cada token se guarda en los arreglos globales.

## 4.Función esAceptado(char *cadena)
    int esAceptado(char *cadena) {
      char estadoActual[10];
      strcpy(estadoActual, inicial); // comenzar en el inicial
      for(int i = 0; i < strlen(cadena); i++) {
        char simbolo[2] = { cadena[i], '\0' };
        int encontrado = 0;
        for(int j = 0; j < numTransiciones; j++) {
            if(strcmp(transiciones[j][0], estadoActual) == 0 &&
               strcmp(transiciones[j][1], simbolo) == 0) {
                strcpy(estadoActual, transiciones[j][2]);
                encontrado = 1;
                break;
            }
      }
        if(!encontrado) return 0; // no existe transición
     }
     for(int k = 0; k < numAcepta; k++) {
        if(strcmp(estadoActual, acepta[k]) == 0) return 1;
     }
    return 0;
    }
    
- Se inicializa el estado actual con el estado inicial (strcpy).

- Recorre símbolo por símbolo de la cadena (for i).

- Busca en la tabla de transiciones la que corresponda al estado actual y el símbolo.

- Si no hay transición válida, retorna NO acepta.

- Al final, revisa si el estado actual está en la lista de aceptación.
  
## **5. Función main()**
    int main() {
      FILE *fa = fopen("afd.txt", "r");
      if(!fa) { perror("Error al abrir afd.txt"); exit(1); }
      cargarAFD(fa);
      fclose(fa);

      printf("== AFD cargado ==\n");
      ...

      FILE *fc = fopen("cadenas.txt", "r");
      if(!fc) { perror("Error al abrir cadenas.txt"); exit(1); }
      char cadena[100];
      while(fgets(cadena, sizeof(cadena), fc)) {
          cadena[strcspn(cadena, "\n")] = 0; // quitar salto de línea
          printf("%s -> %s\n", cadena, esAceptado(cadena) ? "Acepta" : "NO acepta");
      }
      fclose(fc);
    }
    
- Abre Conf.txt, llama a cargarAFD y cierra.

- Muestra la tabla del autómata cargado.

- Abre cadenas.txt y lee cada línea.

- Para cada cadena, llama a esAceptado y muestra el resultado.


### **COMPILACION Y EJECUCION**

<img width="407" height="484" alt="Screenshot From 2025-08-20 13-12-15" src="https://github.com/user-attachments/assets/446d4d04-c99c-4108-a7b9-fbbcbb264a83" />
