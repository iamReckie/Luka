// ============================================================================
// Copyright © 2025 Luka. All rights reserved.
// SPDX-License-Identifier: Proprietary
//
// This software is proprietary and confidential.
// Redistribution, modification, or any form of reuse without explicit
// written permission from Luka is strictly prohibited.
//
// This file is a component of the Luka Risk Intelligence Suite™.
// Unauthorized use may result in legal action.
//
// Developed by: Luka
// ============================================================================
#ifndef SRC_UTILITY_THREAD_POOL_H_
#define SRC_UTILITY_THREAD_POOL_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

/**
 * @class ThreadPool
 * @brief Lock-free based thread pool
 */
class ThreadPool {
 public:
  /**
   * @brief ThreadPool
   * @param num_threads
   */
  explicit ThreadPool(size_t num_threads = 0) {
    if (num_threads == 0) {
      num_threads = std::thread::hardware_concurrency();
      if (num_threads == 0) {
        num_threads = 4;
      }
    }

    for (size_t i = 0; i < num_threads; ++i) {
      workers_.emplace_back([this]() { WorkerLoop(); });
    }
  }

  /**
   * @brief ThreadPool Destructor
   */
  ~ThreadPool() {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      should_stop_.store(true, std::memory_order_release);
    }
    condition_.notify_all();

    for (auto& worker : workers_) {
      if (worker.joinable()) {
        worker.join();
      }
    }
  }

  /**
   * @brief Add thread pool task
   * @param task std::function<void()>)
   */
  void EnqueueTask(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      if (should_stop_.load(std::memory_order_acquire)) {
        return;
      }
      task_queue_.push(std::move(task));
    }
    condition_.notify_one();
  }

  /**
   * @brief Get worker thread count
   * @return Worker thread count
   */
  size_t GetNumWorkers() const { return workers_.size(); }

 private:
  /**
   * @brief Worker main loop
   */
  void WorkerLoop() {
    while (true) {
      std::unique_lock<std::mutex> lock(queue_mutex_);

      // Wait for tasks or stop signal
      condition_.wait(lock, [this]() {
        return !task_queue_.empty() ||
               should_stop_.load(std::memory_order_acquire);
      });

      if (should_stop_.load(std::memory_order_acquire) &&
          task_queue_.empty()) {
        break;
      }

      if (task_queue_.empty()) {
        continue;
      }

      std::function<void()> task = std::move(task_queue_.front());
      task_queue_.pop();
      lock.unlock();

      task();
    }
  }

  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> task_queue_;
  std::mutex queue_mutex_;
  std::condition_variable condition_;
  std::atomic<bool> should_stop_{false};
};

#endif  // SRC_UTILITY_THREAD_POOL_H_
