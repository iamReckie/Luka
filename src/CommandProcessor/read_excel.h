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
#ifndef SRC_COMMANDPROCESSOR_READ_EXCEL_H_
#define SRC_COMMANDPROCESSOR_READ_EXCEL_H_
#include <yaml-cpp/yaml.h>

#include <OpenXLSX.hpp>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "CommandProcessor/command_processor.h"

class ReadExcelCommand : public BaseCommand {
 public:
  explicit ReadExcelCommand(std::shared_ptr<DataHelper> helper)
      : BaseCommand(helper) {}

  void Execute(const YAML::Node& command_data) override;

  std::wstring get_cell_value(const OpenXLSX::XLCellValue& cell_value);

 private:
  // cells: vector of (column, cell_value)
  void ProcessRow(const std::vector<std::pair<int, std::wstring>>& cells,
                  const std::wstring& sheet_name,
                  const std::wstring& sheet_type,
                  std::any* context = nullptr);

  void ExecuteSingleThread(OpenXLSX::XLWorksheet& wks,
                           const std::vector<int>& ranges,
                           const std::wstring& sheet_name,
                           const std::wstring& sheet_type);

  void ExecuteMultiThread(OpenXLSX::XLWorksheet& wks,
                          const std::vector<int>& ranges,
                          const std::wstring& sheet_name,
                          const std::wstring& sheet_type);

  void ExecuteCuda(OpenXLSX::XLWorksheet& wks,
                   const std::vector<int>& ranges,
                   const std::wstring& sheet_name,
                   const std::wstring& sheet_type);
};
#endif  // SRC_COMMANDPROCESSOR_READ_EXCEL_H_
