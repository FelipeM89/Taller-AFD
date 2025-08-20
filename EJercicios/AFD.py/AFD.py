import sys

MAX_ESTADOS = 64
MAX_NOMBRE  = 32
MAX_LINEA   = 256

# --- Almacenamiento del AFD ---
nombres_estados = []
num_estados = 0
inicial = -1
es_final = []
transicion = []

# ---------- Utilidades ----------
def recortar_derecha(s: str) -> str:
    return s.rstrip("\n\r ").rstrip()

def recortar_izquierda(s: str) -> str:
    return s.lstrip()

def recortar(s: str) -> str:
    return recortar_izquierda(recortar_derecha(s))

def indice_estado(nombre: str) -> int:
    try:
        return nombres_estados.index(nombre)
    except ValueError:
        return -1

def agregar_estado(nombre: str):
    global num_estados
    if indice_estado(nombre) != -1:
        return
    if num_estados >= MAX_ESTADOS:
        sys.stderr.write(f"Demasiados estados (>{MAX_ESTADOS})\n")
        sys.exit(1)
    nombres_estados.append(nombre)
    es_final.append(0)
    transicion.append([-1, -1])
    num_estados += 1

def esperar(cond: bool, mensaje: str):
    if not cond:
        sys.stderr.write(f"Error de configuración: {mensaje}\n")
        sys.exit(1)

# ---------- Parseo de Conf.txt ----------
def cargar_config(ruta: str):
    global num_estados, inicial, es_final, transicion
    with open(ruta, "r") as f:
        lineas = f.readlines()

    # inicializar
    num_estados = 0
    inicial = -1
    nombres_estados.clear()
    es_final.clear()
    transicion.clear()

    seccion = None
    for linea in lineas:
        linea = recortar(linea)
        if not linea:
            continue
        if linea.startswith("#"):
            if "Estados de aceptación" in linea:
                seccion = "FINALES"
            elif "Estado inicial" in linea:
                seccion = "INICIAL"
            elif "Transiciones" in linea:
                seccion = "TRANSICIONES"
            elif "Estados" in linea:
                seccion = "ESTADOS"
            else:
                seccion = None
            continue

        if seccion == "ESTADOS":
            for tok in linea.split():
                agregar_estado(tok)
        elif seccion == "INICIAL":
            nombre = linea.split()[0]
            agregar_estado(nombre)
            inicial = indice_estado(nombre)
        elif seccion == "FINALES":
            for tok in linea.split():
                agregar_estado(tok)
                idx = indice_estado(tok)
                esperar(idx != -1, "estado final no declarado")
                es_final[idx] = 1
        elif seccion == "TRANSICIONES":
            partes = linea.split()
            if len(partes) == 3:
                a, c, b = partes
                esperar(c in ['0', '1'], "símbolo distinto de 0/1")
                agregar_estado(a)
                agregar_estado(b)
                i = indice_estado(a)
                j = indice_estado(b)
                esperar(i != -1 and j != -1, "estado en transición no declarado")
                transicion[i][int(c)] = j

    esperar(num_estados > 0, "no se definieron estados")
    esperar(inicial != -1, "no se definió estado inicial")

    for i in range(num_estados):
        esperar(transicion[i][0] != -1, "falta transición con 0")
        esperar(transicion[i][1] != -1, "falta transición con 1")

# ---------- Simulación ----------
def ejecutar(cadena: str) -> bool:
    q = inicial
    for ch in cadena:
        if ch in ['0', '1']:
            q = transicion[q][int(ch)]
        else:
            return False
    return bool(es_final[q])

def mostrar_tabla():
    print("\n== AFD cargado ==")
    print("Estados: ", end="")
    for i in range(num_estados):
        ini = "[" if i == inicial else ""
        fin = "+" if es_final[i] else "]" if i == inicial else ""
        print(f"{ini}{nombres_estados[i]}{fin} ", end="")
    print("\nTransiciones (q,0)-> | (q,1)->")
    for i in range(num_estados):
        print(f"{nombres_estados[i]:8}  {nombres_estados[transicion[i][0]]:8} | {nombres_estados[transicion[i][1]]:8}")
    print()

# ---------- Main ----------
def main():
    cargar_config("Conf.txt")
    mostrar_tabla()  # comenta esta línea si no quieres imprimir la tabla

    try:
        with open("Cadenas.txt", "r") as g:
            for linea in g:
                linea = recortar(linea)
                if not linea:
                    continue
                if linea == "E":
                    linea = ""
                resultado = "Acepta" if ejecutar(linea) else "NO acepta"
                print(f"{linea if linea else 'ε'} -> {resultado}")
    except FileNotFoundError:
        sys.stderr.write("No se encontró Cadenas.txt\n")
        sys.exit(1)

if __name__ == "__main__":
    main()
