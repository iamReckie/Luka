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
#include "SequenceStarter/sequence_starter.h"

#include <yaml-cpp/yaml.h>

#include <OpenXLSX.hpp>
#include <memory>
#include <string>

#include "CommandProcessor/command_processor.h"
#include "Environments/environment_parser.h"
#include "Logger/logger.h"
#include "Utility/string_utils.h"
int StartSequence(int argc, char* argv[]) {
  for (int i = 0; i < argc; i++) {
    printf("%s\n", argv[i]);
  }
  // Initialize logger
  Logger::Initialize("./output.log");

  // first step : initiate commands
  std::shared_ptr<CommandHelper> command_helper = std::make_shared<CommandHelper>();
  // scenario file is always scenario.yaml
  YAML::Node config;
  try {
    config = YAML::LoadFile("./scenario.yaml");
  } catch (YAML::BadFile& e) {
    Logger::Log(L"Config file error!");
    return -1;
  }
  // Run all the scenario items (environment or command)
  Environments::EnvironmentParser::Parse(config);
  for (const auto& item : config) {
    if (item.first.as<std::string>() == "scenario") {
      for (const auto& cmd : item.second) {
        Logger::Log(L"%ls\n", Ctw(cmd["command"].as<std::string>()).c_str());
        command_helper->ExecuteCommand(Ctw(cmd["command"].as<std::string>()),
                                       Ctw(cmd["name"].as<std::string>()), cmd);
      }
    }
  }
  Logger::Finalize();
  return 0;
}
