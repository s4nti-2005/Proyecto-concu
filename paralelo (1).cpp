// paralelo.cpp — Hito 1
// Version multi-hilo (std::thread) de la suma del mismo vector que secuencial.cpp.
//
// Uso:
//   ./paralelo [N] [T]
// T = cantidad de hilos (por defecto 4).

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Indice compartido para despacho dinamico de bloques.
// Sin el candado, varios hilos pueden leer/escribir `proximo` a la vez -> carrera.
std::mutex mtx;
int proximo = 0;

// Tamano de bloque de trabajo. 1<<16 = 65536 elementos por bloque.
// Muy chico: contencion del mutex (muchos hilos pidiendo turno seguido).
// Muy grande: desbalance de carga si N no es multiplo exacto de BLOQUE*T.
const int BLOQUE = 1 << 16;

void worker(const float* data, int n, float& parcial) {
    double acc = 0.0;

    while (true) {
        int lo = 0;
        int hi = 0;

        {
            std::lock_guard<std::mutex> lock(mtx);
            if (proximo >= n) {
                break;  // no queda trabajo: salimos del while (y liberamos el lock)
            }
            lo = proximo;
            hi = std::min(proximo + BLOQUE, n);
            proximo = hi;
        }
        // <- el lock_guard se destruye al cerrar el bloque de arriba: el mutex
        //    ya esta liberado antes de entrar al for. Esto es lo que evita
        //    serializar la suma (si sumaramos adentro del lock, s ~= 1).

        for (int i = lo; i < hi; ++i) {
            acc += static_cast<double>(data[i]);
        }
    }

    // Cada hilo escribe en su propia entrada de `parciales` (sin aliasing),
    // asi que esto no necesita proteccion.
    parcial = static_cast<float>(acc);
}

int main(int argc, char** argv) {
    const int n = (argc > 1) ? std::atoi(argv[1]) : (1 << 24);
    const int t = (argc > 2) ? std::atoi(argv[2]) : 4;
    if (n < 1 || t < 1) {
        std::cerr << "N >= 1 y T >= 1\n";
        return 1;
    }

    std::vector<float> a(static_cast<std::size_t>(n), 1.0f);
    std::vector<float> parciales(static_cast<std::size_t>(t), 0.0f);
    std::vector<std::thread> hilos;
    hilos.reserve(static_cast<std::size_t>(t));

    proximo = 0;
    for (int i = 0; i < t; ++i) {
        hilos.emplace_back(worker, a.data(), n, std::ref(parciales[static_cast<std::size_t>(i)]));
    }
    for (auto& h : hilos) {
        h.join();
    }

    // Reduce serial de T parciales. Con T chico (nucleos de una notebook)
    // no hace falta mutex aca: este hilo es el unico que queda vivo.
    double total = 0.0;
    for (float p : parciales) {
        total += static_cast<double>(p);
    }

    std::cout << "N=" << n << " T=" << t << " suma=" << static_cast<float>(total) << '\n';
    return 0;
}
