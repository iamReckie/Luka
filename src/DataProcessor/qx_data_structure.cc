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

#include "DataProcessor/excel_columns.h"
#include "Logger/logger.h"
void QxDataStructure::ConstructDataStructure(std::any& context,
                                             const std::vector<std::any>& args,
                                             std::wstring& key) {
  if (args.empty()) {
    return;
  }
  auto& qx_table = std::any_cast<QxTableMap&>(context);
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  int column = std::any_cast<int>(args[1]);
  if (column == QxColumns::FIRST_COLUMN) {
    key = input;
    return;
  }
  auto& entry = qx_table[key];
  if (!entry) {
    entry = std::make_shared<QxTable>();
  }

  auto toInt = [](const std::wstring& str) -> int { return std::stoi(str); };
  auto toDouble = [](const std::wstring& str) -> double { return std::stod(str); };

  switch (column) {
    case QxColumns::AGE:
      entry->pending_age_ = toInt(input);
      break;
    case QxColumns::MALE:
      if (entry->pending_age_ >= 0 && entry->pending_age_ < 120) {
        entry->qx_in[0][entry->pending_age_] = toDouble(input);
      }
      break;
    case QxColumns::FEMALE:
      if (entry->pending_age_ >= 0 && entry->pending_age_ < 120) {
        entry->qx_in[1][entry->pending_age_] = toDouble(input);
      }
      break;
    case QxColumns::QX_NAME:
      entry->qx_name = input;
      break;
    default:
      break;
  }
}

void QxDataStructure::MergeDataStructure(std::any& target, const std::any& source) {
  auto& target_map = std::any_cast<QxTableMap&>(target);
  const auto& source_map = std::any_cast<const QxTableMap&>(source);

  for (const auto& [key, src] : source_map) {
    auto& tgt = target_map[key];
    if (!tgt) {
      tgt = std::make_shared<QxTable>(*src);
    } else {
      for (int g = 0; g < 2; ++g) {
        for (int a = 0; a < 120; ++a) {
          if (src->qx_in[g][a] != 0.0) {
            tgt->qx_in[g][a] = src->qx_in[g][a];
          }
        }
      }
    }
  }
}

void QxDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& qx_table = std::any_cast<const QxTableMap&>(context);
  for (const auto& [key, table] : qx_table) {
    Logger::Log(L"Qx key: %ls name: %ls\n", key.c_str(), table->qx_name.c_str());
    for (int age = 0; age < 120; ++age) {
      if (table->qx_in[0][age] != 0.0 || table->qx_in[1][age] != 0.0) {
        Logger::Log(L"  age: %d male: %lf female: %lf\n",
                    age, table->qx_in[0][age], table->qx_in[1][age]);
      }
    }
  }
}
