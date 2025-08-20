import sys

MAX_STATES = 64
MAX_NAME   = 32
MAX_LINE   = 256

# --- Almacenamiento del AFD ---
state_names = []
num_states = 0
initial = -1
is_final = []
delta = []

# ---------- Utilidades ----------
def rtrim(s: str) -> str:
    return s.rstrip("\n\r ").rstrip()

def ltrim(s: str) -> str:
    return s.lstrip()

def trim(s: str) -> str:
    return ltrim(rtrim(s))

def index_of_state(name: str) -> int:
    try:
        return state_names.index(name)
    except ValueError:
        return -1

def add_state(name: str):
    global num_states
    if index_of_state(name) != -1:
        return
    if num_states >= MAX_STATES:
        sys.stderr.write(f"Demasiados estados (>{MAX_STATES})\n")
        sys.exit(1)
    state_names.append(name)
    is_final.append(0)
    delta.append([-1, -1])
    num_states += 1

def expect(cond: bool, msg: str):
    if not cond:
        sys.stderr.write(f"Error de configuración: {msg}\n")
        sys.exit(1)

# ---------- Parseo de Conf.txt ----------
def load_config(path: str):
    global num_states, initial, is_final, delta
    with open(path, "r") as f:
        lines = f.readlines()

    # inicializar
    num_states = 0
    initial = -1
    state_names.clear()
    is_final.clear()
    delta.clear()

    section = None
    for line in lines:
        line = trim(line)
        if not line:
            continue
        if line.startswith("#"):
            if "Estados de aceptación" in line:
                section = "FINALS"
            elif "Estado inicial" in line:
                section = "INITIAL"
            elif "Transiciones" in line:
                section = "TRANSITIONS"
            elif "Estados" in line:
                section = "STATES"
            else:
                section = None
            continue

        if section == "STATES":
            for tok in line.split():
                add_state(tok)
        elif section == "INITIAL":
            name = line.split()[0]
            add_state(name)
            initial = index_of_state(name)
        elif section == "FINALS":
            for tok in line.split():
                add_state(tok)
                idx = index_of_state(tok)
                expect(idx != -1, "estado final no declarado")
                is_final[idx] = 1
        elif section == "TRANSITIONS":
            parts = line.split()
            if len(parts) == 3:
                a, c, b = parts
                expect(c in ['0', '1'], "símbolo distinto de 0/1")
                add_state(a)
                add_state(b)
                i = index_of_state(a)
                j = index_of_state(b)
                expect(i != -1 and j != -1, "estado en transición no declarado")
                delta[i][int(c)] = j

    expect(num_states > 0, "no se definieron estados")
    expect(initial != -1, "no se definió estado inicial")

    for i in range(num_states):
        expect(delta[i][0] != -1, "falta transición con 0")
        expect(delta[i][1] != -1, "falta transición con 1")

# ---------- Simulación ----------
def run(w: str) -> bool:
    q = initial
    for ch in w:
        if ch in ['0', '1']:
            q = delta[q][int(ch)]
        else:
            return False
    return bool(is_final[q])

def dump_table():
    print("\n== AFD cargado ==")
    print("Estados: ", end="")
    for i in range(num_states):
        ini = "[" if i == initial else ""
        fin = "+" if is_final[i] else "]" if i == initial else ""
        print(f"{ini}{state_names[i]}{fin} ", end="")
    print("\nTransiciones (q,0)-> | (q,1)->")
    for i in range(num_states):
        print(f"{state_names[i]:8}  {state_names[delta[i][0]]:8} | {state_names[delta[i][1]]:8}")
    print()

# ---------- Main ----------
def main():
    load_config("Conf.txt")
    dump_table()  # comenta esta línea si no quieres imprimir la tabla

    try:
        with open("Cadenas.txt", "r") as g:
            for line in g:
                line = trim(line)
                if not line:
                    continue
                if line == "E":
                    line = ""
                resultado = "Acepta" if run(line) else "NO acepta"
                print(f"{line if line else 'ε'} -> {resultado}")
    except FileNotFoundError:
        sys.stderr.write("No se encontró Cadenas.txt\n")
        sys.exit(1)

if __name__ == "__main__":
    main()
