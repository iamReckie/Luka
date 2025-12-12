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
#ifndef SRC_DATAPROCESSOR_SRATIO_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_SRATIO_DATA_STRUCTURE_H_
#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataProcessor/data_processor.h"
struct SRatioTable {
  std::wstring name;
  double standard_price;
  int renewal;
  int sex;
  int age;
  int category;
  int real_category;
  int due;
  int real_due;
  double adjust;
  double regular;
  double sratio;
  double min_s;
  double apply_alpha;
  double standard_alpha;
  bool reverse;
};
class SRatioDataStructure : public IDataStructure {
 public:
  explicit SRatioDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(const std::vector<std::any>& args,
                              std::wstring& key) override;
  void PrintDataStructure() const override;

 private:
  std::unordered_map<int, std::vector<std::shared_ptr<SRatioTable>>>
      sratio_table_;
};

#endif  // SRC_DATAPROCESSOR_SRATIO_DATA_STRUCTURE_H_
