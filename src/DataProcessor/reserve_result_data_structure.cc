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
#include "DataProcessor/reserve_result_data_structure.h"

#include <any>
#include <string>
#include <vector>

#include "Utility/string_utils.h"
#include "DataProcessor/data_helper.h"
#include "Logger/logger.h"
void ReserveResultDataStructure::ConstructDataStructure(const std::vector<std::any>& args, std::wstring& key) {
  try {
    ReserveResultIndex reserve_result_index = std::any_cast<ReserveResultIndex>(args[0]);
    for (const auto& vec : reserve_result_index.tVn_Input) {
      for (const auto& str : vec) {
        Logger::Log(L"tVn_Input element: %ls\n", str.c_str());
      }
    }
    std::shared_ptr<DataHelper> data_helper = GetDataHelper();
    // Raw data is saved in TableDataStructure. So we have to get it first.
    auto table_data = std::dynamic_pointer_cast<TableDataStructure>(data_helper->GetDataStructure(key));
    if (!table_data) {
      Logger::Log(L"Error: Failed to cast to TableDataStructure for key: %ls\n", key.c_str());
      return;
    }
    auto insurance_data = std::dynamic_pointer_cast<InsuranceResultDataStructure>(data_helper->GetDataStructure(L"InsuranceResult"));
    if (!insurance_data) {
      Logger::Log(L"Error: Failed to cast to InsuranceResultDataStructure\n");
      return;
    }
    // We need to get nn from insurance data
    for (const auto& _ : insurance_data->GetInsuranceResults()) {
      ReserveResult reserve_result{};
      // Initialize vectors with proper sizes
      reserve_result.tVn_Input.assign(2, std::vector<double>(120, 0.0));
      reserve_result.Alpha_ALD_Input.assign(2, 0.0);
      reserve_result.NP_beta_Input.assign(2, 0.0);
      reserve_result.STD_NP_Input.assign(2, 0.0);
      reserve_result_data_structure_.emplace_back(reserve_result);
    }
    Logger::Log(L"Constructing ReserveResultDataStructure with key: %ls, size: %d\n",
                key.c_str(), static_cast<int>(reserve_result_index.tVn_Input.size()));
  } catch (const std::exception &e) {
    Logger::Log(L"Error in ReserveResultDataStructure::ConstructDataStructure: %ls\n", Ctw(e.what()).c_str());
  }
}
void ReserveResultDataStructure::PrintDataStructure() const {
  Logger::Log(L"ReserveResultDataStructure contents:\n");
}
