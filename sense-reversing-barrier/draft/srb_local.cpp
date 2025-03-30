#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>

std::mutex print_mutex;  // Mutex for printing in multithread

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

// Init local sense
thread_local bool SenseReversingBarrier::local_sense = false;

void worker(SenseReversingBarrier& barrier, int id, const std::vector<int>& arr, int start, int end, std::atomic<int>& local_sum) {
    int sum = 0;

    for (int i = start; i < end; ++i) {
        sum += arr[i];
        {
            std::lock_guard<std::mutex> lock(print_mutex);
            std::cout << arr[i] << " ";
        }
    }

    // Save the calculated result into atomic variable
    local_sum = sum;
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "Thread " << id << " with sum: " << local_sum << std::endl;
    }
    // Wair for other threads
    barrier.wait();
}

int main() {
    constexpr int n_threads = 10;
    std::vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int n = arr.size();
    int chunk_size = n / n_threads;
    std::vector<std::thread> threads;
    std::vector<std::atomic<int>> local_sums(n_threads);
    SenseReversingBarrier barrier(n_threads);

    // Iterate through each threads to sum each part of the array
    for (int i = 0; i < n_threads; ++i) {
        int start = i * chunk_size;
        int end = (i == n_threads - 1) ? n : (i + 1) * chunk_size;
        threads.emplace_back(worker, std::ref(barrier), i, std::ref(arr), start, end, std::ref(local_sums[i]));
    }

    // Wait for all threads done
    for (auto& t : threads) {
        t.join();
    }

    int total_sum = 0;
    for (int i = 0; i < n_threads; ++i) {
        total_sum += local_sums[i].load();
    }

    std::cout << "Total sum: " << total_sum << std::endl;
    return 0;
}
