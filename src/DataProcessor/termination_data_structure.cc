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
#include "DataProcessor/termination_data_structure.h"

#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Logger/logger.h"
void TerminationDataStructure::ConstructDataStructure(
    const std::vector<std::any>& args, std::wstring& key) {
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  int column = std::any_cast<int>(args[1]);
  int key_to_int{0};
  auto toInt = [](const std::wstring& str) -> int { return std::stoi(str); };
  auto toDouble = [](const std::wstring& str) -> float {
    return std::stod(str);
  };
  if (column == 2) {
    key = input;
    key_to_int = toInt(key);
    termination_table_[key_to_int] = std::make_shared<TerminationTable>();
    return;
  }
  key_to_int = toInt(key);
  auto& current_termination_table = termination_table_[key_to_int];
  switch (column) {
    case 3:
      current_termination_table->ten = toDouble(input);
      break;
    case 4:
      current_termination_table->eleven = toDouble(input);
      break;
    case 5:
      current_termination_table->twelve = toDouble(input);
      break;
    case 6:
      current_termination_table->thirteen = toDouble(input);
      break;
    case 7:
      current_termination_table->fourteen = toDouble(input);
      break;
    case 8:
      current_termination_table->fifteen = toDouble(input);
      break;
    case 9:
      current_termination_table->sixteen = toDouble(input);
      break;
    case 10:
      current_termination_table->seventeen = toDouble(input);
      break;
    case 11:
      current_termination_table->eighteen = toDouble(input);
      break;
    case 12:
      current_termination_table->nineteen = toDouble(input);
      break;
    case 13:
      current_termination_table->twenty = toDouble(input);
      break;
    case 14:
      current_termination_table->twenty_one = toDouble(input);
      break;
    case 15:
      current_termination_table->twenty_two = toDouble(input);
      break;
    case 16:
      current_termination_table->twenty_three = toDouble(input);
      break;
    case 17:
      current_termination_table->twenty_four = toDouble(input);
      break;
    case 18:
      current_termination_table->twenty_five = toDouble(input);
      break;
    case 19:
      current_termination_table->twenty_six = toDouble(input);
      break;
    case 20:
      current_termination_table->twenty_seven = toDouble(input);
      break;
    case 21:
      current_termination_table->twenty_eight = toDouble(input);
      break;
    case 22:
      current_termination_table->twenty_nine = toDouble(input);
      break;
    case 23:
      current_termination_table->thirty = toDouble(input);
      break;
  }
}
void TerminationDataStructure::PrintDataStructure() const {
  for (const auto& entry : termination_table_) {
    int termination_index = entry.first;
    auto termination_table = entry.second;
    Logger::Log(L"Termination index : %d\n", termination_index);
    Logger::Log(L"ten : %lf ", termination_table->ten);
    Logger::Log(L"eleven : %lf ", termination_table->eleven);
    Logger::Log(L"twelve : %lf ", termination_table->twelve);
    Logger::Log(L"thirteen : %lf ", termination_table->thirteen);
    Logger::Log(L"fourteen : %lf ", termination_table->fourteen);
    Logger::Log(L"fifteen : %lf ", termination_table->fifteen);
    Logger::Log(L"sixteen : %lf ", termination_table->sixteen);
    Logger::Log(L"seventeen : %lf ", termination_table->seventeen);
    Logger::Log(L"eighteen : %lf ", termination_table->eighteen);
    Logger::Log(L"nineteen : %lf ", termination_table->nineteen);
    Logger::Log(L"twenty : %lf ", termination_table->twenty);
    Logger::Log(L"twenty-one : %lf ", termination_table->twenty_one);
    Logger::Log(L"twenty-two : %lf ", termination_table->twenty_two);
    Logger::Log(L"twenty-three : %lf ", termination_table->twenty_three);
    Logger::Log(L"twenty-four : %lf ", termination_table->twenty_four);
    Logger::Log(L"twenty-five : %lf ", termination_table->twenty_five);
    Logger::Log(L"twenty-six : %lf ", termination_table->twenty_six);
    Logger::Log(L"twenty-seven : %lf ", termination_table->twenty_seven);
    Logger::Log(L"twenty-eight : %lf ", termination_table->twenty_eight);
    Logger::Log(L"twenty-nine : %lf ", termination_table->twenty_nine);
    Logger::Log(L"thirty : %lf \n", termination_table->thirty);
  }
}
