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
#include "CommandProcessor/calc_insurance_output.h"

#include <sstream>
#include <string>
#include <vector>

#include "DataProcessor/insurance_output_data_structure.h"
#include "Logger/logger.h"
#include "Utility/string_utils.h"
std::vector<std::wstring>
CalcInsuranceOutputCommand::SplitAndConvertToWString(const std::string &input) {
  std::vector<std::wstring> result;
  std::string token;
  std::istringstream stream(input);

  // Split by ':' and convert each token to wstring using Ctw
  while (std::getline(stream, token, ':')) {
    result.push_back(Ctw(token));
  }

  return result;
}

void CalcInsuranceOutputCommand::ProcessVariables(
    const YAML::Node &variables_node,
    InsuranceOutputIndex &insurance_output_result_index) {
  for (const auto &var : variables_node) {
    std::wstring name = Ctw(var["name"].as<std::string>());
    if (name == L"tVn_Input") {
      if (var["index"].IsSequence()) {
        for (const auto &idx : var["index"]) {
          std::string string_value = idx.as<std::string>();
          std::vector<std::wstring> tokens =
              SplitAndConvertToWString(string_value);
          insurance_output_result_index.tVn_Input.push_back(tokens);
        }
      }
    } else if (name == L"Alpha_ALD_Input") {
      if (var["index"].IsSequence()) {
        for (const auto &idx : var["index"]) {
          std::string string_value = idx.as<std::string>();
          std::vector<std::wstring> tokens =
              SplitAndConvertToWString(string_value);
          insurance_output_result_index.Alpha_ALD_Input.push_back(tokens);
        }
      }
    } else if (name == L"NP_beta_Input") {
      if (var["index"].IsSequence()) {
        for (const auto &idx : var["index"]) {
          std::string string_value = idx.as<std::string>();
          std::vector<std::wstring> tokens =
              SplitAndConvertToWString(string_value);
          insurance_output_result_index.NP_beta_Input.push_back(tokens);
        }
      }
    } else if (name == L"STD_NP_Input") {
      if (var["index"].IsSequence()) {
        for (const auto &idx : var["index"]) {
          std::string string_value = idx.as<std::string>();
          std::vector<std::wstring> tokens =
              SplitAndConvertToWString(string_value);
          insurance_output_result_index.STD_NP_Input.push_back(tokens);
        }
      }
    } else {
      Logger::Log(L"Unknown variable name: %s\n", name.c_str());
    }
  }
}

void CalcInsuranceOutputCommand::ProcessFile(
    const std::wstring &file_name,
    const YAML::Node &variables_node,
    InsuranceOutputIndex &insurance_output_result_index) {
  auto insurance_result_data_structure =
      data_helper_->GetDataStructure(file_name + L"InsuranceResult");

  if (variables_node) {
    ProcessVariables(variables_node, insurance_output_result_index);
  }
}

void CalcInsuranceOutputCommand::Execute(const YAML::Node &command_data) {
  InsuranceOutputIndex insurance_output_result_index;
  std::wstring primary_file_name;

  // Check if new format with "files" array
  if (command_data["files"] && command_data["files"].IsSequence()) {
    Logger::Log(L"Processing multiple files from 'files' array\n");
    for (const auto &file_node : command_data["files"]) {
      std::wstring file_name = Ctw(file_node["name"].as<std::string>());
      Logger::Log(L"Processing file: %ls\n", file_name.c_str());
      if (primary_file_name.empty()) {
        primary_file_name = file_name;
      }
      ProcessFile(file_name, file_node["variables"], insurance_output_result_index);
    }
  } else if (command_data["name"]) {
    // Legacy format with single "name" and "variables"
    Logger::Log(L"Processing single file (legacy format)\n");
    primary_file_name = Ctw(command_data["name"].as<std::string>());
    ProcessFile(primary_file_name, command_data["variables"], insurance_output_result_index);
  } else {
    Logger::Log(L"Error: Invalid command format - no 'files' or 'name' field\n");
    return;
  }

  // Execute and print data once after processing all files
  std::vector<std::any> args = {insurance_output_result_index};
  data_helper_->ExecuteData(+L"InsuranceOutput", primary_file_name, L"InsuranceOutput", args);
  data_helper_->PrintData(+L"InsuranceOutput");
}
