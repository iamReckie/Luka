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
#include <atomic>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataProcessor/code_data_structure.h"
#include "DataProcessor/data_processor.h"
#include "DataProcessor/expense_data_structure.h"
#include "DataProcessor/expense_output_data_structure.h"
#include "DataProcessor/insurance_output_data_structure.h"
#include "DataProcessor/insurance_result_data_structure.h"
#include "DataProcessor/qx_data_structure.h"
#include "DataProcessor/sratio_data_structure.h"
#include "DataProcessor/tbl_data_structure.h"
#include "DataProcessor/termination_data_structure.h"
#include "Logger/logger.h"

class DataHelper : public std::enable_shared_from_this<DataHelper> {
  struct Registry {
    std::unordered_map<std::wstring, std::shared_ptr<IDataStructure>> processors;
    std::unordered_map<std::wstring, std::any> contexts;
    std::unordered_map<std::wstring, std::shared_ptr<IDataStructure>> type_cache;
  };

 public:
  DataHelper() {
    std::atomic_store(&registry_, std::make_shared<Registry>());
  }
  ~DataHelper() = default;

  std::shared_ptr<IDataStructure> GetOrRegisterProcessor(const std::wstring &name, const std::wstring &type) {
    std::shared_ptr<Registry> current_registry;
    std::shared_ptr<IDataStructure> processor = nullptr;

    while (true) {
      current_registry = std::atomic_load(&registry_);
      auto proc_it = current_registry->processors.find(name);
      if (proc_it != current_registry->processors.end()) {
        return proc_it->second;
      }

      auto new_registry = std::make_shared<Registry>(*current_registry);
      processor = CreateDataStructure(type, *new_registry);
      if (processor) {
        new_registry->processors[name] = processor;
      }

      if (std::atomic_compare_exchange_strong(&registry_, &current_registry, new_registry)) {
        return processor;
      }
    }
  }

  void ExecuteData(const std::wstring &name, std::wstring &key, const std::wstring type, const std::vector<std::any> &args, std::any *specific_context = nullptr) {
    std::shared_ptr<Registry> current_registry;
    std::shared_ptr<IDataStructure> processor = nullptr;
    std::any *context_ptr = specific_context;

    // CAS Loop: Optimistic concurrency control
    while (true) {
      current_registry = std::atomic_load(&registry_);

      // 1. Check if we have everything we need in the current snapshot
      auto proc_it = current_registry->processors.find(name);
      bool has_processor = (proc_it != current_registry->processors.end());
      bool has_context = (current_registry->contexts.find(name) != current_registry->contexts.end());

      if (has_processor && (context_ptr || has_context)) {
        // Fast path: Everything exists, just use it.
        processor = proc_it->second;
        if (!context_ptr) {
          // We can safely take the address because the registry is immutable (shared_ptr keeps it alive)
          // Note: const_cast is needed because we store contexts in a const map in the snapshot
          context_ptr = const_cast<std::any *>(&current_registry->contexts.at(name));
        }
        break;
      }

      // 2. Slow path: Need to update state. Create a new copy (Copy-On-Write).
      auto new_registry = std::make_shared<Registry>(*current_registry);

      // Ensure Processor exists
      if (!has_processor) {
        processor = CreateDataStructure(type, *new_registry);
        if (processor) {
          new_registry->processors[name] = processor;
        }
      } else {
        processor = new_registry->processors[name];
      }

      // Ensure Context exists (only if we are not using specific_context)
      if (!specific_context && processor && new_registry->contexts.find(name) == new_registry->contexts.end()) {
        new_registry->contexts[name] = processor->CreateContext();
      }

      // 3. Atomic Swap
      if (std::atomic_compare_exchange_strong(&registry_, &current_registry, new_registry)) {
        // Success! The new registry is now the source of truth.
        // Update our local pointers to point to the new data
        if (processor && !specific_context) {
          context_ptr = &new_registry->contexts[name];
        }
        break;
      }
      // If CAS failed, another thread updated the registry. Loop again and retry with the new state.
    }

    if (processor && context_ptr) {
      processor->ConstructDataStructure(*context_ptr, args, key);
    } else {
      Logger::Log(L"Error: Failed to create or find data structure: %ls\n", name.c_str());
    }
  }

  void PrintData(const std::wstring &name) {
    auto current_registry = std::atomic_load(&registry_);

    auto proc_it = current_registry->processors.find(name);
    if (proc_it != current_registry->processors.end()) {
      auto ctx_it = current_registry->contexts.find(name);
      if (ctx_it != current_registry->contexts.end()) {
        // Safe to read from the snapshot
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string str_name = converter.to_bytes(name);

        std::filesystem::create_directories("regression");
        Logger::StartSecondaryLog("regression/" + str_name + ".log");

        proc_it->second->PrintDataStructure(const_cast<std::any &>(ctx_it->second));

        Logger::StopSecondaryLog();
      }
    }
  }

  std::shared_ptr<IDataStructure> GetDataStructure(const std::wstring &name) {
    auto current_registry = std::atomic_load(&registry_);
    auto it = current_registry->processors.find(name);
    if (it != current_registry->processors.end()) {
      return it->second;
    }
    return nullptr;
  }

  void MergeContexts(const std::wstring &name, const std::vector<std::any> &contexts) {
    auto processor = GetDataStructure(name);
    if (!processor) {
      return;
    }

    // Ensure global context exists
    std::any *global_context = nullptr;

    // Retry loop to ensure context exists
    while (true) {
      auto current_registry = std::atomic_load(&registry_);
      auto it = current_registry->contexts.find(name);
      if (it != current_registry->contexts.end()) {
        global_context = const_cast<std::any *>(&it->second);
        break;
      }

      // Create new registry with context
      auto new_registry = std::make_shared<Registry>(*current_registry);
      new_registry->contexts[name] = processor->CreateContext();

      if (std::atomic_compare_exchange_strong(&registry_, &current_registry, new_registry)) {
        global_context = &new_registry->contexts[name];
        break;
      }
    }

    for (const auto &local_ctx : contexts) {
      processor->MergeDataStructure(*global_context, local_ctx);
    }
  }

  std::any *GetDataContext(const std::wstring &name) {
    auto current_registry = std::atomic_load(&registry_);
    auto it = current_registry->contexts.find(name);
    if (it != current_registry->contexts.end()) {
      return const_cast<std::any *>(&it->second);
    }
    return nullptr;
  }

 private:
  std::shared_ptr<Registry> registry_;

  std::shared_ptr<IDataStructure> CreateDataStructure(const std::wstring &type, Registry &reg) {
    // Check cache in the new registry being built
    auto it = reg.type_cache.find(type);
    if (it != reg.type_cache.end()) {
      return it->second;
    }

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
    } else if (type == L"InsuranceOutput") {
      ds_instance = std::make_shared<InsuranceOutputDataStructure>(self);
    } else if (type == L"ExpenseOutput") {
      ds_instance = std::make_shared<ExpenseOutputDataStructure>(self);
    } else {
      Logger::Log(L"Warning: Unknown data structure type requested: %ls\n", type.c_str());
    }

    // Update cache in the new registry
    if (ds_instance) {
      reg.type_cache[type] = ds_instance;
    }
    return ds_instance;
  }
};
#endif  // SRC_DATAPROCESSOR_DATA_HELPER_H_
