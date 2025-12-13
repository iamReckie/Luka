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
#ifndef SRC_DATAPROCESSOR_RESERVE_RESULT_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_RESERVE_RESULT_DATA_STRUCTURE_H_
#include <any>
#include <memory>
#include <string>
#include <vector>

#include "DataProcessor/data_processor.h"
struct ReserveResultIndex {
  std::vector<std::vector<std::wstring>> tVn_Input;
  std::vector<std::vector<std::wstring>> Alpha_ALD_Input;
  std::vector<std::vector<std::wstring>> NP_beta_Input;
  std::vector<std::vector<std::wstring>> STD_NP_Input;
};
struct ReserveResult {
  std::vector<std::vector<double>> tVn_Input;
  std::vector<double> Alpha_ALD_Input;
  std::vector<double> NP_beta_Input;
  std::vector<double> STD_NP_Input;
};

class ReserveResultDataStructure : public IDataStructure {
 public:
  explicit ReserveResultDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(const std::vector<std::any>& args,
                              std::wstring& key) override;
  void PrintDataStructure() const override;

 private:
  // std::unordered_map<std::wstring, std::shared_ptr<ReserveResult>>
  std::vector<ReserveResult> reserve_result_data_structure_;
};
#endif  // SRC_DATAPROCESSOR_RESERVE_RESULT_DATA_STRUCTURE_H_
