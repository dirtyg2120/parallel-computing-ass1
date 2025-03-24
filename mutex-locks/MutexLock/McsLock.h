#pragma once
#include <atomic>
#include <thread>
#include <iostream>
#include "ExponentialBackoff.h"

class McsLock
{
    public:
        struct Node {
            std::atomic<Node*> next;
            std::atomic<bool> locked;
        };

        McsLock(int minDelay = 0, int maxDelay = 0)
            : backoff(minDelay, maxDelay) {
        }

        void acquire(Node* myNode) {
            myNode->next.store(nullptr, std::memory_order_relaxed);
            myNode->locked.store(true, std::memory_order_relaxed);

            Node* prevTail = tail.exchange(myNode, std::memory_order_acq_rel);
            if (prevTail != nullptr) {
                prevTail->next.store(myNode, std::memory_order_release);

                while (myNode->locked.load(std::memory_order_acquire)) {
                    backoff.backoff();
                }
            }
            backoff.reset();
        }

        void release(Node* myNode) {
            if (myNode->next.load(std::memory_order_acquire) == nullptr) {
                Node* expected = myNode;
                if (tail.compare_exchange_strong(expected, nullptr, std::memory_order_release, std::memory_order_relaxed)) {
                    backoff.reset();
                    return;
                }

                while (myNode->next.load(std::memory_order_acquire) == nullptr) {
                    backoff.backoff();
                }
            }

            myNode->next.load(std::memory_order_acquire)->locked.store(false, std::memory_order_release);
            backoff.reset();
        }

    private:
        std::atomic<Node*> tail{ nullptr };
        ExponentialBackoff backoff;
};

