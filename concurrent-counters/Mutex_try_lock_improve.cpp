#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>

// =============== Phần triển khai backoff cho mutex ===============
void lock_with_backoff(std::mutex& mtx) {
    int backoff = 1;                 
    const int MAX_BACKOFF = 1 << 15; 
    
    while (!mtx.try_lock()) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(backoff));
        if (backoff < MAX_BACKOFF) {
            backoff <<= 1;  // nhân đôi
        }
    }
}

int counter_mutex_backoff = 0;
std::mutex mtx_backoff;

void increment_mutex_backoff(int times) {
    for (int i = 0; i < times; ++i) {
        lock_with_backoff(mtx_backoff);
        ++counter_mutex_backoff;  // vùng tới hạn
        mtx_backoff.unlock();
    }
}

// =============== Phần kiểm thử ===============
bool test_correctness(int num_threads, int times) {
    // Đặt lại bộ đếm về 0
    counter_mutex_backoff = 0;

    // Tạo các luồng
    std::vector<std::thread> threads;
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(increment_mutex_backoff, times);
    }

    // Chờ tất cả luồng kết thúc
    for (auto &t : threads) {
        t.join();
    }

    // Kiểm tra kết quả cuối cùng
    return (counter_mutex_backoff == num_threads * times);
}

void run_test(int times, int num_threads) {
    // Đo thời gian bắt đầu
    auto start = std::chrono::high_resolution_clock::now();

    bool result = test_correctness(num_threads, times);

    // Đo thời gian kết thúc
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    // Pass/Fail
    std::string pass_fail = result ? "Pass" : "Fail";

    // In ra kết quả
    std::cout << times << "\t" << "\t"
              << num_threads << "\t" << "\t"
              << counter_mutex_backoff << "\t" << "\t"
              << duration.count()*1000 << "\t" << "\t"
              << pass_fail << std::endl;
}

int main() {
    // Tiêu đề bảng
    std::cout << "Times \t \t Threads \t \t Counter \t \t Exec_Time (ms)\t \t Pass/Fail" << std::endl;

    // Kiểm thử với times từ 1 đến 3, và threads = 1, 2, 4, 8
    for (int times : {1, 100000, 10000000}) {
        for (int num_threads : {1, 2, 4, 8}) {
            run_test(times, num_threads);
        }
    }

    return 0;
}