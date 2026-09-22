#!/usr/bin/env python3
"""paralelo.py — Hito 1
Esqueleto multi-hilo (threading) de la suma del mismo vector que secuencial.py.

Ejecución:
  python paralelo.py [N] [T]
T = cantidad de hilos (por defecto 4).

NOTA: Este esqueleto NO cumple el enunciado hasta completar los TODO:
hace falta al menos una primitiva de sincronización (Lock, Condition, etc.).
El join de los hilos no alcanza.
"""

import sys
import threading


# Índice compartido para despacho dinámico de bloques (opción recomendada).
# Sin el Lock, varios hilos pueden leer/escribir `proximo` a la vez → carrera.
mtx = threading.Lock()
proximo = 0

# Tamaño de bloque de trabajo. Elegir y justificar en el informe
# (muy chico: contención del mutex; muy grande: desbalance si N no es múltiplo).
BLOQUE = 1 << 16


def worker(data, n, parcial, idx):
    global proximo
    acc = 0.0

    while True:
        lo = 0
        hi = 0

        with mtx:
            # TODO: tomar un rango [lo, hi) de forma exclusiva:
            #   1. Si proximo >= n, no hay más trabajo → salir del while
            #   2. lo = proximo; avanzar proximo en BLOQUE (sin pasarse de n)
            #      hi = proximo
            #
            # El for de abajo tiene que quedar FUERA del candado. Si sumás
            # adentro del lock, serializás todo (Amdahl con s ≈ 1).
            if proximo >= n:
                break
            lo = proximo
            proximo = min(proximo + BLOQUE, n)
            hi = proximo

        for i in range(lo, hi):
            acc += data[i]

    # Cada hilo escribe en su propia entrada de `parciales` (sin aliasing).
    parcial[idx] = float(acc)


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else (1 << 24)
    t = int(sys.argv[2]) if len(sys.argv) > 2 else 4
    if n < 1 or t < 1:
        print("N >= 1 y T >= 1", file=sys.stderr)
        return 1

    a = [1.0] * n
    parciales = [0.0] * t
    hilos = []

    global proximo
    proximo = 0
    for i in range(t):
        h = threading.Thread(target=worker, args=(a, n, parciales, i))
        hilos.append(h)
        h.start()

    for h in hilos:
        h.join()

    # Reduce serial de T parciales. Con T chico (núcleos de una notebook)
    # no hace falta lock acá: este hilo es el único que queda vivo.
    total = sum(parciales)

    print(f"N={n} T={t} suma={total}")
    return 0


if __name__ == "__main__":
    sys.exit(main())