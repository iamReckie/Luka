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
#ifndef SRC_DATAPROCESSOR_DATA_HELPER_H_
#define SRC_DATAPROCESSOR_DATA_HELPER_H_
#include <any>
#include <memory>
#include <shared_mutex>
#include <unordered_map>
#include <vector>

#include "DataProcessor/code_data_structure.h"
#include "DataProcessor/data_processor.h"
#include "DataProcessor/expense_data_structure.h"
#include "DataProcessor/insurance_result_data_structure.h"
#include "DataProcessor/qx_data_structure.h"
#include "DataProcessor/reserve_result_data_structure.h"
#include "DataProcessor/sratio_data_structure.h"
#include "DataProcessor/tbl_data_structure.h"
#include "DataProcessor/termination_data_structure.h"
#include "Logger/logger.h"

class DataHelper : public std::enable_shared_from_this<DataHelper> {
 public:
  DataHelper() = default;
  ~DataHelper() = default;

  void ExecuteData(const std::wstring &name, std::wstring &key, const std::wstring type, const std::vector<std::any> &args) {
    Logger::Log(L"Executing Data Structure: %ls of type: %ls\n", name.c_str(), type.c_str());
    std::shared_ptr<IDataStructure> processor = nullptr;

    // Double-checked locking pattern with shared_mutex for read-heavy workload
    {
      std::shared_lock<std::shared_mutex> read_lock(data_processors_mutex_);
      auto it = data_processors_.find(name);
      if (it != data_processors_.end()) {
        processor = it->second;
      }
    }

    if (!processor) {
      // Need to create new structure - use exclusive lock
      std::unique_lock<std::shared_mutex> write_lock(data_processors_mutex_);
      // Double-check after acquiring write lock
      auto it = data_processors_.find(name);
      if (it == data_processors_.end()) {
        processor = CreateDataStructure(type);
        if (processor) {
          data_processors_[name] = processor;
        }
      } else {
        processor = it->second;
      }
    }

    if (processor) {
      std::any* context_ptr = nullptr;
      {
        std::unique_lock<std::shared_mutex> lock(data_contexts_mutex_);
        auto it = data_contexts_.find(name);
        if (it == data_contexts_.end()) {
          data_contexts_[name] = processor->CreateContext();
        }
        context_ptr = &data_contexts_[name];
      }
      if (context_ptr) {
        processor->ConstructDataStructure(*context_ptr, args, key);
      }
    } else {
      Logger::Log(L"Error: Failed to create or find data structure: %ls\n", name.c_str());
    }
  }

  void PrintData(const std::wstring &name) {
    std::shared_ptr<IDataStructure> processor = nullptr;
    std::any* context_ptr = nullptr;
    
    {
      std::shared_lock<std::shared_mutex> lock(data_processors_mutex_);
      auto it = data_processors_.find(name);
      if (it != data_processors_.end()) {
        processor = it->second;
      }
    }

    if (processor) {
      std::shared_lock<std::shared_mutex> lock(data_contexts_mutex_);
      auto it = data_contexts_.find(name);
      if (it != data_contexts_.end()) {
        // We need to cast away constness of the pointer to match the map iterator, 
        // but the content is const for PrintDataStructure
        context_ptr = const_cast<std::any*>(&it->second);
        processor->PrintDataStructure(*context_ptr);
      }
    }
  }

  std::shared_ptr<IDataStructure> GetDataStructure(const std::wstring &name) {
    std::shared_lock<std::shared_mutex> lock(data_processors_mutex_);
    auto it = data_processors_.find(name);
    if (it != data_processors_.end()) {
      return it->second;
    }
    return nullptr;
  }

  std::any* GetDataContext(const std::wstring &name) {
    std::shared_lock<std::shared_mutex> lock(data_contexts_mutex_);
    auto it = data_contexts_.find(name);
    if (it != data_contexts_.end()) {
      return const_cast<std::any*>(&it->second);
    }
    return nullptr;
  }

 private:
  std::unordered_map<std::wstring, std::shared_ptr<IDataStructure>>
      data_processors_;
  std::unordered_map<std::wstring, std::any> data_contexts_;
  std::unordered_map<std::wstring, std::shared_ptr<IDataStructure>> type_cache_;
  mutable std::shared_mutex data_processors_mutex_;  // Reader-writer lock
  mutable std::shared_mutex data_contexts_mutex_;    // Reader-writer lock
  mutable std::shared_mutex type_cache_mutex_;       // Reader-writer lock

  std::shared_ptr<IDataStructure> CreateDataStructure(const std::wstring &type) {
    // Try read first (fast path)
    {
      std::shared_lock<std::shared_mutex> read_lock(type_cache_mutex_);
      auto type_cache_it = type_cache_.find(type);
      if (type_cache_it != type_cache_.end()) {
        return type_cache_it->second;
      }
    }

    // Create if not cached (slow path, requires write lock)
    auto self = shared_from_this();
    std::shared_ptr<IDataStructure> ds_instance = nullptr;
    if (type == L"Code") {
      ds_instance = std::make_shared<CodeDataStructure>(self);
    } else if (type == L"Qx") {
      ds_instance = std::make_shared<QxDataStructure>(self);
    } else if (type == L"Termination") {
      ds_instance = std::make_shared<TerminationDataStructure>(self);
    } else if (type == L"Expense") {
      ds_instance = std::make_shared<ExpenseDataStructure>(self);
    } else if (type == L"SRatio") {
      ds_instance = std::make_shared<SRatioDataStructure>(self);
    } else if (type == L"Table") {
      ds_instance = std::make_shared<TableDataStructure>(self);
    } else if (type == L"InsuranceResult") {
      ds_instance = std::make_shared<InsuranceResultDataStructure>(self);
    } else if (type == L"ReserveResult") {
      ds_instance = std::make_shared<ReserveResultDataStructure>(self);
    }

    // Cache with write lock if created
    if (ds_instance) {
      std::unique_lock<std::shared_mutex> write_lock(type_cache_mutex_);
      type_cache_[type] = ds_instance;
    }
    return ds_instance;
  }
};
#endif  // SRC_DATAPROCESSOR_DATA_HELPER_H_
