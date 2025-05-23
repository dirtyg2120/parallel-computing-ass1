#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <iomanip>

class SenseReversingBarrier {
private:
    int count;
    int initial_count;
    bool sense;
    pthread_mutex_t mutex;
    pthread_cond_t cond;

public:
    SenseReversingBarrier(int n_threads) 
        : count(n_threads), initial_count(n_threads), sense(true) {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond, nullptr);
    }

    ~SenseReversingBarrier() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }

    void wait() {
        bool local_sense = !sense;

        pthread_mutex_lock(&mutex);

        count--;

        if (count == 0) {
            count = initial_count;
            sense = local_sense;
            pthread_cond_broadcast(&cond);
        } else {
            while (sense != local_sense) {
                pthread_cond_wait(&cond, &mutex);
            }
        }

        pthread_mutex_unlock(&mutex);
    }
};

void* worker(void* arg) {
    SenseReversingBarrier* barrier = (SenseReversingBarrier*)arg;
    barrier->wait();
    return nullptr;
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
            std::vector<pthread_t> threads(n_threads);

            auto start_time = std::chrono::high_resolution_clock::now();

            for (int iter = 0; iter < n_iterations; ++iter) {
                for (int i = 0; i < n_threads; ++i) {
                    pthread_create(&threads[i], nullptr, worker, (void*)&barrier);
                }

                for (auto& t : threads) {
                    pthread_join(t, nullptr);
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
