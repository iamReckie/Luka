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
 * @brief Lock-free 기반의 간단한 스레드풀 구현
 *
 * C++17 환경에서 효율적인 병렬 처리를 제공합니다.
 * 작업 큐는 mutex로 보호되며, 워커 스레드들이 작업을 가져와 실행합니다.
 */
class ThreadPool {
 public:
  /**
   * @brief ThreadPool 생성자
   * @param num_threads 생성할 워커 스레드 개수. 0 또는 지정하지 않으면 하드웨어 동시성 수 사용.
   */
  explicit ThreadPool(size_t num_threads = 0) {
    if (num_threads == 0) {
      num_threads = std::thread::hardware_concurrency();
      if (num_threads == 0) {
        num_threads = 4;  // 기본값
      }
    }

    for (size_t i = 0; i < num_threads; ++i) {
      workers_.emplace_back([this]() { WorkerLoop(); });
    }
  }

  /**
   * @brief ThreadPool 소멸자
   * 모든 스레드가 종료될 때까지 대기합니다.
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
   * @brief 작업을 스레드풀에 추가
   * @param task 실행할 함수 객체 (std::function<void()>)
   */
  void EnqueueTask(std::function<void()> task) {
    {
      std::lock_guard<std::mutex> lock(queue_mutex_);
      if (should_stop_.load(std::memory_order_acquire)) {
        return;  // 풀이 종료 중이면 작업 거부
      }
      task_queue_.push(std::move(task));
    }
    condition_.notify_one();
  }

  /**
   * @brief 현재 스레드풀의 워커 스레드 개수
   * @return 워커 스레드 개수
   */
  size_t GetNumWorkers() const { return workers_.size(); }

 private:
  /**
   * @brief 워커 스레드의 메인 루프
   * 큐에서 작업을 가져와 실행합니다.
   */
  void WorkerLoop() {
    while (true) {
      std::unique_lock<std::mutex> lock(queue_mutex_);

      // 큐가 비어있고 should_stop이 false일 때까지 대기
      condition_.wait(lock, [this]() {
        return !task_queue_.empty() ||
               should_stop_.load(std::memory_order_acquire);
      });

      if (should_stop_.load(std::memory_order_acquire) &&
          task_queue_.empty()) {
        break;  // 종료 신호 + 큐가 비어있으면 루프 탈출
      }

      if (task_queue_.empty()) {
        continue;  // 큐가 비어있으면 다시 대기
      }

      std::function<void()> task = std::move(task_queue_.front());
      task_queue_.pop();
      lock.unlock();

      // 작업 실행 (뮤텍스 락 없이)
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
