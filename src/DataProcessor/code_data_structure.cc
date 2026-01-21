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

#include "DataProcessor/data_helper.h"
#include "DataProcessor/excel_columns.h"
#include "DataProcessor/qx_data_structure.h"
#include "Logger/logger.h"
void CodeDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
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
      if (column > CodeColumns::M_COUNT && column < CodeColumns::QX_VALUES_START) {
        // Columns 12-41: qx_key values (C1 = column - 12, so column 12 -> C1=0)
        int c1 = column - 12;
        if (!input.empty()) {
          code_context.qx_key_map[c1] = input;
        }
      } else if (column >= CodeColumns::QX_VALUES_START) {
        // Columns 42+: pay, fst, snd values
        int offset = column - CodeColumns::QX_VALUES_START;
        int c1 = offset / 3;
        int mod = offset % 3;

        // Check if input is empty, if so, stop processing
        if (input.empty()) {
          break;
        }

        // Get qx_key for this C1
        auto qx_key_it = code_context.qx_key_map.find(c1);
        if (qx_key_it == code_context.qx_key_map.end()) {
          break;  // No qx_key for this C1, skip
        }
        const std::wstring& qx_key = qx_key_it->second;

        // Get Qx table name based on qx_ku (0 -> "Qx", 1 -> "Qx1")
        auto data_helper = GetDataHelper();
        std::wstring qx_table_name = data_helper->GetQxNameMapping(current_code_table->qx_ku);
        if (qx_table_name.empty()) {
          qx_table_name = L"Qx";  // Default to "Qx" if not found
        }

        // Get the Qx table data from data_helper
        auto qx_context = data_helper->GetDataContext(qx_table_name);
        if (!qx_context) {
          Logger::Log(L"Warning: Qx table '%ls' not found for qx_key '%ls'\n",
                      qx_table_name.c_str(), qx_key.c_str());
          break;
        }

        // Cast to QxTableMap and find data for this qx_key
        auto& qx_table_map = std::any_cast<QxDataStructure::QxTableMap&>(*qx_context);
        auto qx_data_it = qx_table_map.find(qx_key);
        if (qx_data_it != qx_table_map.end()) {
          // Found qx_key in Qx table, populate qx_in array
          const auto& qx_rows = qx_data_it->second;
          for (const auto& row : qx_rows) {
            int age = row->age;
            if (age >= 0 && age < 120) {
              // Store Male (index 0) and Female (index 1) mortality rates
              current_code_table->qx_in[c1][0][age] = row->male;
              current_code_table->qx_in[c1][1][age] = row->female;
            }
          }
        }

        // Initialize SubCodeTable if not exists
        if (current_code_table->sub_code_table.find(qx_key) == current_code_table->sub_code_table.end()) {
          current_code_table->sub_code_table[qx_key] = std::make_shared<SubCodeTable>();
        }

        auto sub_code = current_code_table->sub_code_table[qx_key].get();

        // C1 * 3 + 42 -> pay, C1 * 3 + 43 -> fst, C1 * 3 + 44 -> snd
        switch (mod) {
          case 0:
            sub_code->pay = toDouble(input);
            break;
          case 1:
            sub_code->fst = toDouble(input);
            break;
          case 2:
            sub_code->snd = toDouble(input);
            break;
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

    // sub_code_table
    Logger::Log(L"  sub_code_table:\n");
    for (const auto& sub_entry : current_code_table->sub_code_table) {
      Logger::Log(L"    qx_key=%ls: pay=%.2f, fst=%.2f, snd=%.2f\n",
                  sub_entry.first.c_str(),
                  sub_entry.second->pay,
                  sub_entry.second->fst,
                  sub_entry.second->snd);
    }
    Logger::Log(L"-----------------------------------\n");
  }
}
