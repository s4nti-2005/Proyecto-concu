// benchmark.cpp — Hito 1
// Mide tiempo de la version secuencial y de la paralela al variar T,
// e imprime speedup S(T) = T_seq / T_par(T). Usa mediana de >= 3 corridas
// por punto para evitar que un pico puntual (arranque en frio, scheduler
// del SO, etc.) distorsione la curva.
//
// Uso:
//   ./benchmark [N] [repeticiones]
// repeticiones por defecto: 5

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using clock_type = std::chrono::steady_clock;

std::mutex mtx;
int proximo = 0;
const int BLOQUE = 1 << 16;

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
            std::lock_guard<std::mutex> lock(mtx);
            if (proximo >= n) {
                break;
            }
            lo = proximo;
            hi = std::min(proximo + BLOQUE, n);
            proximo = hi;
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

// Mediana de un vector de tiempos (en ms). Copia y ordena, no modifica el original.
double mediana(std::vector<double> v) {
    std::sort(v.begin(), v.end());
    const std::size_t m = v.size() / 2;
    if (v.size() % 2 == 0) {
        return (v[m - 1] + v[m]) / 2.0;
    }
    return v[m];
}

int main(int argc, char** argv) {
    const int n = (argc > 1) ? std::atoi(argv[1]) : (1 << 24);
    const int reps = (argc > 2) ? std::atoi(argv[2]) : 5;
    if (n < 1) {
        std::cerr << "N >= 1\n";
        return 1;
    }
    if (reps < 3) {
        std::cerr << "El enunciado pide mediana de >= 3 corridas; usando 3.\n";
    }
    const int r = std::max(reps, 3);

    const unsigned hw = std::thread::hardware_concurrency();
    std::vector<float> a(static_cast<std::size_t>(n), 1.0f);

    // --- Secuencial: r corridas, guardamos la mediana ---
    std::vector<double> tiempos_seq;
    tiempos_seq.reserve(static_cast<std::size_t>(r));
    float s_seq = 0.0f;
    for (int k = 0; k < r; ++k) {
        const auto t0 = clock_type::now();
        s_seq = suma_secuencial(a.data(), n);
        const auto t1 = clock_type::now();
        tiempos_seq.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
    }
    const double ms_seq = mediana(tiempos_seq);

    std::cout << "hardware_concurrency=" << hw << " N=" << n << " repeticiones=" << r << '\n';
    std::cout << "hilos,ms_mediana,speedup,suma\n";
    std::cout << "seq," << ms_seq << ",1.00," << s_seq << '\n';

    const int candidatos[] = {1, 2, 4, 8, 16};
    for (int t : candidatos) {
        if (hw != 0 && static_cast<unsigned>(t) > hw * 2) {
            continue;  // evita sobre-suscripcion extrema en maquinas chicas
        }
        std::vector<double> tiempos_par;
        tiempos_par.reserve(static_cast<std::size_t>(r));
        float s_par = 0.0f;
        for (int k = 0; k < r; ++k) {
            const auto u0 = clock_type::now();
            s_par = suma_par(a.data(), n, t);
            const auto u1 = clock_type::now();
            tiempos_par.push_back(std::chrono::duration<double, std::milli>(u1 - u0).count());
        }
        const double ms_par = mediana(tiempos_par);
        const double speedup = (ms_par > 0.0) ? (ms_seq / ms_par) : 0.0;
        std::cout << t << "," << ms_par << "," << speedup << "," << s_par << '\n';
    }
    return 0;
}
