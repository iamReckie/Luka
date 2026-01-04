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
#ifndef SRC_DATAPROCESSOR_EXPENSE_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_EXPENSE_DATA_STRUCTURE_H_
#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

#include "DataProcessor/data_processor.h"
struct ExpenseTable {
  int mm;
  double ap;
  double bp;
  double bs;
  double b2;
  double bo;
};
class ExpenseDataStructure : public IDataStructure {
 public:
  using ExpenseTableMap = std::unordered_map<int, std::vector<std::shared_ptr<ExpenseTable>>>;

  explicit ExpenseDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) override;
  void MergeDataStructure(std::any& target, const std::any& source) override;
  void PrintDataStructure(const std::any& context) const override;
  std::any CreateContext() const override { return ExpenseTableMap(); }
};

#endif  // SRC_DATAPROCESSOR_EXPENSE_DATA_STRUCTURE_H_
