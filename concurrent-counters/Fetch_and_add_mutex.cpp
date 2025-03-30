#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>

int counter = 0;
std::mutex mtx;

void increment_mutex(int times) {
    for (int i = 0; i < times; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter;
    }
}

void run_test_mutex(int times, int num_threads) {
    auto start = std::chrono::high_resolution_clock::now();
    int base = times / num_threads;// 100/4 = 25
    int rem = times % num_threads;//0

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        int iters = base + (i < rem ? 1 : 0);
        threads.push_back(std::thread(increment_mutex, iters));
    }

    for (auto& t : threads) {
        t.join();
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
            run_test_mutex(times, num_threads);
        }
    }
}
