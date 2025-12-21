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
#include "Environments/environment_parser.h"

#include <string>

#include "Environments/global_environment.h"
#include "Logger/logger.h"
#include "Utility/string_utils.h"

namespace Environments {

void EnvironmentParser::Parse(const YAML::Node& config) {
  for (const auto& item : config) {
    std::string key = item.first.as<std::string>();
    if (key == "environments" || key == "environment") {
      const auto& env = item.second;

      if (env["core_type"]) {
        std::string core_type = env["core_type"].as<std::string>();
        Logger::Log(L"[ENV] core_type: %ls\n", Ctw(core_type).c_str());

        ExecutionMode core_mode = ExecutionMode::MULTI_THREAD;
        if (core_type == "single") {
          core_mode = ExecutionMode::SINGLE_THREAD;
        } else if (core_type == "multi") {
          core_mode = ExecutionMode::MULTI_THREAD;
        } else if (core_type == "cuda") {
          core_mode = ExecutionMode::CUDA;
        }
        GlobalEnvironment::GetInstance().SetCoreType(core_mode);
      }
    }
  }
}

}  // namespace Environments
