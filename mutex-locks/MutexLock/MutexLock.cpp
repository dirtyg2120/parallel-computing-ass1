// MutexLock.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>
#include <string>
#include <unordered_map>
#include <omp.h>
#include <mutex>
#include "TasLock.h"
#include "TtasLock.h"
#include "McsLock.h"
#include "JsonExporter.h"

using namespace std::chrono;

/* Configurations */
#define NUM_OF_THREADS 4
#define NUM_OF_ITERS 1500
#define OUTSIDE_WORK 0
#define N 1000003

enum LockType {
    TAS,
    TTAS,
    MCS,
    OMP,
    MUTEX
};

std::unordered_map<LockType, std::string> lockTypeNames = {
    {LockType::TAS, "TAS"},
    {LockType::TTAS, "TTAS"},
    {LockType::MCS, "MCS"},
    {LockType::OMP, "OMP"},
    {LockType::MUTEX, "MUTEX"}
};

std::string getLockTypeName(LockType type) {
    auto it = lockTypeNames.find(type);
    if (it != lockTypeNames.end()) {
        return it->second;
    }
    return "Unknown";
}

enum TestType {
    Correctness,
    Performance
};

std::atomic<int> totalWorkDone{ 0 };
std::atomic<int> criticalWorkDone{ 0 };
std::atomic<int> outsideWorkDone{ 0 };

JsonExporter toJson(int numThreads, LockType lockType, int numOfIters, int targetWork, int outsideWork, int n, int totalWorkDone, int criticalWorkDone, int outsideWorkDone, int maxBackoffDelay, double duration) {
    JsonExporter data;
    data.add("numberOfThreads", numThreads);
    data.add("lock", getLockTypeName(lockType));
    data.add("iters", numOfIters);
    data.add("targetWork", targetWork);
    data.add("outsideWork", outsideWork);
    data.add("N", n);
    data.add("maxBackoffDelay", maxBackoffDelay);
    data.add("totalWorkDone", totalWorkDone);
    data.add("criticalWorkDone", criticalWorkDone);
    data.add("outsideWorkDone", outsideWorkDone);
    data.add("time", duration);

    return data;
}

int is_prime(int p) {
    int d;
    for (d = 2; d < p; d = d + 1) {
        if (p % d == 0) {
            return 0;
        };
    };
    return 1;
}

void correctness_thread_entry(LockType lockType, int numOfIters, int outsideWork, int maxBackoffDelay) {
    McsLock::Node mcsNode;

    TasLock taslock(10, maxBackoffDelay);
    TtasLock ttaslock(10, maxBackoffDelay);
    McsLock mcslock(10, maxBackoffDelay);
    std::mutex mtx;

    if (lockType != MUTEX) {
        for (auto i = 0; i < numOfIters; i++) {
            switch (lockType)
            {
            case TAS:
                taslock.acquire();
                break;
            case TTAS:
                ttaslock.acquire();
                break;
            case MCS:
                mcslock.acquire(&mcsNode);
                break;
            default:
                break;
            };

            // Critical section
            totalWorkDone++;
            criticalWorkDone++;
            is_prime(N);

            switch (lockType)
            {
            case TAS:
                taslock.release();
                break;
            case TTAS:
                ttaslock.release();
                break;
            case MCS:
                mcslock.release(&mcsNode);
                break;
            default:
                break;
            };

            // Outside work
            for (auto j = 0; j < outsideWork; ++j) {
                totalWorkDone++;
                outsideWorkDone++;
                is_prime(N);
            }
        }
    }
    else {
        for (auto i = 0; i < numOfIters; i++) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                totalWorkDone++;
                criticalWorkDone++;
                is_prime(N);
            }

            for (auto j = 0; j < outsideWork; ++j) {
                totalWorkDone++;
                outsideWorkDone++;
                is_prime(N);
            }
        }
    }
}

void performance_thread_entry(LockType lockType, int targetWork, int outsideWork, int maxBackoffDelay) {
    McsLock::Node mcsNode;

    TasLock taslock;
    TtasLock ttaslock;
    McsLock mcslock;
    std::mutex mtx;

    if (lockType != MUTEX) {
        while (totalWorkDone < targetWork) {
            switch (lockType)
            {
            case TAS:
                taslock.acquire();
                break;
            case TTAS:
                ttaslock.acquire();
                break;
            case MCS:
                mcslock.acquire(&mcsNode);
                break;
            default:
                break;
            };

            // Critical section
            totalWorkDone++;
            criticalWorkDone++;
            is_prime(N);

            switch (lockType)
            {
            case TAS:
                taslock.release();
                break;
            case TTAS:
                ttaslock.release();
                break;
            case MCS:
                mcslock.release(&mcsNode);
                break;
            default:
                break;
            };

            // Outside work
            for (auto j = 0; j < outsideWork; ++j) {
                if (totalWorkDone >= targetWork)
                    return;
                totalWorkDone++;
                outsideWorkDone++;
                is_prime(N);
            }
        }
    }
    else {
        while (totalWorkDone < targetWork) {
            {
                std::lock_guard<std::mutex> lock(mtx);
                totalWorkDone++;
                criticalWorkDone++;
                is_prime(N);
            }

            for (auto j = 0; j < outsideWork; ++j) {
                if (totalWorkDone >= targetWork)
                    return;
                totalWorkDone++;
                outsideWorkDone++;
                is_prime(N);
            }
        }
        
    }
}

