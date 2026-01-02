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
#include "DataProcessor/insurance_output_data_structure.h"

#include <any>
#include <string>
#include <vector>

#include "DataProcessor/data_helper.h"
#include "DataProcessor/insurance_result_data_structure.h"
#include "DataProcessor/tbl_data_structure.h"
#include "Logger/logger.h"
#include "Utility/string_utils.h"
void InsuranceOutputDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  auto& insurance_output_context = std::any_cast<InsuranceOutputContext&>(context);
  if (!insurance_output_context.output) {
    insurance_output_context.output = std::make_shared<InsuranceOutput>();
    insurance_output_context.output->tVn_Input.resize(2);
  }
  auto& output = insurance_output_context.output;
  try {
    InsuranceOutputIndex insurance_output_index = std::any_cast<InsuranceOutputIndex>(args[0]);
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
    int idx_STD_NP_1 = -1, idx_STD_NP_2 = -1;

    if (insurance_output_index.tVn_Input.size() > 0 && insurance_output_index.tVn_Input[0].size() > 0) {
      idx_tVn_1_loop = parseIndex(insurance_output_index.tVn_Input[0][0]);
    }
    if (insurance_output_index.tVn_Input.size() > 0 && insurance_output_index.tVn_Input[0].size() > 1) {
      idx_tVn_1_end = parseIndex(insurance_output_index.tVn_Input[0][1]);
    }

    if (insurance_output_index.tVn_Input.size() > 1 && insurance_output_index.tVn_Input[1].size() > 0) {
      idx_tVn_2_loop = parseIndex(insurance_output_index.tVn_Input[1][0]);
    }
    if (insurance_output_index.tVn_Input.size() > 1 && insurance_output_index.tVn_Input[1].size() > 1) {
      idx_tVn_2_end = parseIndex(insurance_output_index.tVn_Input[1][1]);
    }

    if (insurance_output_index.NP_beta_Input.size() > 0 && insurance_output_index.NP_beta_Input[0].size() > 0) {
      idx_NP_1 = parseIndex(insurance_output_index.NP_beta_Input[0][0]);
    }
    if (insurance_output_index.NP_beta_Input.size() > 1 && insurance_output_index.NP_beta_Input[1].size() > 0) {
      idx_NP_2 = parseIndex(insurance_output_index.NP_beta_Input[1][0]);
    }

    if (insurance_output_index.Alpha_ALD_Input.size() > 0 && insurance_output_index.Alpha_ALD_Input[0].size() > 0) {
      idx_Alpha_1 = parseIndex(insurance_output_index.Alpha_ALD_Input[0][0]);
    }
    if (insurance_output_index.Alpha_ALD_Input.size() > 1 && insurance_output_index.Alpha_ALD_Input[1].size() > 0) {
      idx_Alpha_2 = parseIndex(insurance_output_index.Alpha_ALD_Input[1][0]);
    }

    if (insurance_output_index.STD_NP_Input.size() > 0 && insurance_output_index.STD_NP_Input[0].size() > 0) {
      idx_STD_NP_1 = parseIndex(insurance_output_index.STD_NP_Input[0][0]);
    }
    if (insurance_output_index.STD_NP_Input.size() > 1 && insurance_output_index.STD_NP_Input[1].size() > 0) {
      idx_STD_NP_2 = parseIndex(insurance_output_index.STD_NP_Input[1][0]);
    }

    for (const auto& insurance_result : insurance_results) {
      int nn = insurance_result->nn;

      // Loop kk = 0 to nn - 1
      for (int kk = 0; kk < nn; ++kk) {
        if (kk >= static_cast<int>(varTemp1.size())) {
          break;
        }

        const auto& row = varTemp1[kk];

        if (idx_tVn_1_loop != -1) {
          output->tVn_Input[0].push_back(getValue(row, idx_tVn_1_loop));
          output->tVn_Input[1].push_back(getValue(row, idx_tVn_2_loop));
        }

        if (kk == 0) {
          if (idx_Alpha_1 != -1) {
            output->Alpha_ALD_Input.push_back(getValue(row, idx_Alpha_1));
            output->Alpha_ALD_Input.push_back(getValue(row, idx_Alpha_2));
          }
          if (idx_NP_1 != -1) {
            output->NP_beta_Input.push_back(getValue(row, idx_NP_1));
            output->NP_beta_Input.push_back(getValue(row, idx_NP_2));
          }
        }
        if (idx_STD_NP_1 != -1) {
          output->STD_NP_Input.push_back(getValue(row, idx_STD_NP_1));
          output->STD_NP_Input.push_back(getValue(row, idx_STD_NP_2));
        }
        continue;
      }

      if (nn > 0 && nn - 1 < static_cast<int>(varTemp1.size())) {
        const auto& last_row = varTemp1[nn - 1];
        output->tVn_Input[0].push_back(getValue(last_row, idx_tVn_1_end));
        output->tVn_Input[1].push_back(getValue(last_row, idx_tVn_2_end));
      }
    }

    Logger::Log(L"Constructing ReserveResultDataStructure with key: %ls\n", key.c_str());
  } catch (const std::exception& e) {
    Logger::Log(L"Error in ReserveResultDataStructure::ConstructDataStructure: %ls\n", Ctw(e.what()).c_str());
  }
}

