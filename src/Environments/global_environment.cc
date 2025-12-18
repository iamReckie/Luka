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
#include "Environments/global_environment.h"

namespace Environments {

GlobalEnvironment& GlobalEnvironment::GetInstance() {
  static GlobalEnvironment instance;
  return instance;
}

void GlobalEnvironment::SetExecutionMode(ExecutionMode mode) {
  execution_mode_ = mode;
}

ExecutionMode GlobalEnvironment::GetExecutionMode() const {
  return execution_mode_;
}

}  // namespace Environments
