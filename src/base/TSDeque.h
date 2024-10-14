#pragma once

#include <deque>
#include <shared_mutex>
#include <condition_variable>
#include <atomic>

namespace base {
    template<typename T>
    class TSDeque {
    public:
        TSDeque() = default;
        ~TSDeque() = default;

        T &front() {
            std::shared_lock lock(sharedMutex_);
            return deque_.front();
        }

        T &back() {
            std::shared_lock lock(sharedMutex_);
            return deque_.back();
        }

        void pushFront(const T &data) {
            std::unique_lock lock(sharedMutex_);
            deque_.push_front(data);

            std::unique_lock blockLock(blockMutex_);
            condVar_.notify_one();
        }

        void pushBack(const T &data) {
            std::unique_lock lock(sharedMutex_);
            deque_.push_back(data);

            std::unique_lock blockLock(blockMutex_);
            condVar_.notify_one();
        }

        void pushFront(T &&data) {
            std::unique_lock lock(sharedMutex_);
            deque_.emplace_front(data);

            std::unique_lock blockLock(blockMutex_);
            condVar_.notify_one();
        }

        void pushBack(T &&data) {
            std::unique_lock lock(sharedMutex_);
            deque_.emplace_back(data);

            std::unique_lock blockLock(blockMutex_);
            condVar_.notify_one();
        }

        T popFront() {
            std::unique_lock lock(sharedMutex_);
            T data = deque_.front();
            deque_.pop_front();
            return data;
        }

        T popBack() {
            std::unique_lock lock(sharedMutex_);
            T data = deque_.back();
            deque_.pop_back();
            return data;
        }

        bool empty() const {
            std::shared_lock lock(sharedMutex_);
            return deque_.empty();
        }

        size_t size() const {
            std::shared_lock lock(sharedMutex_);
            return deque_.size();
        }

        void clear() {
            std::unique_lock lock(sharedMutex_);
            deque_.clear();
        }

        void quit() {
            quit_ = true;
            condVar_.notify_all();
        }

        bool isRunning() const {
            return !quit_;
        }

        void wait() {
            std::unique_lock lock(blockMutex_);
            condVar_.wait(lock, [this] { return !deque_.empty() || quit_; });
        }

    private:
        std::deque<T> deque_;
        mutable std::shared_mutex sharedMutex_;
        std::mutex blockMutex_;
        std::condition_variable condVar_;
        std::atomic<bool> quit_{false};
    };
}
