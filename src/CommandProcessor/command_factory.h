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
#ifndef SRC_COMMANDPROCESSOR_COMMAND_FACTORY_H_
#define SRC_COMMANDPROCESSOR_COMMAND_FACTORY_H_
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

// Forward declarations
class BaseCommand;
class DataHelper;

class CommandFactory {
 public:
  using CommandCreator = std::function<std::shared_ptr<BaseCommand>(std::shared_ptr<DataHelper>)>;

  static CommandFactory& GetInstance() {
    static CommandFactory instance;
    return instance;
  }

  void RegisterCommand(const std::wstring& command_name, CommandCreator creator) {
    command_creators_[command_name] = creator;
  }

  std::shared_ptr<BaseCommand> CreateCommand(const std::wstring& command_name, std::shared_ptr<DataHelper> data_helper) {
    auto it = command_creators_.find(command_name);
    if (it != command_creators_.end()) {
      return it->second(data_helper);
    }
    return nullptr;
  }

 private:
  CommandFactory() = default;
  ~CommandFactory() = default;
  CommandFactory(const CommandFactory&) = delete;
  CommandFactory& operator=(const CommandFactory&) = delete;

  std::unordered_map<std::wstring, CommandCreator> command_creators_;
};

#endif  // SRC_COMMANDPROCESSOR_COMMAND_FACTORY_H_
