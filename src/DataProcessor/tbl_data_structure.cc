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
#include <any>
#include <sstream>
#include <string>

#include "DataProcessor/tbl_data_structure.h"
#include "Logger/logger.h"
void TableDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  auto& table_data_structure = std::any_cast<TableDataMap&>(context);
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  std::wstringstream ss(input);
  std::wstring token;
  std::vector<int> numbers;
  bool stop = false;
  auto parse_and_push = [&](const std::wstring& str) {
    if (str.find(L'\r') != std::wstring::npos) {
      stop = true;
      return;
    }
    numbers.push_back(std::stod(str));
  };
  while (std::getline(ss, token, L'\t')) {
    parse_and_push(token);
    if (stop) {
      break;
    }
  }
  table_data_structure[key].emplace_back(numbers);
}
void TableDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& table_data_structure = std::any_cast<const TableDataMap&>(context);
  for (const auto& [key, value] : table_data_structure) {
    Logger::Log(L"Key: %ls\n", key.c_str());
    if (key == L"67868") {
      printf("Hello world!\n");
    }
    for (const auto& row : value) {
      std::wstring row_str;
      for (const auto& num : row) {
        row_str += std::to_wstring(num) + L" ";
      }
      Logger::Log("%ls\n", row_str.c_str());
    }
  }
}
