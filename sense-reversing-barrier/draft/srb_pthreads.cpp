// pthread_barrier.cpp
#include <iostream>
#include <vector>
#include <mutex>
#include <pthread.h>

std::mutex print_mutex;

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
    pthread_barrier_init(&pthread_barrier, nullptr, n_threads);
}

int main() {
    constexpr int n_threads = 4;  // Set thread count
    pthread_barrier_init(n_threads);

    std::vector<pthread_t> threads(n_threads);
    std::vector<int> thread_ids(n_threads);

    for (int i = 0; i < n_threads; ++i) {
        thread_ids[i] = i;
        pthread_create(&threads[i], nullptr, pthread_worker, &thread_ids[i]);
    }

    for (auto& t : threads) {
        pthread_join(t, nullptr);
    }

    return 0;
}
