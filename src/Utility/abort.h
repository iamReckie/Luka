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
#ifndef SRC_UTILITY_ABORT_H_
#define SRC_UTILITY_ABORT_H_

#include <cstdarg>
#include <cstdlib>

#include "Logger/logger.h"

inline void Abort(const wchar_t* format, ...) {
  va_list args;
  va_start(args, format);

  // Create buffer for formatted message
  wchar_t buffer[1024];
  vswprintf(buffer, sizeof(buffer) / sizeof(wchar_t), format, args);

  va_end(args);

  // Log the error message
  Logger::Log(L"FATAL ERROR: %ls", buffer);

  // Terminate the program
  std::exit(EXIT_FAILURE);
}

#endif  // SRC_UTILITY_ABORT_H_
