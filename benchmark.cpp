// benchmark.cpp — Hito 1
// Mide tiempo de la versión secuencial y de la paralela al variar T,
// e imprime speedup S(T) = T_seq / T_par(T).
//
// Uso:
//   ./benchmark [N]
//
// Cuando termines paralelo.cpp, copiá tu `worker` (o el patrón que hayas
// elegido) a suma_par() para que esta curva sea la de TU implementación.

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using clock_type = std::chrono::steady_clock;

[[maybe_unused]] std::mutex mtx;
int proximo = 0;
[[maybe_unused]] const int BLOQUE = 1 << 16;

float suma_secuencial(const float* data, int n) {
    double acc = 0.0;
    for (int i = 0; i < n; ++i) {
        acc += static_cast<double>(data[i]);
    }
    return static_cast<float>(acc);
}

void worker(const float* data, int n, float& parcial) {
    double acc = 0.0;
    while (true) {
        int lo = 0;
        int hi = 0;
        {
            // TODO: misma lógica de sincronización que en paralelo.cpp
            (void)n;
            break;
        }
        for (int i = lo; i < hi; ++i) {
            acc += static_cast<double>(data[i]);
        }
    }
    parcial = static_cast<float>(acc);
}

float suma_par(const float* data, int n, int t) {
    std::vector<float> parciales(static_cast<std::size_t>(t), 0.0f);
    std::vector<std::thread> hilos;
    hilos.reserve(static_cast<std::size_t>(t));
    proximo = 0;
    for (int i = 0; i < t; ++i) {
        hilos.emplace_back(worker, data, n, std::ref(parciales[static_cast<std::size_t>(i)]));
    }
    for (auto& h : hilos) {
        h.join();
    }
    double total = 0.0;
    for (float p : parciales) {
        total += static_cast<double>(p);
    }
    return static_cast<float>(total);
}

int main(int argc, char** argv) {
    const int n = (argc > 1) ? std::atoi(argv[1]) : (1 << 24);
    if (n < 1) {
        std::cerr << "N >= 1\n";
        return 1;
    }

    const unsigned hw = std::thread::hardware_concurrency();
    std::vector<float> a(static_cast<std::size_t>(n), 1.0f);

    // TODO: el enunciado pide mediana de >= 3 corridas. Este esqueleto mide
    // una sola pasada (incluye el arranque en frío). Repetí y reemplazá.
    const auto t0 = clock_type::now();
    const float s_seq = suma_secuencial(a.data(), n);
    const auto t1 = clock_type::now();
    const double ms_seq = std::chrono::duration<double, std::milli>(t1 - t0).count();

    std::cout << "hardware_concurrency=" << hw << " N=" << n << '\n';
    std::cout << "hilos,ms,speedup,suma\n";
    std::cout << "seq," << ms_seq << ",1.00," << s_seq << '\n';

    const int candidatos[] = {1, 2, 4, 8, 16};
    for (int t : candidatos) {
        if (hw != 0 && static_cast<unsigned>(t) > hw * 2) {
            continue;  // evita sobre-suscripción extrema en máquinas chicas
        }
        const auto u0 = clock_type::now();
        const float s_par = suma_par(a.data(), n, t);
        const auto u1 = clock_type::now();
        const double ms_par = std::chrono::duration<double, std::milli>(u1 - u0).count();
        const double speedup = (ms_par > 0.0) ? (ms_seq / ms_par) : 0.0;
        std::cout << t << "," << ms_par << "," << speedup << "," << s_par << '\n';
    }
    return 0;
}
