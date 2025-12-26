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
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataProcessor/code_data_structure.h"
#include "DataProcessor/data_helper.h"
#include "DataProcessor/tbl_data_structure.h"
#include "Logger/logger.h"
#include "Utility/string_utils.h"

// Assuming these types based on usage in the project
using TableDataMap = TableDataStructure::TableDataMap;
// using CodeDataMap = std::unordered_map<int, std::shared_ptr<CodeData>>; // Removed incorrect alias

void InsuranceResultDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  auto& insurance_result = std::any_cast<InsuranceResultList&>(context);

  try {
    if (args.empty()) {
      Logger::Log(L"Error: Arguments empty for InsuranceResultDataStructure\n");
      return;
    }
    std::shared_ptr<InsuranceResultIndex> result_index = std::any_cast<std::shared_ptr<InsuranceResultIndex>>(args[0]);
    if (!result_index) {
      Logger::Log(L"Error: result_index is null\n");
      return;
    }
    std::shared_ptr<DataHelper> data_helper = GetDataHelper();

    if (!data_helper) {
      Logger::Log(L"Error: DataHelper is null\n");
      return;
    }

    // Get Table Data Context
    std::any* table_data_any = data_helper->GetDataContext(key);
    if (!table_data_any) {
      Logger::Log(L"Error: Table data context not found for key: %ls\n", key.c_str());
      return;
    }

    // Get Code Data Context
    std::any* code_data_any = data_helper->GetDataContext(L"Code");
    if (!code_data_any) {
      Logger::Log(L"Error: Code data context not found\n");
      return;
    }

    // Cast to expected types
    const auto& table_data = std::any_cast<const TableDataMap&>(*table_data_any);
    const auto& code_context = std::any_cast<const CodeDataContext&>(*code_data_any);
    const auto& code_map = code_context.code_table;

    for (const auto& iter1 : table_data) {
      for (const auto& iter2 : iter1.second) {
        std::shared_ptr<InsuranceResult> result = std::make_shared<InsuranceResult>();
        result->GP_Input.resize(10, std::vector<int>(10, 0));
        // Map fields using result_index
        result->bojong = iter2[result_index->bojong];
        result->nn = iter2[result_index->nn];
        result->mm = iter2[result_index->mm];
        result->x = iter2[result_index->x];
        result->AMT = iter2[result_index->AMT];

        // Map GP_Input
        if (!result_index->GP_Input.empty() && result_index->GP_Input[0].size() >= 3) {
          int r = result_index->GP_Input[0][0];
          int c = result_index->GP_Input[0][1];
          int val_idx = result_index->GP_Input[0][2];

          if (r >= 0 && r < 10 && c >= 0 && c < 10 && val_idx >= 0 && val_idx < (int)iter2.size()) {
            result->GP_Input[r][c] = iter2[val_idx];
          }
        }

        // Map dnum from CodeData
        auto code_it = code_map.find(result->bojong);
        if (code_it != code_map.end()) {
          result->dnum = code_it->second->dnum;
        } else {
          result->dnum = 0;
        }

        insurance_result.emplace_back(result);
      }
    }

  } catch (const std::bad_any_cast& e) {
    Logger::Log(L"Error: Bad any_cast in ConstructDataStructure: %ls. Check data types.\n", Ctw(e.what()).c_str());
  } catch (const std::exception& e) {
    Logger::Log(L"Error in ConstructDataStructure: %ls\n", Ctw(e.what()).c_str());
  }
}

void InsuranceResultDataStructure::PrintDataStructure(const std::any& context) const {
  try {
    const auto& insurance_result = std::any_cast<const InsuranceResultList&>(context);
    for (const auto& result : insurance_result) {
      Logger::Log(
          L"InsuranceResult: bojong: %d, dnum: %d, nn: %d, mm: %d, x: %d, AMT: %d\n",
          result->bojong, result->dnum, result->nn, result->mm, result->x, result->AMT);
      for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
          if (result->GP_Input[i][j] == 0) {
            continue;
          }
          Logger::Log(L"GP_Input[%d][%d]: %d\n", i, j, result->GP_Input[i][j]);
        }
      }
    }
  } catch (const std::exception& e) {
    Logger::Log(L"Error in PrintDataStructure: %ls\n", Ctw(e.what()).c_str());
  }
}
