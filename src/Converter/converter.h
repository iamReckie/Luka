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
#ifndef SRC_CONVERTER_CONVERTER_H_
#define SRC_CONVERTER_CONVERTER_H_
#include <codecvt>
#include <cstdlib>
#include <locale>
#include <string>

inline std::wstring Ctw(const std::string& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::wstring wstr = converter.from_bytes(str);
  return wstr;
}
inline std::string Cts(const std::wstring& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  return converter.to_bytes(str);
}
#endif  // SRC_CONVERTER_CONVERTER_H_
