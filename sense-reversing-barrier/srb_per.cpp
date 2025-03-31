#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>  // For performance measurement

std::mutex print_mutex;  // Mutex to synchronize output to std::cout

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
    constexpr int n_threads = 2;
    constexpr int n_iterations = 10000;

    SenseReversingBarrier barrier(n_threads);
    std::vector<std::thread> threads;

    // Measure the start time
    auto start_time = std::chrono::high_resolution_clock::now();

    // Run the barrier synchronization for multiple iterations
    for (int iter = 0; iter < n_iterations; ++iter) {
        // Start the threads
        threads.clear();
        for (int i = 0; i < n_threads; ++i) {
            threads.emplace_back(worker, std::ref(barrier), i);
        }

        // Wait for all threads to finish
        for (auto& t : threads) {
            t.join();
        }
    }

    // Measure the end time
    auto end_time = std::chrono::high_resolution_clock::now();

    // Calculate the elapsed time
    std::chrono::duration<double> duration = end_time - start_time;
    double throughput = (n_threads * n_iterations) / duration.count();
    
    std::cout << "Time taken for " << n_threads << " threads to complete " << n_iterations << " iterations: " << duration.count() << " seconds" << std::endl;
    std::cout << "Barrier throughput (barriers per second): " << throughput << std::endl;

    return 0;
}
