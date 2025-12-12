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

#include "DataProcessor/data_helper.h"
#include "Logger/logger.h"
void ReserveResultDataStructure::ConstructDataStructure(const std::vector<std::any>& args, std::wstring& key) {
  ReserveResultIndex reserve_result_index = std::any_cast<ReserveResultIndex>(args[0]);
  for (const auto& vec : reserve_result_index.tVn_Input) {
    for (const auto& str : vec) {
      Logger::Log(L"tVn_Input element: %ls\n", str.c_str());
    }
  }
  std::shared_ptr<DataHelper> data_helper = data_helper_.lock();
  // Raw data is saved in TableDataStructure. So we have to get it first.
  auto table_data = std::dynamic_pointer_cast<TableDataStructure>(data_helper->GetDataStructure(key));
  auto insurance_data = std::dynamic_pointer_cast<InsuranceResultDataStructure>(data_helper->GetDataStructure(L"InsuranceResult"));
  // We need to get nn from insurance data
  for (const auto& insurance_result : insurance_data->GetInsuranceResults()) {
    for (int i = 0; i < insurance_result->nn; ++i) {
      ReserveResult reserve_result{};
      reserve_result_data_structure_.emplace_back(reserve_result);
    }
  }
  Logger::Log(L"Constructing ReserveResultDataStructure with key: %ls, temp: %d\n",
              key.c_str(), reserve_result_index.tVn_Input.size());
}
void ReserveResultDataStructure::PrintDataStructure() const {
  Logger::Log(L"ReserveResultDataStructure contents:\n");
}
