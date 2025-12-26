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
#include "DataProcessor/tbl_data_structure.h"
#include "Logger/logger.h"
#include "Utility/string_utils.h"
void ReserveResultDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  auto& reserve_result_data_structure =
      std::any_cast<ReserveResultList&>(context);
  try {
    ReserveResultIndex reserve_result_index = std::any_cast<ReserveResultIndex>(args[0]);
    std::shared_ptr<DataHelper> data_helper = GetDataHelper();

    // 1. Get Table Data Context
    auto* table_context_ptr = data_helper->GetDataContext(key);
    if (!table_context_ptr) {
      Logger::Log(L"Error: Failed to get TableData context\n");
      return;
    }
    const auto& table_data_map = std::any_cast<const TableDataStructure::TableDataMap&>(*table_context_ptr);

    // 2. Get Insurance Result Data Context
    auto* insurance_context_ptr =
        data_helper->GetDataContext(L"InsuranceResult");
    if (!insurance_context_ptr) {
      Logger::Log(L"Error: Failed to get InsuranceResult context\n");
      return;
    }
    const auto& insurance_results = std::any_cast<const InsuranceResultDataStructure::InsuranceResultList&>(*insurance_context_ptr);

    // 3. Get the specific table data for the key
    auto table_it = table_data_map.find(key);
    if (table_it == table_data_map.end()) {
      Logger::Log(L"Error: Table data not found for key: %ls\n", key.c_str());
      return;
    }
    const auto& varTemp1 = table_it->second;  // vector<vector<int>>

    // Helper to parse index from string safely
    auto parseIndex = [](const std::wstring& s) -> int {
      try {
        return std::stoi(s);
      } catch (...) {
        return -1;
      }
    };

    // Helper to get value from row safely
    auto getValue = [&](const std::vector<int>& row, int idx) -> double {
      if (idx >= 0 && idx < static_cast<int>(row.size())) {
        return static_cast<double>(row[idx]);
      }
      return 0.0;
    };

    // Extract indices from ReserveResultIndex
    // Assuming structure: [row][0] = loop index, [row][1] = end index (if applicable)
    int idx_tVn_1_loop = -1, idx_tVn_1_end = -1;
    int idx_tVn_2_loop = -1, idx_tVn_2_end = -1;
    int idx_NP_1 = -1, idx_NP_2 = -1;
    int idx_Alpha_1 = -1, idx_Alpha_2 = -1;

    if (reserve_result_index.tVn_Input.size() > 0 && reserve_result_index.tVn_Input[0].size() > 0) {
      idx_tVn_1_loop = parseIndex(reserve_result_index.tVn_Input[0][0]);
    }
    if (reserve_result_index.tVn_Input.size() > 0 && reserve_result_index.tVn_Input[0].size() > 1) {
      idx_tVn_1_end = parseIndex(reserve_result_index.tVn_Input[0][1]);
    }

    if (reserve_result_index.tVn_Input.size() > 1 && reserve_result_index.tVn_Input[1].size() > 0) {
      idx_tVn_2_loop = parseIndex(reserve_result_index.tVn_Input[1][0]);
    }
    if (reserve_result_index.tVn_Input.size() > 1 && reserve_result_index.tVn_Input[1].size() > 1) {
      idx_tVn_2_end = parseIndex(reserve_result_index.tVn_Input[1][1]);
    }

    if (reserve_result_index.NP_beta_Input.size() > 0 && reserve_result_index.NP_beta_Input[0].size() > 0) {
      idx_NP_1 = parseIndex(reserve_result_index.NP_beta_Input[0][0]);
    }
    if (reserve_result_index.NP_beta_Input.size() > 1 && reserve_result_index.NP_beta_Input[1].size() > 0) {
      idx_NP_2 = parseIndex(reserve_result_index.NP_beta_Input[1][0]);
    }

    if (reserve_result_index.Alpha_ALD_Input.size() > 0 && reserve_result_index.Alpha_ALD_Input[0].size() > 0) {
      idx_Alpha_1 = parseIndex(reserve_result_index.Alpha_ALD_Input[0][0]);
    }
    if (reserve_result_index.Alpha_ALD_Input.size() > 1 && reserve_result_index.Alpha_ALD_Input[1].size() > 0) {
      idx_Alpha_2 = parseIndex(reserve_result_index.Alpha_ALD_Input[1][0]);
    }

    // Fallback to hardcoded if parsing failed (or for safety during dev)
    // Based on user prompt: 15, 19, 17, 18, 21, 22, 16, 20
    if (idx_tVn_1_loop == -1) {
      idx_tVn_1_loop = 15;
    }
    if (idx_tVn_2_loop == -1) {
      idx_tVn_2_loop = 19;
    }
    if (idx_NP_1 == -1) {
      idx_NP_1 = 17;
    }
    if (idx_Alpha_1 == -1) {
      idx_Alpha_1 = 18;
    }
    if (idx_NP_2 == -1) {
      idx_NP_2 = 21;
    }
    if (idx_Alpha_2 == -1) {
      idx_Alpha_2 = 22;
    }
    if (idx_tVn_1_end == -1) {
      idx_tVn_1_end = 16;
    }
    if (idx_tVn_2_end == -1) {
      idx_tVn_2_end = 20;
    }

    for (const auto& insurance_result : insurance_results) {
      ReserveResult reserve_result{};
      int nn = insurance_result->nn;

      // Initialize vectors
      reserve_result.tVn_Input.resize(2);
      reserve_result.tVn_Input[0].resize(nn + 1);  // +1 for the end value
      reserve_result.tVn_Input[1].resize(nn + 1);

      reserve_result.NP_beta_Input.resize(2);
      reserve_result.Alpha_ALD_Input.resize(2);

      // Loop kk = 0 to nn - 1
      for (int kk = 0; kk < nn; ++kk) {
        if (kk >= static_cast<int>(varTemp1.size())) {
          break;
        }

        const auto& row = varTemp1[kk];

        reserve_result.tVn_Input[0][kk] = getValue(row, idx_tVn_1_loop);
        reserve_result.tVn_Input[1][kk] = getValue(row, idx_tVn_2_loop);

        if (kk == 0) {
          reserve_result.NP_beta_Input[0] = getValue(row, idx_NP_1);
          reserve_result.Alpha_ALD_Input[0] = getValue(row, idx_Alpha_1);
          reserve_result.NP_beta_Input[1] = getValue(row, idx_NP_2);
          reserve_result.Alpha_ALD_Input[1] = getValue(row, idx_Alpha_2);
        }
      }

      // Handle the end value (kk = nn case in VBA logic, or just the last assignment)
      // tVn_Input(1, nn) = varTemp1(16)
      // We need to access the row corresponding to nn? Or is it the same row?
      // The VBA loop was `For kk = 0 To nn - 1`.
      // The assignment `tVn_Input(1, nn) = varTemp1(16)` is likely OUTSIDE the loop or for the last element.
      // If it's outside the loop, which row of varTemp1 do we use?
      // If varTemp1 has size >= nn, maybe row nn?
      // Or maybe it uses the last processed row?
      // User said: "If kk = nn - 1 Then ... tVn_Input(1, nn) = varTemp1(16)"
      // Wait, if it's inside the loop "If kk = nn - 1", then it uses the row `nn-1`.
      // Let's re-read carefully: "For kk = 0 To nn - 1 ... tVn_Input(1, kk) = varTemp1(15) ... If kk = nn - 1 Then tVn_Input(1, nn) = varTemp1(16)"
      // So it uses the row `nn-1` (the last iteration of the loop) to populate `tVn_Input(1, nn)`.

      if (nn > 0 && nn - 1 < static_cast<int>(varTemp1.size())) {
        const auto& last_row = varTemp1[nn - 1];
        reserve_result.tVn_Input[0][nn] = getValue(last_row, idx_tVn_1_end);
        reserve_result.tVn_Input[1][nn] = getValue(last_row, idx_tVn_2_end);
      }

      reserve_result_data_structure.emplace_back(reserve_result);
    }

    Logger::Log(L"Constructing ReserveResultDataStructure with key: %ls, size: %d\n",
                key.c_str(), static_cast<int>(reserve_result_data_structure.size()));
  } catch (const std::exception& e) {
    Logger::Log(L"Error in ReserveResultDataStructure::ConstructDataStructure: %ls\n", Ctw(e.what()).c_str());
  }
}
void ReserveResultDataStructure::PrintDataStructure(const std::any& context) const {
  (void)context;
  Logger::Log(L"ReserveResultDataStructure contents:\n");
}
