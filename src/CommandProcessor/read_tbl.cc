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
#include "CommandProcessor/read_tbl.h"

#include <fstream>
#include <string>
#include <vector>

#include "Logger/logger.h"
#include "Utility/abort.h"
#include "Utility/string_utils.h"
void ReadTblCommand::Execute(const YAML::Node &command_data) {
  std::string file_name = command_data["name"].as<std::string>();
  Logger::Log(L"Read %ls\n", Ctw(file_name).c_str());
  std::ifstream tbl_file(file_name);
  if (!tbl_file) {
    Abort(L"Failed to open file %ls\n", Ctw(file_name).c_str());
  }
  std::string line{};
  std::wstring key{Ctw(file_name)};
  while (std::getline(tbl_file, line)) {
    std::vector<std::any> args{Ctw(line)};
    data_helper_->ExecuteData(Ctw(file_name), key, L"Table", args);
  }
  tbl_file.close();
  data_helper_->PrintData(Ctw(file_name));
}
