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
#ifndef SRC_DATAPROCESSOR_EXPENSE_OUTPUT_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_EXPENSE_OUTPUT_DATA_STRUCTURE_H_
#include <any>
#include <memory>
#include <string>
#include <vector>

#include "DataProcessor/data_processor.h"
struct ExpenseOutput {
  double alp_in[10][30];
  double beta1_in[10][30];
  double beta2_in[10][30];
  double beta3_in[10][30];
  double gamma_in[10][30];
};
struct ExpenseOutputContext {
  std::shared_ptr<ExpenseOutput> output;
};

class ExpenseOutputDataStructure : public IDataStructure {
 public:
  explicit ExpenseOutputDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) override;
  void MergeDataStructure(std::any& target, const std::any& source) override;
  void PrintDataStructure(const std::any& context) const override;
  std::any CreateContext() const override { return ExpenseOutputContext(); }
};
#endif  // SRC_DATAPROCESSOR_EXPENSE_OUTPUT_DATA_STRUCTURE_H_
