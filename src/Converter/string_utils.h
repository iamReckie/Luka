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
#ifndef SRC_CONVERTER_STRING_UTILS_H_
#define SRC_CONVERTER_STRING_UTILS_H_
#include <codecvt>
#include <locale>
#include <string>

class StringUtils {
 public:
  // Convert std::string to std::wstring (UTF-8)
  static std::wstring CharToWide(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(str);
  }

  // Convert std::wstring to std::string (UTF-8)
  static std::string WideToChar(const std::wstring& str) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(str);
  }
};

// Backward compatibility aliases
inline std::wstring Ctw(const std::string& str) {
  return StringUtils::CharToWide(str);
}

inline std::string Cts(const std::wstring& str) {
  return StringUtils::WideToChar(str);
}

#endif  // SRC_CONVERTER_STRING_UTILS_H_
