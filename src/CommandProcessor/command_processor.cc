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
#include "CommandProcessor/command_processor.h"

#include <memory>

#include "CommandProcessor/calc_insurance_expense.h"
#include "CommandProcessor/calc_reserve_expense.h"
#include "CommandProcessor/read_excel.h"
#include "CommandProcessor/read_tbl.h"
#include "Logger/logger.h"
void CommandHelper::RegisterCommand(const std::wstring& command_name,
                                    const std::wstring& name) {
  if (command_name == L"read_excel") {
    commands_[command_name][name] =
        std::make_shared<ReadExcelCommand>(data_helper_);
  } else if (command_name == L"read_tbl") {
    commands_[command_name][name] =
        std::make_shared<ReadTblCommand>(data_helper_);
  } else if (command_name == L"calc_insurance_expense") {
    commands_[command_name][name] =
        std::make_shared<CalcInsuranceExpenseCommand>(data_helper_);
  } else if (command_name == L"calc_reserve_expense") {
    commands_[command_name][name] =
        std::make_shared<CalcReserveExpenseCommand>(data_helper_);
  } else {
    Logger::Log(L"Unknown command %ls\n", command_name.c_str());
  }
}
void CommandHelper::ExecuteCommand(const std::wstring& command_name,
                                   const std::wstring& name,
                                   const YAML::Node& command_data) {
  RegisterCommand(command_name, name);
  if (commands_.find(command_name) != commands_.end()) {
    commands_[command_name][name]->Execute(command_data);
  } else {
    Logger::Log(L"Command not found %ls\n", name.c_str());
  }
}