void run_test(TestType testType, LockType lockType, int numThreads, int numOfIters, int outsideWork, int n, int maxBackoffDelay, JsonExporter& data) {
    std::vector<std::thread> threads;
    totalWorkDone = 0;
    outsideWorkDone = 0;
    criticalWorkDone = 0;

    omp_lock_t lock;
    omp_init_lock(&lock);
    omp_set_num_threads(numThreads);

    if (lockType == MUTEX || lockType == OMP) {
        maxBackoffDelay = 0;
    }

    auto start = high_resolution_clock::now();

    if (lockType != OMP) {
        if (testType == Correctness) {
            for (int i = 0; i < numThreads; ++i) {
                threads.emplace_back(correctness_thread_entry, lockType, numOfIters, outsideWork, maxBackoffDelay);
            }
        }
        else {
            for (int i = 0; i < numThreads; ++i) {
                threads.emplace_back(performance_thread_entry, lockType, numOfIters, outsideWork, maxBackoffDelay);
            }
        }

        for (auto& t : threads) {
            t.join();
        }
    }
    else {
        if (testType == Performance) {
            #pragma omp parallel
            {
                while (totalWorkDone < numOfIters) {
                    omp_set_lock(&lock);
                    totalWorkDone++;
                    criticalWorkDone++;
                    is_prime(N);
                    omp_unset_lock(&lock);

                    for (auto j = 0; j < outsideWork; ++j) {
                        if (totalWorkDone < numOfIters) {
                            totalWorkDone++;
                            outsideWorkDone++;
                            is_prime(N);
                        };
                    }
                }
            }
        }
        else {
            #pragma omp parallel
            {
                for (auto i = 0; i < numOfIters; i++) {
                    omp_set_lock(&lock);
                    totalWorkDone++;
                    criticalWorkDone++;
                    is_prime(N);
                    omp_unset_lock(&lock);

                    for (auto j = 0; j < outsideWork; ++j) {
                        totalWorkDone++;
                        outsideWorkDone++;
                        is_prime(N);
                    }
                }
            }
        }
    }

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    omp_destroy_lock(&lock);

    if (testType == Performance) {
        data = toJson(numThreads, lockType, 0, numOfIters, outsideWork, n, totalWorkDone, criticalWorkDone, outsideWorkDone, maxBackoffDelay, duration);
        std::cout << "Threads: " << numThreads
            << ", Lock: " << getLockTypeName(lockType)
            << ", Target Work: " << numOfIters
            << ", Outside Work: " << outsideWork
            << ", N: " << n
            << ", Max Backoff Delay: " << maxBackoffDelay
            << ", Total Work Done: " << totalWorkDone
            << ", Critical Work Done: " << criticalWorkDone
            << ", Outside Work Done: " << outsideWorkDone
            << ", Time: " << duration << " ms";
    }
    
    if (testType == Correctness) {
        data = toJson(numThreads, lockType, numOfIters, 0, outsideWork, n, totalWorkDone, criticalWorkDone, outsideWorkDone, maxBackoffDelay, duration);
        std::cout << "Threads: " << numThreads
            << ", Lock: " << getLockTypeName(lockType)
            << ", Iters: " << numOfIters
            << ", Outside Work: " << outsideWork
            << ", N: " << n
            << ", Max Backoff Delay: " << maxBackoffDelay
            << ", Total Work Done: " << totalWorkDone
            << ", Critical Work Done: " << criticalWorkDone
            << ", Outside Work Done: " << outsideWorkDone
            << ", Time: " << duration << " ms";

        auto raceCheck = "No race conditions detected";

        if (totalWorkDone != numThreads * numOfIters * (outsideWork + 1)) {
            raceCheck = "Race conditions detected";
        }

        std::cout << ", " << raceCheck << std::endl;
    }
    else {
        std::cout << std::endl;
    }
}

int main() {
    std::vector<LockType> lock_types = { TAS, TTAS, MCS, OMP, MUTEX };
    std::vector<int> thread_counts = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> num_of_iters = { 500, 1000, 2000 };
    std::vector<int> outside_work_values = { 0, 1, 2, 3 };
    std::vector<int> max_back_off_delays = { 0, 300, 600, 900 };
    
    std::vector<JsonExporter> cResults = {};
    std::vector<JsonExporter> pResults = {};

    for (LockType type : lock_types) {
        for (int threads : thread_counts) {
            if (type == MUTEX || type == OMP || threads == 1) {
                max_back_off_delays = { 0 };
            }
            else {
                max_back_off_delays = { 0, 300, 600, 900 };
            };
            if (threads == 1) {
                outside_work_values = { 0 };
            }
            else {
                outside_work_values = { 0, 1, 2, 3 };
            }
            for (int iters : num_of_iters) {
                for (int outsidework : outside_work_values) {
                    for (int delay : max_back_off_delays) {
                        JsonExporter performanceResult;
                        run_test(Performance, type, threads, iters, outsidework, N, delay, performanceResult);
                        pResults.push_back(performanceResult);

                        JsonExporter correctnessResult;
                        run_test(Correctness, type, threads, iters, outsidework, N, delay, correctnessResult);
                        cResults.push_back(correctnessResult);
                    }
                }
            }
        }
    }

    JsonExporter performanceResults;
    performanceResults.add("data", pResults);
    performanceResults.exportToFile("performance-results.json");

    JsonExporter correctnessResults;
    correctnessResults.add("data", cResults);
    correctnessResults.exportToFile("correctness-results.json");
    return 0;
}
