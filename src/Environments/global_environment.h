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
#ifndef SRC_ENVIRONMENTS_GLOBAL_ENVIRONMENT_H_
#define SRC_ENVIRONMENTS_GLOBAL_ENVIRONMENT_H_

namespace Environments {

enum class ExecutionMode {
  SINGLE_THREAD,
  MULTI_THREAD,
  CUDA
};

class GlobalEnvironment {
 public:
  static GlobalEnvironment& GetInstance();
  void SetExecutionMode(ExecutionMode mode);
  ExecutionMode GetExecutionMode() const;

  GlobalEnvironment(const GlobalEnvironment&) = delete;
  GlobalEnvironment& operator=(const GlobalEnvironment&) = delete;

 private:
  GlobalEnvironment() = default;
  ExecutionMode execution_mode_ = ExecutionMode::MULTI_THREAD;
};

}  // namespace Environments

#endif  // SRC_ENVIRONMENTS_GLOBAL_ENVIRONMENT_H_
