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
#include "CommandProcessor/calc_reserve_expense.h"

#include <sstream>
#include <string>
#include <vector>

#include "DataProcessor/reserve_result_data_structure.h"
#include "Logger/logger.h"
#include "Utility/string_utils.h"
std::vector<std::wstring>
CalcReserveExpenseCommand::SplitAndConvertToWString(const std::string &input) {
  std::vector<std::wstring> result;
  std::string token;
  std::istringstream stream(input);

  // Split by ':' and convert each token to wstring using Ctw
  while (std::getline(stream, token, ':')) {
    result.push_back(Ctw(token));
  }

  return result;
}
void CalcReserveExpenseCommand::Execute(const YAML::Node &command_data) {
  std::wstring file_name = Ctw(command_data["name"].as<std::string>());

  auto insurance_result_data_structure =
      data_helper_->GetDataStructure(file_name + L"InsuranceResult");
  // Create reserve result index data structure
  // std::unique_ptr<ReserveResultIndex> reserve_result_index =
  //     std::shared_unique<ReserveResultIndex>();
  ReserveResultIndex reserve_result_index;
  // Start making yaml data structure
  if (command_data["variables"]) {
    for (const auto &var : command_data["variables"]) {
      std::wstring name = Ctw(var["name"].as<std::string>());
      if (name == L"tVn_Input") {
        if (var["index"].IsSequence()) {
          for (const auto &idx : var["index"]) {
            std::string string_value = idx.as<std::string>();
            std::vector<std::wstring> tokens =
                SplitAndConvertToWString(string_value);
            reserve_result_index.tVn_Input.push_back(tokens);
          }
        }
      } else if (name == L"Alpha_ALD_Input") {
        if (var["index"].IsSequence()) {
          for (const auto &idx : var["index"]) {
            std::string string_value = idx.as<std::string>();
            std::vector<std::wstring> tokens =
                SplitAndConvertToWString(string_value);
            reserve_result_index.Alpha_ALD_Input.push_back(tokens);
          }
        }
      } else if (name == L"NP_beta_Input") {
        if (var["index"].IsSequence()) {
          for (const auto &idx : var["index"]) {
            std::string string_value = idx.as<std::string>();
            std::vector<std::wstring> tokens =
                SplitAndConvertToWString(string_value);
            reserve_result_index.NP_beta_Input.push_back(tokens);
          }
        }
      } else if (name == L"STD_NP_Input") {
        if (var["index"].IsSequence()) {
          for (const auto &idx : var["index"]) {
            std::string string_value = idx.as<std::string>();
            std::vector<std::wstring> tokens =
                SplitAndConvertToWString(string_value);
            reserve_result_index.STD_NP_Input.push_back(tokens);
          }
        }
      } else {
        Logger::Log(L"Unknown variable name: %s\n", name.c_str());
      }
    }
  }
  std::vector<std::any> args = {reserve_result_index};
  data_helper_->ExecuteData(file_name + L"ReserveResult", file_name,
                            L"ReserveResult", args);
}
