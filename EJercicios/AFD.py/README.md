# Simulador de AFD en Python

Este proyecto implementa un **Autómata Finito Determinista (AFD)** en Python.  
El programa lee dos archivos:

- **Conf.txt** → Contiene la definición del AFD (estados, inicial, finales y transiciones).  
- **Cadenas.txt** → Contiene las cadenas binarias (alfabeto {0,1}) que se quieren evaluar.  

El simulador verifica si cada cadena es aceptada o rechazada según el autómata definido.

---

##  Archivos del proyecto

- `afd.py` → Código principal en Python.  
- `Conf.txt` → Archivo de configuración del autómata.  
- `Cadenas.txt` → Lista de cadenas a evaluar.  

---

##  Estructura de `Conf.txt`

El archivo debe seguir este formato (puedes cambiar nombres y transiciones según tu AFD):

<img width="344" height="345" alt="image" src="https://github.com/user-attachments/assets/7c9413a4-527e-4287-8306-be205d0d4968" />


 Explicación:
- `# Estados` → Lista de estados (ejemplo: `q0 q1 q2`).  
- `# Estado inicial` → Nombre de un único estado inicial.  
- `# Estados de aceptación` → Estados finales (puede haber más de uno).  
- `# Transiciones` → Una por línea, con el formato:
- 

El alfabeto es binario (`0` y `1`).  

---

## Estructura de `Cadenas.txt`

Ejemplo de archivo con cadenas de entrada:

<img width="308" height="285" alt="image" src="https://github.com/user-attachments/assets/76e16096-a03e-4b35-8d9d-70558210333f" />

### Explicación detallada del código (afd.py)

1. ## **Variables globales**
      ```py
       state_names = []
       num_states = 0
       initial = -1
       is_final = {}
       delta = {}
- state_names → Nombres de los estados.
- num_states → Número total de estados.
- initial → Índice del estado inicial.
- is_final → Diccionario que marca qué estados son finales.
- delta → Diccionario de transiciones con clave (estado, símbolo).

2. ## **Funciones de utilidad**

- trim(line) → Elimina espacios en blanco al inicio y final de cada línea.
- index_of_state(name) → Devuelve el índice de un estado.
- add_state(name) → Agrega un estado nuevo si no existe.
- expect(cond, msg) → Lanza un error si no se cumple una condición.

4. ## **Carga del archivo de configuración**
       def load_config(path):
- Abre y lee Conf.txt.
- Reconoce las secciones: Estados, Estado inicial, Estados de aceptación, Transiciones.
- Llena las estructuras globales (state_names, initial, is_final, delta).
- Valida que cada estado tenga transiciones definidas con 0 y 1.

5. ## **Simulación del AFD**
       def run(w):
- Recorre la cadena w carácter por carácter.
- Usa la tabla delta para cambiar de estado según el símbolo leído.
- Si se encuentra un carácter distinto de 0 o 1, se rechaza la cadena.
- Al final, se acepta si el estado final alcanzado está en is_final.

6. ## **Impresión de tabla de transiciones**
       def dump_table():
- Muestra todos los estados (con [] el inicial y + los de aceptación).
- Imprime la tabla de transiciones (estado,0)-> y (estado,1)->.

7. ## **Función principal**
       def main():

- Carga la configuración (Conf.txt).
- Imprime la tabla de transiciones.
- Abre Cadenas.txt y procesa cada línea.
- Para cada cadena imprime si Acepta o NO acepta.

### **COMPILACION Y EJECUCION**

<img width="354" height="419" alt="image" src="https://github.com/user-attachments/assets/3c5e4e1b-0f20-44e7-815d-9de831c5b9a0" />
