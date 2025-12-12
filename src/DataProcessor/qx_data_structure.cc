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
#include "DataProcessor/qx_data_structure.h"

#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Logger/logger.h"
void QxDataStructure::ConstructDataStructure(const std::vector<std::any>& args,
                                             std::wstring& key) {
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  int column = std::any_cast<int>(args[1]);
  std::wstring key_to_string = L"";
  if (column == 1) {
    key = input;
    return;
  }
  auto toInt = [](const std::wstring& str) -> int { return std::stoi(str); };
  auto toDouble = [](const std::wstring& str) -> float {
    return std::stod(str);
  };
  auto& current_qx_table = qx_table_[key];
  std::shared_ptr<QxTable> new_qx_table;
  switch (column) {
    case 2:
      new_qx_table = std::make_shared<QxTable>();
      current_qx_table.emplace_back(new_qx_table);
      current_qx_table.back()->risk_class = toInt(input);
      break;
    case 3:
      current_qx_table.back()->driver = toInt(input);
      break;
    case 4:
      current_qx_table.back()->sub1 = toInt(input);
      break;
    case 5:
      current_qx_table.back()->sub2 = toInt(input);
      break;
    case 6:
      current_qx_table.back()->sub3 = toInt(input);
      break;
    case 7:
      current_qx_table.back()->sub4 = toInt(input);
      break;
    case 8:
      current_qx_table.back()->age = toInt(input);
      break;
    case 9:
      current_qx_table.back()->male = toDouble(input);
      break;
    case 10:
      current_qx_table.back()->female = toDouble(input);
      break;
    case 11:
      current_qx_table.back()->qx_name = input;
      break;
    default:
      break;
  }
}
void QxDataStructure::PrintDataStructure() const {
  for (const auto& entry : qx_table_) {
    std::wstring qx_name = entry.first;
    auto current_qx_table = entry.second;
    Logger::Log(L"Qx name: %ls\n", qx_name.c_str());
    for (const auto& iter : current_qx_table) {
      Logger::Log(L"  risk_class: %d", iter->risk_class);
      Logger::Log(L" driver: %d", iter->driver);
      Logger::Log(L" sub1: %d", iter->sub1);
      Logger::Log(L" sub2: %d", iter->sub2);
      Logger::Log(L" sub3: %d", iter->sub3);
      Logger::Log(L" sub4: %d", iter->sub4);
      Logger::Log(L" age: %d", iter->age);
      Logger::Log(L" male: %lf", iter->male);
      Logger::Log(L" female: %lf", iter->female);
      Logger::Log(L" qx_name: %ls\n", iter->qx_name.c_str());
    }
  }
}
