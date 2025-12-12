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
#ifndef SRC_DATAPROCESSOR_CODE_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_CODE_DATA_STRUCTURE_H_
#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataProcessor/data_processor.h"
struct CodeTable {
  int dnum;
  std::wstring name;
  int qx_ku;
  int mhj;
  int re;
  int M_count;
  std::unordered_map<std::wstring, std::vector<float>> qx_table_;
};
class CodeDataStructure : public IDataStructure {
 public:
  explicit CodeDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper), current_index_of_qx_table_(0) {}
  void ConstructDataStructure(const std::vector<std::any>& args,
                              std::wstring& key) override;
  void PrintDataStructure() const override;
  std::unordered_map<int, std::shared_ptr<CodeTable>> GetDataStructure() const {
    return code_table_;
  }

 private:
  std::unordered_map<int, std::shared_ptr<CodeTable>> code_table_;
  int current_index_of_qx_table_;
  std::unordered_map<int, std::wstring> table_for_qx_table_;
};

#endif  // SRC_DATAPROCESSOR_CODE_DATA_STRUCTURE_H_
