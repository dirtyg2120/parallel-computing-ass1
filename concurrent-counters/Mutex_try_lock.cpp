#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>

std::mutex mtx;
int counter = 0;

void increment_mutex(int times) {
    for (int i = 0; i < times; ++i) {
        std::lock_guard<std::mutex> lock(mtx);
        ++counter;  // Tăng giá trị của counter trong vùng bảo vệ bởi mutex
    }
}

void run_test_mutex(int times, int num_threads) {
    counter = 0;  // Đặt lại bộ đếm về 0 cho mỗi bài kiểm thử

    auto start = std::chrono::high_resolution_clock::now();
    int base = times / num_threads;// 100/8 = 12
    int rem = times % num_threads;//4
    
    // times = times/num_threads;
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        int iters = base + (i < rem ? 1 : 0);
        threads.push_back(std::thread(increment_mutex, iters));
    }

    for (auto& t : threads) {
        t.join();  // Đợi tất cả các luồng hoàn thành
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Times: " << times << "\tThreads: " << num_threads 
    << "\tExecution Time (ms): " << duration.count() * 1000          
    << "\tCounter: " << counter 
               << std::endl;
}

int main() {
    std::cout << "Running tests with std::mutex 1" << std::endl;
    for (int times : {1000, 100000, 10000000}) {//{100, 100000, 10000000}
        for (int threads : {1, 2, 4, 8}) {
            run_test_mutex(times, threads);
        }
    }
    return 0;
}
