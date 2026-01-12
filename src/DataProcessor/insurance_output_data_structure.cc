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

#include <algorithm>
#include <any>
#include <string>
#include <vector>

#include "DataProcessor/code_data_structure.h"
#include "DataProcessor/data_helper.h"
#include "DataProcessor/expense_data_structure.h"
#include "DataProcessor/expense_output_data_structure.h"
#include "DataProcessor/insurance_result_data_structure.h"
#include "DataProcessor/tbl_data_structure.h"
#include "Logger/logger.h"
#include "Utility/abort.h"
#include "Utility/string_utils.h"
void InsuranceOutputDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  auto& insurance_output_context = std::any_cast<InsuranceOutputContext&>(context);
  try {
    InsuranceOutputIndex insurance_output_index = std::any_cast<InsuranceOutputIndex>(args[0]);
    std::shared_ptr<DataHelper> data_helper = GetDataHelper();

    // 1. Get Table Data Context
    auto* table_context_ptr = data_helper->GetDataContext(key);
    if (!table_context_ptr) {
      Abort(L"Failed to get TableData context\n");
    }
    const auto& table_data_map = std::any_cast<const TableDataStructure::TableDataMap&>(*table_context_ptr);

    // 2. Get Insurance Result Data Context
    auto* insurance_context_ptr = data_helper->GetDataContext(L"InsuranceResult");
    if (!insurance_context_ptr) {
      Abort(L"Failed to get InsuranceResult context\n");
    }
    const auto& insurance_results = std::any_cast<const InsuranceResultDataStructure::InsuranceResultList&>(*insurance_context_ptr);

    // Get Expense Output Data Context
    std::shared_ptr<ExpenseOutput> expense_output = nullptr;
    auto* expense_output_context_ptr = data_helper->GetDataContext(L"ExpenseOutput");
    if (expense_output_context_ptr) {
      const auto& expense_output_ctx = std::any_cast<const ExpenseOutputContext&>(*expense_output_context_ptr);
      expense_output = expense_output_ctx.output;
    } else {
      Logger::Log(L"Warning: Failed to get ExpenseOutput context\n");
    }

    // 3. Get the specific table data for the key
    auto table_it = table_data_map.find(key);
    if (table_it == table_data_map.end()) {
      Abort(L"Table data not found for key: %ls\n", key.c_str());
    }

    auto expense_context_ptr = data_helper->GetDataContext(L"Expense");
    if (!expense_context_ptr) {
      Abort(L"Failed to get Expense context\n");
    }
    const auto& expense_table_map = std::any_cast<const ExpenseDataStructure::ExpenseTableMap&>(*expense_context_ptr);

    auto code_context_ptr = data_helper->GetDataContext(L"Code");
    if (!code_context_ptr) {
      Abort(L"Failed to get Code context\n");
    }
    const auto& code_context = std::any_cast<const CodeDataContext&>(*code_context_ptr);
    const auto& code_map = code_context.code_table;

    // Helper lambda to parse index from string safely
    auto parse_index = [](const std::wstring& s) -> int {
      try {
        return std::stoi(s);
      } catch (...) {
        return -1;
      }
    };

    // Helper lambda to get value from row safely
    auto get_value = [&](const std::vector<int>& row, int idx) -> double {
      if (idx >= 0 && idx < static_cast<int>(row.size())) {
        return static_cast<double>(row[idx]);
      }
      return 0.0;
    };

    // Helper lambda to get table row from a given table name and row index
    auto get_table_row = [&](const std::wstring& table_name, int row_idx) -> const std::vector<int>* {
      auto* ctx_ptr = data_helper->GetDataContext(table_name);
      if (!ctx_ptr) {
        Abort(L"Failed to get TableData context for %ls\n", table_name.c_str());
      }
      const auto& map = std::any_cast<const TableDataStructure::TableDataMap&>(*ctx_ptr);
      auto it = map.find(table_name);
      if (it == map.end()) {
        Abort(L"Table data not found for key: %ls\n", table_name.c_str());
      }
      if (row_idx < 0 || row_idx >= static_cast<int>(it->second.size())) {
        return nullptr;
      }
      return &(it->second[row_idx]);
    };

    // Process each insurance_result separately
    for (const auto& insurance_result : insurance_results) {
      // Create a new output_ptr for each insurance_result
      std::shared_ptr<InsuranceOutput> output_ptr = std::make_shared<InsuranceOutput>();
      output_ptr->tVn_Input.resize(2);

      // Extract indices from InsuranceOutputIndex
      int idx_tVn_1_loop = -1, idx_tVn_1_end = -1;
      int idx_tVn_2_loop = -1, idx_tVn_2_end = -1;
      int idx_NP_1 = -1, idx_NP_2 = -1;
      int idx_Alpha_1 = -1, idx_Alpha_2 = -1;
      int idx_STD_NP_1 = -1, idx_STD_NP_2 = -1;
      int nn = insurance_result->nn, mm = insurance_result->mm, bojong = insurance_result->bojong;

      // Get dnum from code_map using bojong
      int dnum = 0;
      auto code_it = code_map.find(bojong);
      dnum = code_it->second->dnum;

      if (insurance_output_index.tVn_Input.second.size() > 0 && !insurance_output_index.tVn_Input.second[0].empty()) {
        idx_tVn_1_loop = parse_index(insurance_output_index.tVn_Input.second[0].back());
        idx_tVn_1_end = idx_tVn_1_loop + 1;
      }

      if (insurance_output_index.tVn_Input.second.size() > 1 && !insurance_output_index.tVn_Input.second[1].empty()) {
        idx_tVn_2_loop = parse_index(insurance_output_index.tVn_Input.second[1].back());
        idx_tVn_2_end = idx_tVn_2_loop + 1;
      }

      if (insurance_output_index.NP_beta_Input.second.size() > 0 && !insurance_output_index.NP_beta_Input.second[0].empty()) {
        idx_NP_1 = parse_index(insurance_output_index.NP_beta_Input.second[0].back());
      }
      if (insurance_output_index.NP_beta_Input.second.size() > 1 && !insurance_output_index.NP_beta_Input.second[1].empty()) {
        idx_NP_2 = parse_index(insurance_output_index.NP_beta_Input.second[1].back());
      }

      if (insurance_output_index.Alpha_ALD_Input.second.size() > 0 && !insurance_output_index.Alpha_ALD_Input.second[0].empty()) {
        idx_Alpha_1 = parse_index(insurance_output_index.Alpha_ALD_Input.second[0].back());
      }
      if (insurance_output_index.Alpha_ALD_Input.second.size() > 1 && !insurance_output_index.Alpha_ALD_Input.second[1].empty()) {
        idx_Alpha_2 = parse_index(insurance_output_index.Alpha_ALD_Input.second[1].back());
      }

      if (insurance_output_index.STD_NP_Input.second.size() > 0 && !insurance_output_index.STD_NP_Input.second[0].empty()) {
        idx_STD_NP_1 = parse_index(insurance_output_index.STD_NP_Input.second[0].back());
      }
      if (insurance_output_index.STD_NP_Input.second.size() > 1 && !insurance_output_index.STD_NP_Input.second[1].empty()) {
        idx_STD_NP_2 = parse_index(insurance_output_index.STD_NP_Input.second[1].back());
      }

      // Loop kk = 0 to nn - 1 for this specific insurance_result
      for (int kk = 0; kk < nn; ++kk) {
        // Get row from tVn_Input table
        const auto* row = get_table_row(insurance_output_index.tVn_Input.first, kk);
        if (!row) {
          continue;
        }

        if (idx_tVn_1_loop != -1) {
          output_ptr->tVn_Input[0].push_back(get_value(*row, idx_tVn_1_loop));
          output_ptr->tVn_Input[1].push_back(get_value(*row, idx_tVn_2_loop));
        }

        if (kk == 0) {
          if (idx_Alpha_1 != -1) {
            output_ptr->Alpha_ALD_Input.push_back(get_value(*row, idx_Alpha_1));
            output_ptr->Alpha_ALD_Input.push_back(get_value(*row, idx_Alpha_2));
          }
          if (idx_NP_1 != -1) {
            output_ptr->NP_beta_Input.push_back(get_value(*row, idx_NP_1));
            output_ptr->NP_beta_Input.push_back(get_value(*row, idx_NP_2));
          }
        }

        // Get row from STD_NP_Input table
        const auto* std_row = get_table_row(insurance_output_index.STD_NP_Input.first, kk);
        if (std_row && idx_STD_NP_1 != -1) {
          output_ptr->STD_NP_Input.push_back(get_value(*std_row, idx_STD_NP_1));
          output_ptr->STD_NP_Input.push_back(get_value(*std_row, idx_STD_NP_2));
        }

        if (kk == nn - 1 && idx_tVn_1_end != -1) {
          output_ptr->tVn_Input[0].push_back(get_value(*row, idx_tVn_1_end));
          output_ptr->tVn_Input[1].push_back(get_value(*row, idx_tVn_2_end));
        }
      }

      // Get alp and beta values from expense_table_map using dnum and mm
      auto expense_it = expense_table_map.find(dnum);
      if (expense_it != expense_table_map.end()) {
        const auto& expense_vector = expense_it->second;
        bool found = false;

        // Loop through expense_vector to find matching mm
        for (const auto& expense_item : expense_vector) {
          if (expense_item->mm == mm) {
            output_ptr->alp = expense_item->ap;
            output_ptr->beta1 = expense_item->bp;
            output_ptr->beta2 = expense_item->bs;
            output_ptr->beta3 = expense_item->b2;
            output_ptr->gamma = expense_item->bo;
            found = true;
            break;
          }
        }

        if (!found) {
          Abort(L"mm value %d not found in expense_vector for dnum %d\n", mm, dnum);
        }
      } else {
        Abort(L"dnum %d not found in expense_table_map\n", dnum);
      }

      output_ptr->am = std::min(nn, 20);

      for (int i = 0; i < 2; ++i) {
        int w = 0;
        i == 1 ? w = 110 : w = 100;
      }

      // Add this completed output_ptr to the context
      insurance_output_context.output.push_back(output_ptr);
    }

    Logger::Log(L"Constructing InsuranceOutputDataStructure with key: %ls\n", key.c_str());
  } catch (const std::exception& e) {
    Logger::Log(L"Error in InsuranceOutputDataStructure::ConstructDataStructure: %ls\n", Ctw(e.what()).c_str());
  }
}

