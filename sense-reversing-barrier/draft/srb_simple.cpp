#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

std::mutex print_mutex;  // Mutex to synchronize output to std::cout

class SenseReversingBarrier {
private:
    std::atomic<int> count;      // Number of threads not yet at the barrier
    std::atomic<bool> sense;     // Global sense variable
    int initial_count;           // Total threads to wait for
    thread_local static bool local_sense;  // Per-thread sense (C++11 thread_local)

public:
    SenseReversingBarrier(int n_threads) 
        : count(n_threads), initial_count(n_threads), sense(true) {}

    void wait() {
        local_sense = !local_sense;  // Flip local sense
        int arrived = count.fetch_sub(1, std::memory_order_acq_rel);

        if (arrived == 1) {
            // Last thread resets the barrier
            count.store(initial_count, std::memory_order_relaxed);
            sense.store(local_sense, std::memory_order_release);
        } else {
            // Wait until global sense matches local sense
            while (sense.load(std::memory_order_acquire) != local_sense) {
                // Spin and yield to reduce contention
                std::this_thread::yield();
            }
        }
    }
};

// Initialize thread-local sense
thread_local bool SenseReversingBarrier::local_sense = false;

void worker(SenseReversingBarrier& barrier, int id) {
    {
        std::lock_guard<std::mutex> lock(print_mutex);  // Lock to ensure thread-safe printing
        std::cout << "Thread " << id << " is about to reach the barrier." << std::endl;
    }

    barrier.wait();  // Wait for all threads here

    {
        std::lock_guard<std::mutex> lock(print_mutex);  // Lock to ensure thread-safe printing
        std::cout << "Thread " << id << " passed the barrier." << std::endl;
    }
}

int main() {
    constexpr int n_threads = 40;
    SenseReversingBarrier barrier(n_threads);
    std::vector<std::thread> threads;

    for (int i = 0; i < n_threads; ++i) {
        threads.emplace_back(worker, std::ref(barrier), i);
    }

    for (auto& t : threads) {
        t.join();
    }

    return 0;
}
