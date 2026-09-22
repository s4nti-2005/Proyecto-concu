#!/usr/bin/env python3
"""secuencial.py — Hito 1
Suma secuencial de un vector grande de float. Referencia completa y ejecutable.

Uso:
  python secuencial.py [N]
N por defecto: 2^24 (16 777 216), el mismo orden de magnitud que en las prácticas.
"""

import sys
import time


def suma_secuencial(data):
    """Acumula en double para no perder precisión con N grande."""
    acc = 0.0
    for val in data:
        acc += val
    return float(acc)


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else (1 << 24)
    if n < 1:
        print("N debe ser >= 1", file=sys.stderr)
        return 1

    a = [1.0] * n

    t0 = time.perf_counter()
    suma = suma_secuencial(a)
    t1 = time.perf_counter()
    ms = (t1 - t0) * 1000

    print(f"N={n} suma={suma} tiempo_ms={ms:.3f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())