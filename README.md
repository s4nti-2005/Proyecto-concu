# Hito 1 — Concurrencia en CPU


**Objetivo:** demostrar dominio de hilos y sincronización.

## Enunciado

1. Problema con paralelismo de **datos** claro. Este esqueleto usa **suma de un vector grande** de `float` (el mismo *shape* que las Clases 2 y 4, y que el Hito 2). Pueden cambiar a multiplicación de vectores, conteo de palabras o una operación matricial simple: actualizá este README si lo hacen.
2. Versión **secuencial** (`secuencial.cpp`, ya completa) y versión **multi-hilo** (`paralelo.cpp`, esqueleto con `TODO`).
3. Al menos **una** primitiva de sincronización de la Clase 3 (`std::mutex`, variable de condición, o monitor). OpenMP es válido si lo justificás en el informe.
4. Medir aceleración \( S(T) = T_{\mathrm{seq}} / T(T) \) vs. número de hilos y comparar con la Ley de Amdahl, con una fracción secuencial \( s \) **declarada** (estimada o barrida). Las columnas ilustrativas de las slides **no** son tus mediciones.

## Entregable

- Código que **compila y corre**
- Informe de 1–2 páginas: curva medida vs. teórica y **por qué** difieren
- Tabla de tiempos (mediana de ≥ 3 corridas), `hardware_concurrency()`, flags de compilación

## Compilación

```bash
make
./secuencial 16777216
./paralelo 16777216 4
./benchmark 16777216
```

Compilador C++17. En Linux, `make` agrega `-pthread`. Windows (Developer Command Prompt):

```text
cl /std:c++17 /O2 /EHsc secuencial.cpp
cl /std:c++17 /O2 /EHsc paralelo.cpp
cl /std:c++17 /O2 /EHsc benchmark.cpp
```

## Criterio de “listo”

- [ ] `secuencial` y `paralelo` dan la misma suma (tolerancia float; con vector de unos, el total esperado es \( N \))
- [ ] `paralelo` usa una primitiva de sincronización **real** (no un comentario). El `break` placeholder tiene que desaparecer.
- [ ] `benchmark` imprime \( T(T) \) y \( S(T) \) para varios \( T \)
- [ ] informe con Amdahl y análisis (memoria, *join*, sobre-suscripción, \( s \), tamaño de `BLOQUE`, …)

## Esqueleto

| Archivo | Qué hay |
|---|---|
| `secuencial.cpp` | referencia completa |
| `paralelo.cpp` | `std::thread` + cola dinámica a medias: los `TODO` son el despacho con mutex |
| `benchmark.cpp` | mide seq vs. par variando hilos; **copiá** tu worker cuando lo termines |
| `Makefile` | `secuencial`, `paralelo`, `benchmark` |

Hasta que completes los `TODO`, `./paralelo` imprime `suma=0`: compila a propósito, no está resuelto. Dejar el placeholder = no cumple el enunciado.
