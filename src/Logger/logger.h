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
#include <mutex>
#include <string>
// Logger with lock-free atomic flag and mutex for thread-safe file I/O
class Logger {
 public:
  static void Log(const wchar_t* format, ...) {
    // Quick lock-free atomic check (acquire memory order)
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

    // Lock only for file I/O
    {
      std::lock_guard<std::mutex> lock(GetInstance().file_mutex_);
      if (GetInstance().log_file.is_open()) {
        printf("%s", str_buffer.c_str());
        GetInstance().log_file << str_buffer << std::flush;
      }
    }
  }

  static void Log(const char* format, ...) {
    // Quick lock-free atomic check (acquire memory order)
    if (!GetInstance().is_initialized_.load(std::memory_order_acquire)) {
      return;
    }

    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Lock only for file I/O
    {
      std::lock_guard<std::mutex> lock(GetInstance().file_mutex_);
      if (GetInstance().log_file.is_open()) {
        printf("%s", buffer);
        GetInstance().log_file << buffer << std::flush;
      }
    }
  }

  static void Initialize(const std::string& file_name) {
    std::lock_guard<std::mutex> lock(GetInstance().file_mutex_);
    if (std::ifstream(file_name)) {
      std::remove(file_name.c_str());
    }
    GetInstance().log_file.open(file_name, std::ios::app);
    // Set initialized flag with release memory order
    GetInstance().is_initialized_.store(true, std::memory_order_release);
  }

  static void Finalize() {
    std::lock_guard<std::mutex> lock(GetInstance().file_mutex_);
    GetInstance().is_initialized_.store(false, std::memory_order_release);
    if (GetInstance().log_file.is_open()) {
      GetInstance().log_file.close();
    }
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

  std::ofstream log_file;
  std::mutex file_mutex_;                      // Protects log_file access
  std::atomic<bool> is_initialized_{false};    // Lock-free initialization flag
};
#endif  // SRC_LOGGER_LOGGER_H_
