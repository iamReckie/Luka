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

#include "DataProcessor/excel_columns.h"
#include "Logger/logger.h"
void QxDataStructure::ConstructDataStructure(std::any& context,
                                             const std::vector<std::any>& args,
                                             std::wstring& key) {
  auto& qx_table = std::any_cast<QxTableMap&>(context);
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  int column = std::any_cast<int>(args[1]);
  std::wstring key_to_string = L"";
  if (column == QxColumns::FIRST_COLUMN) {
    key = input;
    return;
  }
  auto toInt = [](const std::wstring& str) -> int { return std::stoi(str); };
  auto toDouble = [](const std::wstring& str) -> float {
    return std::stod(str);
  };
  auto& current_qx_table = qx_table[key];
  std::shared_ptr<QxTable> new_qx_table;
  switch (column) {
    case QxColumns::RISK_CLASS:
      new_qx_table = std::make_shared<QxTable>();
      current_qx_table.emplace_back(new_qx_table);
      current_qx_table.back()->risk_class = toInt(input);
      break;
    case QxColumns::DRIVER:
      current_qx_table.back()->driver = toInt(input);
      break;
    case QxColumns::SUB1:
      current_qx_table.back()->sub1 = toInt(input);
      break;
    case QxColumns::SUB2:
      current_qx_table.back()->sub2 = toInt(input);
      break;
    case QxColumns::SUB3:
      current_qx_table.back()->sub3 = toInt(input);
      break;
    case QxColumns::SUB4:
      current_qx_table.back()->sub4 = toInt(input);
      break;
    case QxColumns::AGE:
      current_qx_table.back()->age = toInt(input);
      break;
    case QxColumns::MALE:
      current_qx_table.back()->male = toDouble(input);
      break;
    case QxColumns::FEMALE:
      current_qx_table.back()->female = toDouble(input);
      break;
    case QxColumns::QX_NAME:
      current_qx_table.back()->qx_name = input;
      break;
    default:
      break;
  }
}

void QxDataStructure::MergeDataStructure(std::any& target, const std::any& source) {
  auto& target_map = std::any_cast<QxTableMap&>(target);
  const auto& source_map = std::any_cast<const QxTableMap&>(source);

  for (const auto& [key, val] : source_map) {
    auto& target_vec = target_map[key];
    target_vec.insert(target_vec.end(), val.begin(), val.end());
  }
}

void QxDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& qx_table = std::any_cast<const QxTableMap&>(context);
  for (const auto& entry : qx_table) {
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
