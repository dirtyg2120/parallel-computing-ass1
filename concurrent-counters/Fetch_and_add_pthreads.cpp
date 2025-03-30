#include <iostream>
#include <pthread.h>
#include <vector>
#include <chrono>

int counter = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void* increment_pthreads(void* arg) {
    int times = *(int*)arg;
    for (int i = 0; i < times; ++i) {
        pthread_mutex_lock(&mtx);
        ++counter;
        pthread_mutex_unlock(&mtx);
    }
    return nullptr;
}

void run_test_pthreads(int times, int num_threads) {
    auto start = std::chrono::high_resolution_clock::now();
    int base = times / num_threads;// 100/4 = 25
    int rem = times % num_threads;//0

    std::vector<pthread_t> threads(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        int iters = base + (i < rem ? 1 : 0);
        pthread_create(&threads[i], nullptr, increment_pthreads, &iters);
    }

    for (auto& t : threads) {
        pthread_join(t, nullptr);
    }

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
            run_test_pthreads(times, num_threads);
        }
    }
}
