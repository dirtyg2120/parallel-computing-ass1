#pragma once
#include <atomic>
#include <thread>
#include <iostream>
#include "ExponentialBackoff.h"

class TasLock
{
    public:
        TasLock(int minDelay = 0, int maxDelay = 0)
            : backoff(minDelay, maxDelay) {}

        void acquire() {
            while (true) {
                if (!flag.exchange(true, std::memory_order_acquire)) {
                    return;
                }
                backoff.backoff();
            }
        }

        void release() {
            flag.store(false, std::memory_order_release);
            backoff.reset();
        }

    private:
        std::atomic<bool> flag{ false };
        ExponentialBackoff backoff;
};

