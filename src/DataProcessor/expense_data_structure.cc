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
void ExpenseDataStructure::ConstructDataStructure(
    std::any& context, const std::vector<std::any>& args, std::wstring& key) {
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
  std::shared_ptr<ExpenseTable> new_qx_table;
  switch (column) {
    case 2:
      new_qx_table = std::make_shared<ExpenseTable>();
      current_expense_table.emplace_back(new_qx_table);
      current_expense_table.back()->mm = toInt(input);
      break;
    case 3:
      current_expense_table.back()->ap = toDouble(input);
      break;
    case 4:
      current_expense_table.back()->bp = toDouble(input);
      break;
    case 5:
      current_expense_table.back()->bs = toDouble(input);
      break;
    case 6:
      current_expense_table.back()->b2 = toDouble(input);
      break;
    case 7:
      current_expense_table.back()->bo = toDouble(input);
      break;
    default:
      break;
  }
}
void ExpenseDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& expense_table = std::any_cast<const ExpenseTableMap&>(context);
  for (const auto& entry : expense_table) {
    int dnum = entry.first;
    auto current_expense_table = entry.second;
    Logger::Log(L"expense dnum: %d\n", dnum);
    for (const auto& iter : current_expense_table) {
      Logger::Log(L"  mm: %d", iter->mm);
      Logger::Log(L" ap: %lf", iter->ap);
      Logger::Log(L" bp: %lf", iter->bp);
      Logger::Log(L" bs: %lf", iter->bs);
      Logger::Log(L" b2: %lf", iter->b2);
      Logger::Log(L" bo: %lf\n", iter->bo);
    }
  }
}
