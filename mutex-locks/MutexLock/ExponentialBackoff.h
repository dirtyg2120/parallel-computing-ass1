#pragma once
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>

class ExponentialBackoff
{
    private:
        int minDelay;  // Minimum delay (initial backoff time)
        int maxDelay;  // Maximum delay (maximum backoff time)
        int delay;     // Current backoff limit

    public:
        ExponentialBackoff(int min, int max) : minDelay(min), maxDelay(max), delay(min) {}

        void backoff() {
            if (maxDelay == 0) return;

            std::this_thread::sleep_for(std::chrono::microseconds(delay));

            // Exponentially increase the limit for the next backoff
            delay = std::min(2 * delay, maxDelay);
        }

        void reset() {
            delay = minDelay; // Reset the backoff limit
        }
};

