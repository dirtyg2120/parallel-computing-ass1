#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <iomanip>

std::mutex print_mutex;

class SenseReversingBarrier {
private:
    std::atomic<int> count;
    std::atomic<bool> sense;
    int initial_count;
    thread_local static bool local_sense;

public:
    SenseReversingBarrier(int n_threads)
        : count(n_threads), initial_count(n_threads), sense(true) {}

    void wait() {
        local_sense = !local_sense;
        int arrived = count.fetch_sub(1, std::memory_order_acq_rel);

        if (arrived == 1) {
            count.store(initial_count, std::memory_order_relaxed);
            sense.store(local_sense, std::memory_order_release);
        } else {
            while (sense.load(std::memory_order_acquire) != local_sense) {
                std::this_thread::yield();
            }
        }
    }
};

thread_local bool SenseReversingBarrier::local_sense = false;

void worker(SenseReversingBarrier& barrier, int id) {
    barrier.wait();
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

            SenseReversingBarrier barrier(n_threads);
            std::vector<std::thread> threads;

            auto start_time = std::chrono::high_resolution_clock::now();

            for (int iter = 0; iter < n_iterations; ++iter) {
                threads.clear();
                for (int i = 0; i < n_threads; ++i) {
                    threads.emplace_back(worker, std::ref(barrier), i);
                }

                for (auto& t : threads) {
                    t.join();
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
