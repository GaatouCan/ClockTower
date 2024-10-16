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

        T &Front() {
            std::shared_lock lock(sharedMutex_);
            return deque_.front();
        }

        T &Back() {
            std::shared_lock lock(sharedMutex_);
            return deque_.back();
        }

        void PushFront(const T &data) {
            std::unique_lock lock(sharedMutex_);
            deque_.push_front(data);

            std::unique_lock blockLock(blockMutex_);
            condVar_.notify_one();
        }

        void PushBack(const T &data) {
            std::unique_lock lock(sharedMutex_);
            deque_.push_back(data);

            std::unique_lock blockLock(blockMutex_);
            condVar_.notify_one();
        }

        void PushFront(T &&data) {
            std::unique_lock lock(sharedMutex_);
            deque_.emplace_front(data);

            std::unique_lock blockLock(blockMutex_);
            condVar_.notify_one();
        }

        void PushBack(T &&data) {
            std::unique_lock lock(sharedMutex_);
            deque_.emplace_back(data);

            std::unique_lock blockLock(blockMutex_);
            condVar_.notify_one();
        }

        T PopFront() {
            std::unique_lock lock(sharedMutex_);
            T data = deque_.front();
            deque_.pop_front();
            return data;
        }

        T PopBack() {
            std::unique_lock lock(sharedMutex_);
            T data = deque_.back();
            deque_.pop_back();
            return data;
        }

        bool IsEmpty() const {
            std::shared_lock lock(sharedMutex_);
            return deque_.empty();
        }

        size_t Size() const {
            std::shared_lock lock(sharedMutex_);
            return deque_.size();
        }

        void Clear() {
            std::unique_lock lock(sharedMutex_);
            deque_.clear();
        }

        void Quit() {
            quit_ = true;
            condVar_.notify_all();
        }

        bool IsRunning() const {
            return !quit_;
        }

        void Wait() {
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
