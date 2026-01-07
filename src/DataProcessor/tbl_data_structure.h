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
#ifndef SRC_DATAPROCESSOR_TBL_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_TBL_DATA_STRUCTURE_H_
#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataProcessor/data_processor.h"
class TableDataStructure : public IDataStructure {
 public:
  using TableDataMap =
      std::unordered_map<std::wstring, std::vector<std::vector<int>>>;

  explicit TableDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) override;
  void MergeDataStructure(std::any& target, const std::any& source) override;
  void PrintDataStructure(const std::any& context) const override;
  std::any CreateContext() const override { return TableDataMap(); }
};

#endif  // SRC_DATAPROCESSOR_TBL_DATA_STRUCTURE_H_
