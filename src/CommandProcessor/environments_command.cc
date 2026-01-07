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
#include "CommandProcessor/environments_command.h"

#include <string>

#include "Environments/global_environment.h"
#include "Logger/logger.h"
#include "Utility/string_utils.h"

void EnvironmentsCommand::Execute(const YAML::Node& command_data) {
  if (command_data["name"] && command_data["name"].as<std::string>() == "core_type") {
    if (command_data["value"]) {
      std::string core_type = command_data["value"].as<std::string>();
      Logger::Log(L"[ENV] core_type: %ls\n", Ctw(core_type).c_str());

      Environments::ExecutionMode core_mode = Environments::ExecutionMode::MULTI_THREAD;
      if (core_type == "single") {
        core_mode = Environments::ExecutionMode::SINGLE_THREAD;
      } else if (core_type == "multi") {
        core_mode = Environments::ExecutionMode::MULTI_THREAD;
      } else if (core_type == "cuda") {
        core_mode = Environments::ExecutionMode::CUDA;
      }
      Environments::GlobalEnvironment::GetInstance().SetCoreType(core_mode);
    }
  }
}