void InsuranceOutputDataStructure::MergeDataStructure(std::any& target, const std::any& source) {
  auto& target_ctx = std::any_cast<InsuranceOutputContext&>(target);
  const auto& source_ctx = std::any_cast<const InsuranceOutputContext&>(source);

  if (!source_ctx.output) {
    return;
  }
  if (!target_ctx.output) {
    target_ctx.output = std::make_shared<InsuranceOutput>();
    target_ctx.output->tVn_Input.resize(2);
  }

  auto& t_res = target_ctx.output;
  const auto& s_res = source_ctx.output;

  if (s_res->tVn_Input.size() >= 2) {
    t_res->tVn_Input[0].insert(t_res->tVn_Input[0].end(), s_res->tVn_Input[0].begin(), s_res->tVn_Input[0].end());
    t_res->tVn_Input[1].insert(t_res->tVn_Input[1].end(), s_res->tVn_Input[1].begin(), s_res->tVn_Input[1].end());
  }

  t_res->Alpha_ALD_Input.insert(t_res->Alpha_ALD_Input.end(), s_res->Alpha_ALD_Input.begin(), s_res->Alpha_ALD_Input.end());
  t_res->NP_beta_Input.insert(t_res->NP_beta_Input.end(), s_res->NP_beta_Input.begin(), s_res->NP_beta_Input.end());
  t_res->STD_NP_Input.insert(t_res->STD_NP_Input.end(), s_res->STD_NP_Input.begin(), s_res->STD_NP_Input.end());
}

void InsuranceOutputDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& insurance_context = std::any_cast<const InsuranceOutputContext&>(context);
  if (!insurance_context.output) {
    Logger::Log(L"InsuranceOutputDataStructure is empty.\n");
    return;
  }
  const auto& output = insurance_context.output;

  Logger::Log(L"InsuranceOutputDataStructure contents:\n");

  Logger::Log(L"  tVn_Input (Row 0): ");
  for (double val : output->tVn_Input[0]) {
    Logger::Log(L"%.2f ", val);
  }
  Logger::Log(L"\n");

  Logger::Log(L"  tVn_Input (Row 1): ");
  for (double val : output->tVn_Input[1]) {
    Logger::Log(L"%.2f ", val);
  }
  Logger::Log(L"\n");

  Logger::Log(L"  Alpha_ALD_Input: ");
  for (double val : output->Alpha_ALD_Input) {
    Logger::Log(L"%.2f ", val);
  }
  Logger::Log(L"\n");

  Logger::Log(L"  NP_beta_Input: ");
  for (double val : output->NP_beta_Input) {
    Logger::Log(L"%.2f ", val);
  }
  Logger::Log(L"\n");

  Logger::Log(L"  STD_NP_Input: ");
  for (double val : output->STD_NP_Input) {
    Logger::Log(L"%.2f ", val);
  }
  Logger::Log(L"\n");
}