void InsuranceOutputDataStructure::MergeDataStructure(std::any& target, const std::any& source) {
  auto& target_ctx = std::any_cast<InsuranceOutputContext&>(target);
  const auto& source_ctx = std::any_cast<const InsuranceOutputContext&>(source);

  if (source_ctx.output.empty()) {
    return;
  }

  target_ctx.output.insert(target_ctx.output.end(), source_ctx.output.begin(), source_ctx.output.end());
}

void InsuranceOutputDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& insurance_context = std::any_cast<const InsuranceOutputContext&>(context);
  if (insurance_context.output.empty()) {
    Logger::Log(L"InsuranceOutputDataStructure is empty.\n");
    return;
  }
  // const auto& output = insurance_context.output;

  Logger::Log(L"InsuranceOutputDataStructure contents:\n");

  for (const auto& iter : insurance_context.output) {
    Logger::Log(L"  alp: %.2f\n", iter->alp);
    Logger::Log(L"  beta1: %.2f\n", iter->beta1);
    Logger::Log(L"  beta2: %.2f\n", iter->beta2);
    Logger::Log(L"  beta3: %.2f\n", iter->beta3);
    Logger::Log(L"  gamma: %.2f\n", iter->gamma);
    Logger::Log(L"  am: %d\n", iter->am);

    Logger::Log(L"  tVn_Input (Row 0): ");
    for (double val : iter->tVn_Input[0]) {
      Logger::Log(L"%.2f ", val);
    }
    Logger::Log(L"\n");

    Logger::Log(L"  tVn_Input (Row 1): ");
    for (double val : iter->tVn_Input[1]) {
      Logger::Log(L"%.2f ", val);
    }
    Logger::Log(L"\n");

    Logger::Log(L"  Alpha_ALD_Input: ");
    for (double val : iter->Alpha_ALD_Input) {
      Logger::Log(L"%.2f ", val);
    }
    Logger::Log(L"\n");

    Logger::Log(L"  NP_beta_Input: ");
    for (double val : iter->NP_beta_Input) {
      Logger::Log(L"%.2f ", val);
    }
    Logger::Log(L"\n");

    Logger::Log(L"  STD_NP_Input: ");
    for (double val : iter->STD_NP_Input) {
      Logger::Log(L"%.2f ", val);
    }
    Logger::Log(L"\n");
  }
}
