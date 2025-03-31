#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <iomanip>

void worker(int id) {
    // No need to manually manage barriers since OpenMP will handle it
}

int main() {
    std::vector<int> n_threads_array = {1, 2, 4};
    std::vector<int> n_iterations_array = {1, 100, 1000, 10000};

    std::cout << std::setw(12) << "n_threads" 
              << std::setw(15) << "n_iterations" 
              << std::setw(20) << "duration" 
              << std::setw(23) << "throughput" << std::endl;

    for (int n_threads : n_threads_array) {
        for (int n_iterations : n_iterations_array) {
            omp_set_num_threads(n_threads);

            auto start_time = std::chrono::high_resolution_clock::now();

            for (int iter = 0; iter < n_iterations; ++iter) {
                #pragma omp parallel
                {
                    int id = omp_get_thread_num();
                    worker(id);

                    #pragma omp barrier
                }
            }

            auto end_time = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> duration = end_time - start_time;
            double throughput = (n_threads * n_iterations) / duration.count();

            std::cout << std::setw(12) << n_threads
                      << std::setw(15) << n_iterations
                      << std::setw(20) << duration.count()
                      << std::setw(23) << throughput << std::endl;
        }
    }

    return 0;
}
