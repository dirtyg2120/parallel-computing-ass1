#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <chrono>
#include <cmath>
#include <omp.h>
#include <pthread.h>

std::mutex print_mutex;

// ----- Sense Reversing Barrier with Exponential Backoff -----

// Original SenseReversingBarrier with added exponential backoff
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
            // Last thread resets the barrier
            count.store(initial_count, std::memory_order_relaxed);
            sense.store(local_sense, std::memory_order_release);
        } else {
            // Exponential backoff loop
            int backoff = 1;
            while (sense.load(std::memory_order_acquire) != local_sense) {
                std::this_thread::yield();  // Reduce contention
                std::this_thread::sleep_for(std::chrono::milliseconds(backoff));
                backoff = std::min(backoff * 2, 1000);  // Cap the backoff time
            }
        }
    }
};

// Initialize thread-local sense
thread_local bool SenseReversingBarrier::local_sense = false;

// ----- OpenMP Barrier Implementation -----

void openmp_worker(int id, int n_threads) {
    #pragma omp barrier
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "OpenMP: Thread " << id << " passed the barrier.\n";
    }
}

// ----- Pthreads Barrier Implementation -----
pthread_barrier_t pthread_barrier;

void* pthread_worker(void* arg) {
    int id = *(int*)arg;
    pthread_barrier_wait(&pthread_barrier);
    {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "Pthreads: Thread " << id << " passed the barrier.\n";
    }
    return nullptr;
}

void pthread_barrier_init(int n_threads) {
    pthread_barrier_init(&pthread_barrier, NULL, n_threads);
}

// ----- Performance Measurement -----

template <typename Barrier>
void measure_performance(Barrier& barrier, int n_threads, int n_iterations) {
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < n_threads; ++i) {
        threads.emplace_back([&barrier, i, n_iterations]() {  // Capture n_iterations
            for (int j = 0; j < n_iterations; ++j) {
                barrier.wait();
            }
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    double barrier_episodes_per_sec = n_threads * n_iterations / duration.count();
    std::cout << "Barrier episodes per second: " << barrier_episodes_per_sec << "\n";
}

// ----- Main -----
int main() {
    constexpr int n_threads = 4;  // Set thread count
    constexpr int n_iterations = 10;  // Set number of barrier episodes

    // Measure Sense Reversing Barrier with Exponential Backoff
    std::cout << "Testing Sense Reversing Barrier with Exponential Backoff:\n";
    SenseReversingBarrier barrier(n_threads);
    measure_performance(barrier, n_threads, n_iterations);

    // Measure OpenMP Barrier
    std::cout << "\nTesting OpenMP Barrier:\n";
    #pragma omp parallel num_threads(n_threads)
    openmp_worker(omp_get_thread_num(), n_threads);

    // Measure Pthreads Barrier
    std::cout << "\nTesting Pthreads Barrier:\n";
    std::vector<pthread_t> pthread_threads(n_threads);
    std::vector<int> thread_ids(n_threads);
    pthread_barrier_init(n_threads);
    
    for (int i = 0; i < n_threads; ++i) {
        thread_ids[i] = i;
        pthread_create(&pthread_threads[i], nullptr, pthread_worker, &thread_ids[i]);
    }

    for (auto& t : pthread_threads) {
        pthread_join(t, nullptr);
    }

    return 0;
}
