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
#include "Converter/string_utils.h"
#include "Logger/logger.h"
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
  // Run all the commands.
  for (const auto& item : config["scenario"]) {
    Logger::Log(L"%ls\n", Ctw(item["command"].as<std::string>()).c_str());
    command_helper->ExecuteCommand(Ctw(item["command"].as<std::string>()),
                                   Ctw(item["name"].as<std::string>()), item);
  }
  Logger::Finalize();
  return 0;
}
