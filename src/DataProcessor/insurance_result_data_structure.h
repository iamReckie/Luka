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
#ifndef SRC_DATAPROCESSOR_INSURANCE_RESULT_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_INSURANCE_RESULT_DATA_STRUCTURE_H_
#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataProcessor/data_processor.h"
struct InsuranceResult {
  int bojong;
  int nn;
  int mm;
  int x;
  int AMT;
  int dnum;
  std::vector<std::vector<int>> GP_Input;
};
struct InsuranceResultIndex {
  int bojong;
  int nn;
  int mm;
  int x;
  int AMT;
  std::vector<std::vector<int>> GP_Input;
};
class InsuranceResultDataStructure : public IDataStructure {
 public:
  using InsuranceResultList = std::vector<std::shared_ptr<InsuranceResult>>;

  explicit InsuranceResultDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) override;
  void PrintDataStructure(const std::any& context) const override;
  std::any CreateContext() const override { return InsuranceResultList(); }
};
#endif  // SRC_DATAPROCESSOR_INSURANCE_RESULT_DATA_STRUCTURE_H_
