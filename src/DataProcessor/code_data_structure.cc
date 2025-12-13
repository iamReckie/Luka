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
#include "DataProcessor/code_data_structure.h"

#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

#include "DataProcessor/excel_columns.h"
#include "Logger/logger.h"
void CodeDataStructure::ConstructDataStructure(
    const std::vector<std::any>& args, std::wstring& key) {
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  int column = std::any_cast<int>(args[1]);
  int key_to_int = 0;
  auto toInt = [](const std::wstring& str) -> int { return std::stoi(str); };
  auto toDouble = [](const std::wstring& str) -> float {
    return std::stod(str);
  };
  if (column == CodeColumns::FIRST_COLUMN) {
    key = input;
    key_to_int = toInt(key);
    code_table_[key_to_int] = std::make_shared<CodeTable>();
    return;
  }
  key_to_int = toInt(key);
  auto current_code_table = code_table_[key_to_int].get();
  switch (column) {
    case CodeColumns::FIRST_COLUMN:
      break;
    case CodeColumns::DNUM:
      current_code_table->dnum = toInt(input);
      break;
    case CodeColumns::NAME:
      current_code_table->name = input;
      break;
    case CodeColumns::QX_KU:
      current_code_table->qx_ku = toInt(input);
      break;
    case CodeColumns::MHJ:
      current_code_table->mhj = toInt(input);
      break;
    case CodeColumns::RE:
      current_code_table->re = toInt(input);
      break;
    case CodeColumns::M_COUNT:
      current_code_table->M_count = toInt(input);
      break;
    default:
      if (column > CodeColumns::QX_TABLE_START && column <= CodeColumns::QX_TABLE_END) {
        current_code_table->qx_table_[input];  // default-construct entry
        table_for_qx_table_[current_index_of_qx_table_++] = input;
      } else if (column >= CodeColumns::QX_VALUES_START) {
        int qx_index = (column - CodeColumns::QX_VALUES_START) / 3;
        if (qx_index < current_index_of_qx_table_) {
          const std::wstring& qx_name = table_for_qx_table_[qx_index];
          current_code_table->qx_table_[qx_name].emplace_back(toDouble(input));
        }
      }
      break;
  }
}
void CodeDataStructure::PrintDataStructure() const {
  for (const auto& entry : code_table_) {
    int code_index = entry.first;
    auto current_code_table = entry.second.get();

    Logger::Log(L"Code Index: %d\n", code_index);
    Logger::Log(L"  dnum: %d\n", current_code_table->dnum);
    Logger::Log(L"  name: %ls\n", current_code_table->name.c_str());
    Logger::Log(L"  qx_ku: %d\n", current_code_table->qx_ku);
    Logger::Log(L"  mhj: %d\n", current_code_table->mhj);
    Logger::Log(L"  re: %d\n", current_code_table->re);
    Logger::Log(L"  M_count: %d\n", current_code_table->M_count);

    // qx_table
    Logger::Log(L"  qx_table:\n");
    for (const auto& qx_entry : current_code_table->qx_table_) {
      Logger::Log(L"    %ls: ", qx_entry.first.c_str());
      for (const float value : qx_entry.second) {
        Logger::Log(L"%.2f ", value);
      }
      Logger::Log(L"\n");
    }
    Logger::Log(L"-----------------------------------\n");
  }
}
