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
#ifndef SRC_LOGGER_LOGGER_H_
#define SRC_LOGGER_LOGGER_H_
#include <atomic>
#include <codecvt>
#include <cstdarg>
#include <fstream>
#include <iostream>
#include <locale>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

// Logger with single output file
class Logger {
 public:
  static void Log(const wchar_t* format, ...) {
    if (!GetInstance().is_initialized_.load(std::memory_order_acquire)) {
      return;
    }

    va_list args;
    va_start(args, format);
    wchar_t buffer[2048];
    vswprintf(buffer, sizeof(buffer) / sizeof(wchar_t), format, args);
    va_end(args);
    std::wstring wstr_buffer(buffer);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string str_buffer = converter.to_bytes(wstr_buffer);

    std::lock_guard<std::mutex> lock(GetInstance().mutex_);
    if (GetInstance().file_stream_.is_open()) {
      GetInstance().file_stream_ << str_buffer << std::flush;
    }
    if (GetThreadLocalStream().is_open()) {
      GetThreadLocalStream() << str_buffer << std::flush;
    }
    printf("%s", str_buffer.c_str());
  }

  static void Log(const char* format, ...) {
    if (!GetInstance().is_initialized_.load(std::memory_order_acquire)) {
      return;
    }

    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::lock_guard<std::mutex> lock(GetInstance().mutex_);
    if (GetInstance().file_stream_.is_open()) {
      GetInstance().file_stream_ << buffer << std::flush;
    }
    if (GetThreadLocalStream().is_open()) {
      GetThreadLocalStream() << buffer << std::flush;
    }
    printf("%s", buffer);
  }

  static void StartSecondaryLog(const std::string& file_name) {
    if (GetThreadLocalStream().is_open()) {
      GetThreadLocalStream().close();
    }
    GetThreadLocalStream().open(file_name, std::ios::out);
  }

  static void StopSecondaryLog() {
    if (GetThreadLocalStream().is_open()) {
      GetThreadLocalStream().close();
    }
  }

  static void Initialize(const std::string& file_name) {
    std::lock_guard<std::mutex> lock(GetInstance().mutex_);
    GetInstance().file_stream_.open(file_name, std::ios::out);
    GetInstance().is_initialized_.store(true, std::memory_order_release);
  }

  static void Finalize() {
    std::lock_guard<std::mutex> lock(GetInstance().mutex_);
    if (GetInstance().file_stream_.is_open()) {
      GetInstance().file_stream_.close();
    }
    GetInstance().is_initialized_.store(false, std::memory_order_release);
  }

 private:
  Logger() = default;
  ~Logger() = default;
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  static Logger& GetInstance() {
    static Logger instance;
    return instance;
  }

  static std::ofstream& GetThreadLocalStream() {
    static thread_local std::ofstream stream;
    return stream;
  }

  std::atomic<bool> is_initialized_{false};
  std::ofstream file_stream_;
  std::mutex mutex_;
};
#endif  // SRC_LOGGER_LOGGER_H_
