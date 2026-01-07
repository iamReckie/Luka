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
#ifndef SRC_DATAPROCESSOR_TERMINATION_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_TERMINATION_DATA_STRUCTURE_H_
#include <any>
#include <memory>
#include <unordered_map>
#include <vector>

#include "DataProcessor/data_processor.h"
struct TerminationTable {
  double ten;
  double eleven;
  double twelve;
  double thirteen;
  double fourteen;
  double fifteen;
  double sixteen;
  double seventeen;
  double eighteen;
  double nineteen;
  double twenty;
  double twenty_one;
  double twenty_two;
  double twenty_three;
  double twenty_four;
  double twenty_five;
  double twenty_six;
  double twenty_seven;
  double twenty_eight;
  double twenty_nine;
  double thirty;
};
class TerminationDataStructure : public IDataStructure {
 public:
  using TerminationTableMap =
      std::unordered_map<int, std::shared_ptr<TerminationTable>>;

  explicit TerminationDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) override;
  void MergeDataStructure(std::any& target, const std::any& source) override;
  void PrintDataStructure(const std::any& context) const override;
  std::any CreateContext() const override { return TerminationTableMap(); }
};

#endif  // SRC_DATAPROCESSOR_TERMINATION_DATA_STRUCTURE_H_
