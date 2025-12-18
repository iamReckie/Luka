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
      if (env["thread_type"]) {
        std::string thread_type = env["thread_type"].as<std::string>();
        Logger::Log(L"[ENV] thread_type: %ls\n", Ctw(thread_type).c_str());

        ExecutionMode mode = ExecutionMode::MULTI_THREAD;
        if (thread_type == "single") {
          mode = ExecutionMode::SINGLE_THREAD;
        } else if (thread_type == "multi") {
          mode = ExecutionMode::MULTI_THREAD;
        } else if (thread_type == "cuda") {
          mode = ExecutionMode::CUDA;
        }
        GlobalEnvironment::GetInstance().SetExecutionMode(mode);
      }
    }
  }
}

}  // namespace Environments
