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
#include "DataProcessor/expense_data_structure.h"

#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Logger/logger.h"
void ExpenseDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  auto& expense_table = std::any_cast<ExpenseTableMap&>(context);
  int key_to_int{0};
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  int column = std::any_cast<int>(args[1]);
  if (column == 1) {
    key = input;
    return;
  }
  auto toInt = [](const std::wstring& str) -> int { return std::stoi(str); };
  auto toDouble = [](const std::wstring& str) -> float {
    return std::stod(str);
  };
  key_to_int = toInt(key);
  auto& current_expense_table = expense_table[key_to_int];
  if (!current_expense_table) {
    current_expense_table = std::make_shared<ExpenseTable>();
  }
  int mm = current_expense_table->mm;
  switch (column) {
    case 2:
      current_expense_table->mm = toInt(input);
      break;
    case 3:
      current_expense_table->alp_in[key_to_int][mm] = toDouble(input);
      break;
    case 4:
      current_expense_table->beta1_in[key_to_int][mm] = toDouble(input);
      break;
    case 5:
      current_expense_table->beta2_in[key_to_int][mm] = toDouble(input);
      break;
    case 6:
      current_expense_table->beta3_in[key_to_int][mm] = toDouble(input);
      break;
    case 7:
      current_expense_table->gamma_in[key_to_int][mm] = toDouble(input);
      break;
    default:
      break;
  }
}

void ExpenseDataStructure::MergeDataStructure(std::any& target, const std::any& source) {
  auto& target_map = std::any_cast<ExpenseTableMap&>(target);
  const auto& source_map = std::any_cast<const ExpenseTableMap&>(source);

  for (const auto& [key, val] : source_map) {
    if (!target_map.count(key)) {
      target_map[key] = val;
    }
  }
}

void ExpenseDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& expense_table = std::any_cast<const ExpenseTableMap&>(context);
  for (const auto& [key, table] : expense_table) {
    Logger::Log(L"expense key: %d\n", key);
    for (int m = 0; m < static_cast<int>(table->alp_in[key].size()); ++m) {
      Logger::Log(L"  mm: %d", m);
      Logger::Log(L" alp_in: %lf", table->alp_in[key][m]);
      Logger::Log(L" beta1_in: %lf", table->beta1_in[key][m]);
      Logger::Log(L" beta2_in: %lf", table->beta2_in[key][m]);
      Logger::Log(L" beta3_in: %lf", table->beta3_in[key][m]);
      Logger::Log(L" gamma_in: %lf\n", table->gamma_in[key][m]);
    }
  }
}
