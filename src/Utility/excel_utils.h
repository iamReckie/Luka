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
#ifndef SRC_UTILITY_EXCEL_UTILS_H_
#define SRC_UTILITY_EXCEL_UTILS_H_
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

class ExcelUtils {
 public:
  // Parses Excel range string (e.g., "A1:Z10") into vector of integers
  // Returns [start_row, end_row, start_col, end_col]
  static std::vector<int> ParseExcelRange(const std::wstring& input) {
    std::vector<int> ranges;
    std::wstringstream ss(input);
    std::wstring token;

    auto excel_column_to_number = [](const std::wstring& column) -> int {
      int result = 0;
      for (char c : column) {
        result = result * 26 + (c - 'A' + 1);
      }
      return result;
    };

    auto is_numeric = [](const std::wstring& str) -> bool {
      return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
    };

    while (std::getline(ss, token, L':')) {
      if (!is_numeric(token)) {
        ranges.push_back(excel_column_to_number(token));
      } else {
        ranges.push_back(std::stoi(token));
      }
    }
    return ranges;
  }
};

#endif  // SRC_UTILITY_EXCEL_UTILS_H_
