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
#ifndef SRC_COMMANDPROCESSOR_CALC_INSURANCE_OUTPUT_H_
#define SRC_COMMANDPROCESSOR_CALC_INSURANCE_OUTPUT_H_
#include <yaml-cpp/yaml.h>

#include <OpenXLSX.hpp>
#include <memory>
#include <string>
#include <vector>

#include "CommandProcessor/command_processor.h"
class CalcInsuranceOutputCommand : public BaseCommand {
 public:
  explicit CalcInsuranceOutputCommand(std::shared_ptr<DataHelper> helper)
      : BaseCommand(helper) {}
  void Execute(const YAML::Node& command_data) override;
  std::vector<std::wstring> SplitAndConvertToWString(const std::string& input);
};
#endif  // SRC_COMMANDPROCESSOR_CALC_INSURANCE_OUTPUT_H_
