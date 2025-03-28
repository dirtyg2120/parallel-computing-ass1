#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

std::atomic<int> counter(0);

void increment_cas(int times) {
    for (int i = 0; i < times; ++i) {
        int expected = counter.load();
        while (!counter.compare_exchange_weak(expected, expected + 1)) {
            // Nếu CAS thất bại, expected được cập nhật và vòng lặp thử lại
        }
    }
}

bool test_correctness(int num_threads, int times) {
    // Khởi tạo lại bộ đếm
    counter.store(0);
    
    // Tạo các luồng
    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        threads.push_back(std::thread(increment_cas, times));
    }

    // Đợi tất cả các luồng hoàn thành
    for (auto& t : threads) {
        t.join();
    }

    // Kiểm tra tính đúng đắn
    return counter.load() == num_threads * times;
}

void run_test(int times, int num_threads) {
    // Đo thời gian thực thi
    auto start = std::chrono::high_resolution_clock::now();

    bool result = test_correctness(num_threads, times);
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::string pass_fail = result ? "Pass" : "Fail";

    // In ra kết quả kiểm thử cùng giá trị bộ đếm
    std::cout << times << "\t" << "\t" << num_threads << "\t" << "\t"
              << counter.load() << "\t" << "\t"
              << duration.count()*1000 << "\t" << "\t" 
              << pass_fail << std::endl;
}

int main() {
    // In tiêu đề bảng
    std::cout << "Times\tThreads\tCounter\tExec_Time (ms)\tPass/Fail" << std::endl;

    // Kiểm thử với các số lần từ 1 đến 3 và số luồng từ 1, 2, 4, 8
    for (int times : {1, 100000, 10000000}) {
        for (int num_threads : {1, 2, 4, 8}) {
            run_test(times, num_threads);
        }
    }

    return 0;
}
