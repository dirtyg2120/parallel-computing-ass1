#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cassert>

class SenseReversingBarrier {
private:
    int num_threads;             // Number of threads in the barrier
    std::atomic<int> count;      // Count of threads that have arrived at the barrier
    std::atomic<bool> sense;     // The shared sense state (either true or false)

public:
    // Constructor
    SenseReversingBarrier(int num_threads) : num_threads(num_threads), count(0), sense(true) {}

    // Barrier function
    void wait() {
        bool my_sense = sense.load(); // Capture the current sense value
        count.fetch_add(1, std::memory_order_release);  // Increment the count
        std::cout << "wow" << std::endl;
        
        // Memory fence to ensure visibility
        std::atomic_thread_fence(std::memory_order_acquire);

        // Wait for all threads to arrive
        while (count.load(std::memory_order_acquire) < num_threads) {
            std::cout << "wait" << std::endl;
        }

        if (count.load(std::memory_order_acquire) == num_threads) {
            count.store(0, std::memory_order_release);  // Reset the count for the next round
            sense.store(!my_sense, std::memory_order_release);  // Reverse the sense
        }

        // Busy-wait for the current thread to continue after the barrier
        while (sense.load() != my_sense) {}
    }
};

void thread_function(SenseReversingBarrier& barrier, int id) {
    // Some work before the barrier (not necessary for correctness)
    barrier.wait();  // Wait at the barrier
    // Some work after the barrier (not necessary for correctness)
}

int main() {
    const int num_threads = 2;
    SenseReversingBarrier barrier(num_threads);

    std::vector<std::thread> threads;
    
    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(thread_function, std::ref(barrier), i));
    }

    for (auto& t : threads) {
        t.join();
    }

    std::cout << "All threads passed the barrier successfully!" << std::endl;

    return 0;
}
