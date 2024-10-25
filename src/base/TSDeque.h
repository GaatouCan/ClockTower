#pragma once

#include <deque>
#include <condition_variable>
#include <atomic>

namespace base {
    template<typename T>
    class TSDeque {
    public:
        TSDeque() = default;
        ~TSDeque() = default;

        T &Front() {
            std::scoped_lock lock(mutex_);
            return deque_.front();
        }

        T &Back() {
            std::scoped_lock lock(mutex_);
            return deque_.back();
        }

        void PushFront(const T &data) {
            std::scoped_lock lock(mutex_);
            deque_.push_front(data);

            std::unique_lock blockLock(blocking_);
            condVar_.notify_one();
        }

        void PushBack(const T &data) {
            std::scoped_lock lock(mutex_);
            deque_.push_back(data);

            std::unique_lock blockLock(blocking_);
            condVar_.notify_one();
        }

        void PushFront(T &&data) {
            std::scoped_lock lock(mutex_);
            deque_.emplace_front(data);

            std::unique_lock blockLock(blocking_);
            condVar_.notify_one();
        }

        void PushBack(T &&data) {
            std::scoped_lock lock(mutex_);
            deque_.emplace_back(data);

            std::unique_lock blockLock(blocking_);
            condVar_.notify_one();
        }

        T PopFront() {
            std::scoped_lock lock(mutex_);
            auto data = std::move(deque_.front());
            deque_.pop_front();
            return data;
        }

        T PopBack() {
            std::scoped_lock lock(mutex_);
            auto data = std::move(deque_.back());
            deque_.pop_back();
            return data;
        }

        bool IsEmpty() const {
            std::scoped_lock lock(mutex_);
            return deque_.empty();
        }

        size_t Size() const {
            std::scoped_lock lock(mutex_);
            return deque_.size();
        }

        void Clear() {
            std::scoped_lock lock(blocking_);
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
            std::unique_lock lock(blocking_);
            condVar_.wait(lock, [this] { return !deque_.empty() || quit_; });
        }

    private:
        std::deque<T> deque_;
        mutable std::mutex mutex_;
        std::mutex blocking_;
        std::condition_variable condVar_;
        std::atomic_bool quit_{false};
    };
}
