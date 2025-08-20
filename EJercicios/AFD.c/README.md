# 📝 Simulador de AFD en C

Este proyecto implementa un **simulador de Autómatas Finitos Deterministas (AFD)** en lenguaje **C**.  
La configuración del autómata (estados, transiciones, inicial y finales) se lee desde un archivo de texto (`afd.txt`) y las cadenas a evaluar desde otro (`cadenas.txt`).  
De esta forma, el código es **flexible** y no está “quemado” en el `.c`.

---


## 📖 Estructura de `afd.txt`

Ejemplo de definición de un AFD:


<img width="379" height="344" alt="Screenshot From 2025-08-20 12-55-02" src="https://github.com/user-attachments/assets/449e9a17-dd2b-4834-8fec-3017a9a65cfd" />

---

- **Estados:** lista de todos los estados del autómata.  
- **Inicial:** el estado de inicio.  
- **Acepta:** lista de estados de aceptación.  
- **Transiciones:** cada línea indica `(estado_origen, símbolo, estado_destino)`.

---

## 📖 Estructura de `cadenas.txt`

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
