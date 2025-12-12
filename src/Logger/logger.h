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
#include <codecvt>
#include <cstdarg>
#include <fstream>
#include <iostream>
#include <locale>
#include <string>
// Logger is defined as a static
class Logger {
 public:
  static void Log(const wchar_t* format, ...) {
    // this is single instance. if log file is not constructed, return.
    if (!GetInstance().log_file.is_open()) {
      return;
    }

    va_list args;
    va_start(args, format);
    wchar_t buffer[2048];
    vswprintf(buffer, sizeof(buffer) / sizeof(wchar_t), format, args);
    va_end(args);
    std::wstring wstr_buffer(buffer);
    // std::string str_buffer(wstr_buffer.begin(), wstr_buffer.end());
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string str_buffer = converter.to_bytes(wstr_buffer);
    printf("%s", str_buffer.c_str());
    GetInstance().log_file << str_buffer << std::flush;
  }
  static void Log(const char* format, ...) {
    // this is single instance. if log file is not constructed, return.
    if (!GetInstance().log_file.is_open()) {
      return;
    }
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    printf("%s", buffer);
    // write log
    GetInstance().log_file << buffer << std::flush;
  }
  static void Initialize(const std::string& file_name) {
    if (std::ifstream(file_name)) {
      std::remove(file_name.c_str());
    }
    GetInstance().log_file.open(file_name, std::ios::app);
  }
  static void Finalize() {
    if (GetInstance().log_file.is_open()) {
      GetInstance().log_file.close();
    } else {
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
};
#endif  // SRC_LOGGER_LOGGER_H_
