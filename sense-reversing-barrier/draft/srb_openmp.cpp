// openmp_barrier.cpp
#include <iostream>
#include <mutex>
#include <omp.h>

std::mutex print_mutex;

void openmp_worker(int id, int n_threads) {
    #pragma omp barrier
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "OpenMP: Thread " << id << " passed the barrier.\n";
    }
}

int main() {
    constexpr int n_threads = 4;  // Set thread count

    // Measure OpenMP Barrier
    std::cout << "Testing OpenMP Barrier:\n";
    #pragma omp parallel num_threads(n_threads)
    openmp_worker(omp_get_thread_num(), n_threads);

    return 0;
}
