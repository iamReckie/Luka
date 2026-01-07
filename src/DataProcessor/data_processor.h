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
#ifndef SRC_DATAPROCESSOR_DATA_PROCESSOR_H_
#define SRC_DATAPROCESSOR_DATA_PROCESSOR_H_
#include <any>
#include <memory>
#include <stdexcept>
#include <vector>

class DataHelper;
class IDataStructure {
 public:
  explicit IDataStructure(std::weak_ptr<DataHelper> data_helper)
      : data_helper_(data_helper) {}
  virtual void ConstructDataStructure(std::any& context,
                                      const std::vector<std::any>& args,
                                      std::wstring& key) = 0;
  virtual void MergeDataStructure(std::any& /*target*/, const std::any& /*source*/) {}
  virtual void PrintDataStructure(const std::any& context) const = 0;
  virtual std::any CreateContext() const = 0;
  virtual ~IDataStructure() = default;

 protected:
  // Safe accessor for data_helper with null check
  std::shared_ptr<DataHelper> GetDataHelper() const {
    auto helper = data_helper_.lock();
    if (!helper) {
      throw std::runtime_error("DataHelper has been destroyed");
    }
    return helper;
  }

  std::weak_ptr<DataHelper> data_helper_;
};
class DataProcessor {
 public:
  explicit DataProcessor(std::shared_ptr<IDataStructure> data_structure)
      : data_structure_(data_structure) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) {
    if (data_structure_) {
      data_structure_->ConstructDataStructure(context, args, key);
    }
  }
  void PrintDataStructure(const std::any& context) {
    if (data_structure_) {
      data_structure_->PrintDataStructure(context);
    }
  }
  virtual ~DataProcessor() = default;

 private:
  std::shared_ptr<IDataStructure> data_structure_;
};
#endif  // SRC_DATAPROCESSOR_DATA_PROCESSOR_H_
