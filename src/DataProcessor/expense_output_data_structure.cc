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
#include "DataProcessor/expense_output_data_structure.h"

#include <any>
#include <string>
#include <vector>

#include "DataProcessor/data_helper.h"
#include "DataProcessor/expense_data_structure.h"
#include "Logger/logger.h"
#include "Utility/abort.h"
#include "Utility/string_utils.h"
void ExpenseOutputDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  (void)key;   // Unused parameter
  (void)args;  // Unused parameter
  auto& expense_output_context = std::any_cast<ExpenseOutputContext&>(context);
  std::shared_ptr<ExpenseOutput> output_ptr = std::make_shared<ExpenseOutput>();
  try {
    std::shared_ptr<DataHelper> data_helper = GetDataHelper();
    const auto& expense_data_context_ptr = data_helper->GetDataContext(L"Expense");
    if (!expense_data_context_ptr) {
      Abort(L"Failed to get ExpenseData context\n");
    }
    const auto& expense_data_map = std::any_cast<const ExpenseDataStructure::ExpenseTableMap&>(*expense_data_context_ptr);
    for (const auto& [key_int, expense_tables] : expense_data_map) {
      for (const auto& expense_table : expense_tables) {
        output_ptr->alp_in[key_int][expense_table->mm] = expense_table->ap;
        output_ptr->beta1_in[key_int][expense_table->mm] = expense_table->bp;
        output_ptr->beta2_in[key_int][expense_table->mm] = expense_table->bs;
        output_ptr->beta3_in[key_int][expense_table->mm] = expense_table->b2;
        output_ptr->gamma_in[key_int][expense_table->mm] = expense_table->bo;
      }
    }
    expense_output_context.output = output_ptr;
    GetDataHelper()->PrintData(L"ExpenseOutput");
  } catch (const std::exception& e) {
    Logger::Log(L"Error in ExpenseOutputDataStructure::ConstructDataStructure: %ls\n", Ctw(e.what()).c_str());
  }
}

void ExpenseOutputDataStructure::MergeDataStructure(std::any& target, const std::any& source) {
  auto& target_ctx = std::any_cast<ExpenseOutputContext&>(target);
  const auto& source_ctx = std::any_cast<const ExpenseOutputContext&>(source);

  if (!source_ctx.output) {
    return;
  }

  target_ctx.output = source_ctx.output;
}

void ExpenseOutputDataStructure::PrintDataStructure(const std::any& context) const {
  try {
    const auto& expense_output_context = std::any_cast<const ExpenseOutputContext&>(context);
    if (!expense_output_context.output) {
      Logger::Log(L"ExpenseOutputDataStructure: Context is empty.\n");
      return;
    }

    const auto& output = expense_output_context.output;
    Logger::Log(L"=== Expense Output Data Structure ===\n");

    for (int i = 0; i < 10; ++i) {
      for (int j = 0; j < 30; ++j) {
        if (output->alp_in[i][j] != 0.0 || output->beta1_in[i][j] != 0.0 ||
            output->beta2_in[i][j] != 0.0 || output->beta3_in[i][j] != 0.0 ||
            output->gamma_in[i][j] != 0.0) {
          Logger::Log(
              L"Index [%d][%d]: alp_in=%.6f, beta1_in=%.6f, beta2_in=%.6f, "
              L"beta3_in=%.6f, gamma_in=%.6f\n",
              i, j, output->alp_in[i][j], output->beta1_in[i][j],
              output->beta2_in[i][j], output->beta3_in[i][j],
              output->gamma_in[i][j]);
        }
      }
    }
    Logger::Log(L"=====================================\n");

  } catch (const std::bad_any_cast& e) {
    Logger::Log(
        L"Error in ExpenseOutputDataStructure::PrintDataStructure: Bad any cast "
        L"- %ls\n",
        Ctw(e.what()).c_str());
  } catch (const std::exception& e) {
    Logger::Log(L"Error in ExpenseOutputDataStructure::PrintDataStructure: %ls\n",
                Ctw(e.what()).c_str());
  }
}
