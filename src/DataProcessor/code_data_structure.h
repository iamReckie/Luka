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
struct SubCodeTable {
  double pay;
  double fst;
  double snd;
};
struct CodeTable {
  int dnum;
  std::wstring name;
  int qx_ku;
  int mhj;
  int re;
  int M_count;
  std::unordered_map<std::wstring, std::shared_ptr<SubCodeTable>> sub_code_table;
  // qx_in[C1][gender][age]: C1(0-4), gender(0=Male,1=Female), age(0-119)
  double qx_in[5][2][120] = {{{0.0}}};
};
struct CodeDataContext {
  std::unordered_map<int, std::shared_ptr<CodeTable>> code_table;
  int current_index_of_qx_table = 0;
  std::unordered_map<int, std::wstring> table_for_qx_table;
  std::unordered_map<int, std::wstring> qx_key_map;  // C1 index -> qx_key
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
