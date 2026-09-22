#!/usr/bin/env python3
"""benchmark.py — Hito 1
Mide tiempo de la versión secuencial y de la paralela al variar T,
e imprime speedup S(T) = T_seq / T_par(T).

Uso:
  python benchmark.py [N]

Cuando termines paralelo.py, copiá tu `worker` (o el patrón que hayas
elegido) a suma_par() para que esta curva sea la de TU implementación.
"""

import sys
import threading
import time


# Copiados de paralelo.py para que el benchmark sea autocontenido
mtx = threading.Lock()
proximo = 0
BLOQUE = 1 << 16


def suma_secuencial(data):
    acc = 0.0
    for val in data:
        acc += val
    return float(acc)


def worker(data, n, parcial, idx):
    global proximo
    acc = 0.0
    while True:
        lo = 0
        hi = 0
        with mtx:
            if proximo >= n:
                break
            lo = proximo
            proximo = min(proximo + BLOQUE, n)
            hi = proximo
        for i in range(lo, hi):
            acc += data[i]
    parcial[idx] = float(acc)


def suma_par(data, n, t):
    parciales = [0.0] * t
    hilos = []

    global proximo
    proximo = 0
    for i in range(t):
        h = threading.Thread(target=worker, args=(data, n, parciales, i))
        hilos.append(h)
        h.start()

    for h in hilos:
        h.join()

    return sum(parciales)


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else (1 << 24)
    if n < 1:
        print("N >= 1", file=sys.stderr)
        return 1

    a = [1.0] * n

    # El enunciado pide mediana de >= 3 corridas. Este esqueleto mide
    # una sola pasada (incluye el arranque en frío). Repetí y reemplazá.
    t0 = time.perf_counter()
    s_seq = suma_secuencial(a)
    t1 = time.perf_counter()
    ms_seq = (t1 - t0) * 1000

    print(f"N={n}")
    print("hilos,ms,speedup,suma")
    print(f"seq,{ms_seq:.3f},1.00,{s_seq}")

    candidatos = [1, 2, 4, 8, 16]
    for t in candidatos:
        u0 = time.perf_counter()
        s_par = suma_par(a, n, t)
        u1 = time.perf_counter()
        ms_par = (u1 - u0) * 1000
        speedup = (ms_seq / ms_par) if ms_par > 0 else 0.0
        print(f"{t},{ms_par:.3f},{speedup:.2f},{s_par}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
