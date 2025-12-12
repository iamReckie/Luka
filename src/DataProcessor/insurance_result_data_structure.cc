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
#include "DataProcessor/insurance_result_data_structure.h"

#include <any>
#include <string>
#include <vector>

#include "DataProcessor/data_helper.h"
#include "Logger/logger.h"
void InsuranceResultDataStructure::ConstructDataStructure(const std::vector<std::any> &args, std::wstring &key) {
  std::shared_ptr<InsuranceResultIndex> result_index = std::any_cast<std::shared_ptr<InsuranceResultIndex>>(args[0]);
  std::shared_ptr<DataHelper> data_helper = data_helper_.lock();
  auto table_data = std::dynamic_pointer_cast<TableDataStructure>(data_helper->GetDataStructure(key));
  auto code_data = std::dynamic_pointer_cast<CodeDataStructure>(data_helper->GetDataStructure(L"Code"));
  for (const auto &iter1 : table_data->get_data_structure()) {
    for (const auto &iter2 : iter1.second) {
      std::shared_ptr<InsuranceResult> result = std::make_shared<InsuranceResult>();
      result->bojong = iter2[result_index->bojong];
      result->nn = iter2[result_index->nn];
      result->mm = iter2[result_index->mm];
      result->x = iter2[result_index->x];
      result->AMT = iter2[result_index->AMT];
      result->GP_Input[result_index->GP_Input[0][0]][result_index->GP_Input[0][1]] =
          iter2[result_index->GP_Input[0][2]];
      result->GP_Input[result_index->GP_Input[0][0]][result_index->GP_Input[0][1]] =
          iter2[result_index->GP_Input[0][2]];
      result->dnum = code_data->GetDataStructure()[result->bojong]->dnum;
      insurance_result_.emplace_back(result);
    }
  }
}
void InsuranceResultDataStructure::PrintDataStructure() const {
  for (const auto &insurance_result_ : insurance_result_) {
    Logger::Log(
        L"InsuranceResult: bojong: %d, dnum: %d, nn: %d, mm: %d, x: %d, AMT: "
        L"%d\n",
        insurance_result_->bojong, insurance_result_->dnum, insurance_result_->nn, insurance_result_->mm, insurance_result_->x, insurance_result_->AMT);
    for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 10; ++j) {
        if (insurance_result_->GP_Input[i][j] == 0) {
          continue;
        }  // Only log non-zero values
        Logger::Log(L"GP_Input[%d][%d]: %d\n", i, j, insurance_result_->GP_Input[i][j]);
      }
    }
  }
}
