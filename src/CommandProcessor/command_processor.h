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
  CommandHelper() : data_helper_(std::make_shared<DataHelper>()) {}
  ~CommandHelper() = default;
  void ExecuteCommand(const std::wstring& command_name,
                      const std::wstring& name, const YAML::Node& command_data);
  void RegisterCommand(const std::wstring& command_name,
                       const std::wstring& name);

 private:
  std::shared_ptr<DataHelper> data_helper_;
  std::unordered_map<
      std::wstring,
      std::unordered_map<std::wstring, std::shared_ptr<BaseCommand>>>
      commands_;
};
#endif  // SRC_COMMANDPROCESSOR_COMMAND_PROCESSOR_H_
