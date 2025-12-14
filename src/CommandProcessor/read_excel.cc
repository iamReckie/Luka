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
#include <thread>

#include "Utility/excel_utils.h"
#include "Utility/string_utils.h"
#include "Logger/logger.h"
#include "Utility/thread_pool.h"
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

void ReadExcelCommand::ProcessRow(OpenXLSX::XLWorksheet wks, int row,
                                   int col_start, int col_end,
                                   const std::wstring& sheet_name,
                                   const std::wstring& sheet_type) {
  std::wstring key = L"";
  for (int col = col_start; col <= col_end; col++) {
    OpenXLSX::XLCellReference cell_ref(row, col);
    OpenXLSX::XLCellValue cell_value = wks.cell(cell_ref).value();
    std::wstring cell_string = get_cell_value(cell_value);
    if (cell_string.empty()) {
      continue;
    }
    std::vector<std::any> args{cell_string, col};
    data_helper_->ExecuteData(sheet_name, key, sheet_type, args);
  }
}
void ReadExcelCommand::Execute(const YAML::Node &command_data) {
  OpenXLSX::XLDocument doc;
  std::string excel_name = command_data["name"].as<std::string>();
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

    // 멀티스레드 처리: ThreadPool 사용
    ThreadPool pool;
    int row_count = ranges[1] - ranges[0] + 1;
    Logger::Log(L"Processing %d rows in parallel\n", row_count);

    for (int row = ranges[0]; row <= ranges[1]; row++) {
      pool.EnqueueTask([this, wks, row, col_start = ranges[2],
                        col_end = ranges[3], sheet_name, sheet_type]() {
        ProcessRow(wks, row, col_start, col_end, sheet_name, sheet_type);
      });
    }
    // ThreadPool 소멸자에서 모든 작업이 완료될 때까지 대기

    data_helper_->PrintData(sheet_name);
  }
}
