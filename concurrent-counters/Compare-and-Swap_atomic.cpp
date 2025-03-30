#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <chrono>

std::atomic<int> counter(0); // Bộ đếm được bảo vệ bằng atomic

void increment_cas(int times) {
    for (int i = 0; i < times; ++i) {
        int expected = counter.load();
        while (!counter.compare_exchange_weak(expected, expected + 1)) {
            // Nếu CAS thất bại (do giá trị đã thay đổi), thử lại
            expected = counter.load();
        }
    }
}

bool test_correctness(int num_threads, int times) {
    int base = times / num_threads;// 100/4 = 25
    int rem = times % num_threads;//0
    counter.store(0); // Đặt lại bộ đếm về 0

    std::vector<std::thread> threads;
    for (int i = 0; i < num_threads; ++i) {
        int iters = base + (i < rem ? 1 : 0);
        threads.push_back(std::thread(increment_cas, iters));
    }

    for (auto& t : threads) {
        t.join();
    }

    return counter.load() == num_threads * times; // Kiểm tra tính đúng đắn
}

void run_test(int times, int num_threads) {
    auto start = std::chrono::high_resolution_clock::now();
    int base = times / num_threads;// 100/4 = 25
    int rem = times % num_threads;//0
    bool result = test_correctness(num_threads, times);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::string pass_fail = result ? "Pass" : "Fail";

    std::cout << "Times: " << times << "\tThreads: " << num_threads 
              << "\tExecution Time (ms): " << duration.count() * 1000 
              << "\tCounter: " << counter << std::endl;
}

int main() {
    for (int times : {1000, 100000, 10000000}) {
        for (int num_threads : {1, 2, 4, 8}) {
            run_test(times, num_threads);
        }
    }

    return 0;
}
