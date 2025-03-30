#include <iostream>
#include <omp.h>
#include <vector>
#include <chrono>

int counter = 0;

void increment_omp(int times) {
    #pragma omp parallel for reduction(+:counter)
    for (int i = 0; i < times; ++i) {
        ++counter;
    }
}

void run_test_omp(int times, int num_threads) {
    auto start = std::chrono::high_resolution_clock::now();

    // Set number of threads for OpenMP
    omp_set_num_threads(num_threads);

    increment_omp(times);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Times: " << times << "\tThreads: " << num_threads 
    << "\tExecution Time (ms): " << duration.count() * 1000          
    << "\tCounter: " << counter << std::endl;
}

int main() {
    for (int times : {1000, 100000, 10000000}) {
        for (int num_threads : {1, 2, 4, 8}) {
            counter = 0;
            run_test_omp(times, num_threads);
        }
    }

    return 0;
}
