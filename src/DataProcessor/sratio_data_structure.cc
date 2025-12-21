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
#include "DataProcessor/sratio_data_structure.h"

#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Logger/logger.h"
void SRatioDataStructure::ConstructDataStructure(
    std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  auto& sratio_table = std::any_cast<SRatioTableMap&>(context);
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  int column = std::any_cast<int>(args[1]);
  int key_to_int{0};
  if (column == 1) {
    key = input;
    return;
  }
  auto toInt = [](const std::wstring& str) -> int { return std::stoi(str); };
  auto toDouble = [](const std::wstring& str) -> float {
    return std::stod(str);
  };
  key_to_int = toInt(key);
  auto& current_sratio_table = sratio_table[key_to_int];
  std::shared_ptr<SRatioTable> new_sratio_table;
  switch (column) {
    case 2:
      new_sratio_table = std::make_shared<SRatioTable>();
      current_sratio_table.emplace_back(new_sratio_table);
      current_sratio_table.back()->name = input;
      break;
    case 3:
      current_sratio_table.back()->standard_price = toDouble(input);
      break;
    case 6:
      current_sratio_table.back()->renewal = toInt(input);
      break;
    case 7:
      current_sratio_table.back()->sex = toInt(input);
      break;
    case 8:
      current_sratio_table.back()->age = toInt(input);
      break;
    case 11:
      current_sratio_table.back()->category = toInt(input);
      break;
    case 12:
      current_sratio_table.back()->real_category = toInt(input);
      break;
    case 13:
      current_sratio_table.back()->due = toInt(input);
      break;
    case 14:
      current_sratio_table.back()->real_due = toInt(input);
      break;
    case 15:
      current_sratio_table.back()->adjust = toDouble(input);
      break;
    case 16:
      current_sratio_table.back()->regular = toDouble(input);
      break;
    case 17:
      current_sratio_table.back()->sratio = toDouble(input);
      break;
    case 18:
      current_sratio_table.back()->min_s = toDouble(input);
      break;
    case 19:
      current_sratio_table.back()->apply_alpha = toDouble(input);
      break;
    case 20:
      current_sratio_table.back()->standard_alpha = toDouble(input);
      break;
    case 21:
      if (input == L"미초과") {
        current_sratio_table.back()->reverse = false;
      } else {
        current_sratio_table.back()->reverse = true;
      }
      break;
    default:
      break;
  }
}
void SRatioDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& sratio_table = std::any_cast<const SRatioTableMap&>(context);
  for (const auto& entry : sratio_table) {
    int dnum = entry.first;
    auto current_sratio_table = entry.second;
    Logger::Log(L"sratio dnum: %d\n", dnum);
    for (const auto& iter : current_sratio_table) {
      Logger::Log(L"  name: %ls", iter->name.c_str());
      Logger::Log(L" standard_price: %lf", iter->standard_price);
      Logger::Log(L" renewal: %d", iter->renewal);
      Logger::Log(L" sex: %d", iter->sex);
      Logger::Log(L" age: %d", iter->age);
      Logger::Log(L" category: %d", iter->category);
      Logger::Log(L" real_category: %d", iter->real_category);
      Logger::Log(L" due: %d", iter->due);
      Logger::Log(L" real_due: %d", iter->real_due);
      Logger::Log(L" adjust: %lf", iter->adjust);
      Logger::Log(L" regular: %lf", iter->regular);
      Logger::Log(L" sratio: %lf", iter->sratio);
      Logger::Log(L" min_s: %lf", iter->min_s);
      Logger::Log(L" apply_alpha: %lf", iter->apply_alpha);
      Logger::Log(L" standard_alpha: %lf", iter->standard_alpha);
      Logger::Log(L" reverse: %d\n", iter->reverse);
    }
  }
}
