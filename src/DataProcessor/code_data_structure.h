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
struct CodeDataContext {
  std::unordered_map<int, std::shared_ptr<CodeTable>> code_table;
  int current_index_of_qx_table = 0;
  std::unordered_map<int, std::wstring> table_for_qx_table;
};

class CodeDataStructure : public IDataStructure {
 public:
  explicit CodeDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) override;
  void MergeDataStructure(std::any& target, const std::any& source) override;
  void PrintDataStructure(const std::any& context) const override;
  std::any CreateContext() const override { return CodeDataContext(); }
};

#endif  // SRC_DATAPROCESSOR_CODE_DATA_STRUCTURE_H_
