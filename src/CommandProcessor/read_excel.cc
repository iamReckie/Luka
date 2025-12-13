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
#include "CommandProcessor/read_excel.h"

#include <OpenXLSX.hpp>

#include "Converter/excel_utils.h"
#include "Converter/string_utils.h"
#include "Logger/logger.h"
std::wstring
ReadExcelCommand::get_cell_value(const OpenXLSX::XLCellValue &cell_value) {
  std::wstring type = Ctw(cell_value.typeAsString());
  if (type == L"integer") {
    return Ctw(std::to_string(cell_value.get<int>()));
  } else if (type == L"float") {
    return Ctw(std::to_string(cell_value.get<double>()));
  } else if (type == L"string") {
    return Ctw(cell_value.get<std::string>());
  } else {
    return L"";
  }
}
void ReadExcelCommand::Execute(const YAML::Node &command_data) {
  OpenXLSX::XLDocument doc;
  std::string excel_name = command_data["name"].as<std::string>();
  std::wstring key = L"";
  Logger::Log(L"Read %ls\n", Ctw(excel_name).c_str());
  doc.open(excel_name);
  for (const auto &sheet : command_data["sheets"]) {
    std::wstring sheet_name = Ctw(sheet["name"].as<std::string>()),
                 range = Ctw(sheet["range"].as<std::string>()),
                 sheet_type = Ctw(sheet["type"].as<std::string>());
    std::vector<int> ranges;
    Logger::Log(L"sheet name : %ls type : %ls\n", sheet_name.c_str(),
                sheet_type.c_str());
    ranges = ExcelUtils::ParseExcelRange(range);
    auto wks = doc.workbook().worksheet(Cts(sheet_name));
    for (int row = ranges[0]; row <= ranges[1]; row++) {
      key = L"";
      for (int col = ranges[2]; col <= ranges[3]; col++) {
        OpenXLSX::XLCellReference cell_ref(row, col);
        OpenXLSX::XLCellValue cell_value = wks.cell(cell_ref).value();
        std::wstring cell_string = get_cell_value(cell_value);
        if (cell_string.empty()) {
          continue;
        }
        // Logger::log("%s ", cell_string.c_str());
        std::vector<std::any> args{cell_string, col};
        data_helper_->ExecuteData(sheet_name, key, sheet_type, args);
      }
      // Logger::log("\n");
    }
    data_helper_->PrintData(sheet_name);
  }
}
