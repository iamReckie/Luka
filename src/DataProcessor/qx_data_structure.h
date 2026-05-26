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
#ifndef SRC_DATAPROCESSOR_QX_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_QX_DATA_STRUCTURE_H_
#include <any>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataProcessor/data_processor.h"
struct QxTable {
  std::wstring qx_name;
  // qx_in[sex=0..1][age=0..119]  (internal 0-based, male=0 female=1)
  std::array<std::array<double, 120>, 2> qx_in{};
  int pending_age_ = -1;  // temp used during ConstructDataStructure
};
class QxDataStructure : public IDataStructure {
 public:
  using QxTableMap =
      std::unordered_map<std::wstring, std::shared_ptr<QxTable>>;

  explicit QxDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) override;
  void MergeDataStructure(std::any& target, const std::any& source) override;
  void PrintDataStructure(const std::any& context) const override;
  std::any CreateContext() const override { return QxTableMap(); }
};

#endif  // SRC_DATAPROCESSOR_QX_DATA_STRUCTURE_H_
