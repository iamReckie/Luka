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
    std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  auto& code_context = std::any_cast<CodeDataContext&>(context);
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
    code_context.code_table[key_to_int] = std::make_shared<CodeTable>();
    return;
  }
  key_to_int = toInt(key);
  auto current_code_table = code_context.code_table[key_to_int].get();
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
        code_context.table_for_qx_table[code_context.current_index_of_qx_table++] = input;
      } else if (column >= CodeColumns::QX_VALUES_START) {
        int qx_index = (column - CodeColumns::QX_VALUES_START) / 3;
        if (qx_index < code_context.current_index_of_qx_table) {
          const std::wstring& qx_name = code_context.table_for_qx_table[qx_index];
          current_code_table->qx_table_[qx_name].emplace_back(toDouble(input));
        }
      }
      break;
  }
}

void CodeDataStructure::MergeDataStructure(std::any& target, const std::any& source) {
  auto& target_ctx = std::any_cast<CodeDataContext&>(target);
  const auto& source_ctx = std::any_cast<const CodeDataContext&>(source);

  for (const auto& [key, val] : source_ctx.code_table) {
    target_ctx.code_table[key] = val;
  }
}

void CodeDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& code_context = std::any_cast<const CodeDataContext&>(context);
  for (const auto& entry : code_context.code_table) {
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
