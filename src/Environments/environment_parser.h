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
#ifndef SRC_ENVIRONMENTS_ENVIRONMENT_PARSER_H_
#define SRC_ENVIRONMENTS_ENVIRONMENT_PARSER_H_

#include <yaml-cpp/yaml.h>

namespace Environments {

class EnvironmentParser {
 public:
  static void Parse(const YAML::Node& config);
};

}  // namespace Environments

#endif  // SRC_ENVIRONMENTS_ENVIRONMENT_PARSER_H_
