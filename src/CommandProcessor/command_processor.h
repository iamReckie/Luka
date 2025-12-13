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
#ifndef SRC_COMMANDPROCESSOR_COMMAND_PROCESSOR_H_
#define SRC_COMMANDPROCESSOR_COMMAND_PROCESSOR_H_
#include <atomic>
#include <memory>
#include <unordered_map>

#include "../OpenXLSX/OpenXLSX.hpp"
#include "../yaml-cpp/yaml.h"
#include "DataProcessor/data_helper.h"
class ICommand {
 public:
  virtual void Execute(const YAML::Node& command_data) = 0;
  virtual ~ICommand() = default;
};
class BaseCommand : public ICommand {
 public:
  void Execute(const YAML::Node& command_data) override = 0;
  explicit BaseCommand(std::shared_ptr<DataHelper> helper)
      : data_helper_(helper) {}
  virtual ~BaseCommand() = default;

 protected:
  std::shared_ptr<DataHelper> data_helper_;
};

class CommandHelper {
 public:
  CommandHelper() : data_helper_(std::make_shared<DataHelper>()),
                    commands_initialized_(false) {
    RegisterAllCommands();
    // Mark as initialized with release semantics for lock-free synchronization
    commands_initialized_.store(true, std::memory_order_release);
  }
  ~CommandHelper() = default;

  void ExecuteCommand(const std::wstring& command_name,
                      const std::wstring& /*name*/, const YAML::Node& command_data) {
    // All commands are pre-initialized, just look up and execute
    auto it = command_instances_.find(command_name);
    if (it != command_instances_.end()) {
      it->second->Execute(command_data);
    } else {
      Logger::Log(L"Unknown command %ls\n", command_name.c_str());
    }
  }

 private:
  std::shared_ptr<DataHelper> data_helper_;
  std::unordered_map<std::wstring, std::shared_ptr<BaseCommand>>
      command_instances_;
  std::atomic<bool> commands_initialized_;  // Lock-free initialization flag

  void RegisterAllCommands();
};
#endif  // SRC_COMMANDPROCESSOR_COMMAND_PROCESSOR_H_
