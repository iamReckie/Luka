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
#ifndef SRC_COMMANDPROCESSOR_ENVIRONMENTS_COMMAND_H_
#define SRC_COMMANDPROCESSOR_ENVIRONMENTS_COMMAND_H_

#include <memory>

#include "CommandProcessor/command_processor.h"

class EnvironmentsCommand : public BaseCommand {
 public:
  explicit EnvironmentsCommand(std::shared_ptr<DataHelper> helper)
      : BaseCommand(helper) {}
  void Execute(const YAML::Node& command_data) override;
};

#endif  // SRC_COMMANDPROCESSOR_ENVIRONMENTS_COMMAND_H_
