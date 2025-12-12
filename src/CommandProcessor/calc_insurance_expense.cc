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
#include "CommandProcessor/calc_insurance_expense.h"

#include <memory>
#include <string>
#include <vector>

#include "Converter/converter.h"
#include "DataProcessor/insurance_result_data_structure.h"
#include "Logger/logger.h"

std::vector<int>
CalcInsuranceExpenseCommand::get_GP_Input_range(const std::wstring &input) {
  std::vector<int> ranges;
  std::wstringstream ss(input);
  std::wstring token;
  auto excel_column_to_number = [](const std::wstring &column) -> int {
    int result = 0;
    for (char c : column) {
      result = result * 26 + (c - 'A' + 1);
    }
    return result;
  };
  auto is_numeric = [](const std::wstring &input) -> bool {
    return !input.empty() && std::all_of(input.begin(), input.end(), ::isdigit);
  };
  while (std::getline(ss, token, L':')) {
    if (!is_numeric(token)) {
      ranges.push_back(excel_column_to_number(token));
    } else {
      ranges.push_back(std::stoi(token));
    }
  }
  return ranges;
}

void CalcInsuranceExpenseCommand::Execute(const YAML::Node &command_data) {
  std::wstring file_name = Ctw(command_data["name"].as<std::string>());
  std::vector<int> ranges;
  Logger::Log(L"Calculating insurance expense from %ls\n", file_name.c_str());
  std::shared_ptr<InsuranceResultIndex> result =
      std::make_shared<InsuranceResultIndex>();
  if (command_data["variables"]) {
    for (const auto &var : command_data["variables"]) {
      std::string name = var["name"].as<std::string>();
      if (name == "bojong" && var["index"]) {
        result->bojong = var["index"].as<int>();
      } else if (name == "nn" && var["index"]) {
        result->nn = var["index"].as<int>();
      } else if (name == "mm" && var["index"]) {
        result->mm = var["index"].as<int>();
      } else if (name == "x" && var["index"]) {
        result->x = var["index"].as<int>();
      } else if (name == "AMT" && var["index"]) {
        result->AMT = var["index"].as<int>();
      } else if (name == "GP_Input" && var["index"]) {
        if (var["index"].IsSequence()) {
          for (const auto &idx : var["index"]) {
            std::string string_value = idx.as<std::string>();
            auto temp_ranges = get_GP_Input_range(Ctw(string_value));
            result->GP_Input.push_back(temp_ranges);
          }
        } else {
          std::string string_value = var["index"].as<std::string>();
          auto temp_ranges = get_GP_Input_range(Ctw(string_value));
          result->GP_Input.push_back(temp_ranges);
        }
      }
    }
    std::vector<std::any> args = {result};
    data_helper_->ExecuteData(L"InsuranceResult", file_name, L"InsuranceResult", args);
    data_helper_->PrintData(L"InsuranceResult");
  }
}
