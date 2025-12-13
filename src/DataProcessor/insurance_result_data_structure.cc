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

#include "Converter/string_utils.h"
#include "DataProcessor/data_helper.h"
#include "Logger/logger.h"
void InsuranceResultDataStructure::ConstructDataStructure(const std::vector<std::any> &args, std::wstring &key) {
  try {
    std::shared_ptr<InsuranceResultIndex> result_index = std::any_cast<std::shared_ptr<InsuranceResultIndex>>(args[0]);
    std::shared_ptr<DataHelper> data_helper = GetDataHelper();
    auto table_data = std::dynamic_pointer_cast<TableDataStructure>(data_helper->GetDataStructure(key));
    if (!table_data) {
      Logger::Log(L"Error: Failed to cast to TableDataStructure for key: %ls\n", key.c_str());
      return;
    }
    auto code_data = std::dynamic_pointer_cast<CodeDataStructure>(data_helper->GetDataStructure(L"Code"));
    if (!code_data) {
      Logger::Log(L"Error: Failed to cast to CodeDataStructure\n");
      return;
    }
    for (const auto &iter1 : table_data->get_data_structure()) {
      for (const auto &iter2 : iter1.second) {
        std::shared_ptr<InsuranceResult> result = std::make_shared<InsuranceResult>();
        result->bojong = iter2[result_index->bojong];
        result->nn = iter2[result_index->nn];
        result->mm = iter2[result_index->mm];
        result->x = iter2[result_index->x];
        result->AMT = iter2[result_index->AMT];
        result->dnum = code_data->GetDataStructure()[result->bojong]->dnum;

        // Initialize GP_Input as a 2D vector
        result->GP_Input.assign(10, std::vector<int>(10, 0));
        for (const auto& gp_range : result_index->GP_Input) {
          if (gp_range.size() >= 3) {
            if (gp_range[0] < 10 && gp_range[1] < 10 && gp_range[2] < static_cast<int>(iter2.size())) {
              result->GP_Input[gp_range[0]][gp_range[1]] = iter2[gp_range[2]];
            }
          }
        }
        insurance_result_.emplace_back(result);
      }
    }
  } catch (const std::exception &e) {
    Logger::Log(L"Error in ConstructDataStructure: %ls\n", Ctw(e.what()).c_str());
  }
}
void InsuranceResultDataStructure::PrintDataStructure() const {
  for (const auto &insurance_result_ : insurance_result_) {
    Logger::Log(
        L"InsuranceResult: bojong: %d, dnum: %d, nn: %d, mm: %d, x: %d, AMT: %d\n",
        insurance_result_->bojong, insurance_result_->dnum, insurance_result_->nn,
        insurance_result_->mm, insurance_result_->x, insurance_result_->AMT);
    for (size_t i = 0; i < insurance_result_->GP_Input.size(); ++i) {
      for (size_t j = 0; j < insurance_result_->GP_Input[i].size(); ++j) {
        if (insurance_result_->GP_Input[i][j] == 0) {
          continue;
        }
        Logger::Log(L"  GP_Input[%zu][%zu]: %d\n", i, j,
                    insurance_result_->GP_Input[i][j]);
      }
    }
  }
}
