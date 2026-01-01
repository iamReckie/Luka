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
#include "CommandProcessor/environments_command.h"
#include "CommandProcessor/read_excel.h"
#include "CommandProcessor/read_tbl.h"

void CommandHelper::RegisterAllCommands() {
  // Pre-register all commands at initialization time (thread-safe: only called in constructor)
  command_instances_[L"environments"] =
      std::make_shared<EnvironmentsCommand>(data_helper_);
  command_instances_[L"read_excel"] =
      std::make_shared<ReadExcelCommand>(data_helper_);
  command_instances_[L"read_tbl"] =
      std::make_shared<ReadTblCommand>(data_helper_);
  command_instances_[L"calc_insurance_expense"] =
      std::make_shared<CalcInsuranceExpenseCommand>(data_helper_);
  command_instances_[L"calc_reserve_expense"] =
      std::make_shared<CalcReserveExpenseCommand>(data_helper_);
}
