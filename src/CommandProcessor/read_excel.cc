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

#include "Logger/logger.h"
#include "Utility/excel_utils.h"
#include "Utility/string_utils.h"
#include "Utility/thread_pool.h"

#ifdef CUDA_ENABLED
#include "Utility/cuda_processor.h"
#endif
std::wstring
ReadExcelCommand::get_cell_value(const OpenXLSX::XLCellValue& cell_value) {
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

void ReadExcelCommand::ProcessRow(const std::vector<std::pair<int, std::wstring>>& cells, const std::wstring& sheet_name, const std::wstring& sheet_type) {
  std::wstring key = L"";
  for (const auto& [col, cell_string] : cells) {
    if (cell_string.empty()) {
      continue;
    }
    std::vector<std::any> args{cell_string, col};
    data_helper_->ExecuteData(sheet_name, key, sheet_type, args);
  }
}
void ReadExcelCommand::ExecuteSingleThread(OpenXLSX::XLWorksheet& wks,
                                           const std::vector<int>& ranges,
                                           const std::wstring& sheet_name,
                                           const std::wstring& sheet_type) {
  Logger::Log(L"Processing %d rows in single thread mode\n",
              ranges[1] - ranges[0] + 1);

  for (int row = ranges[0]; row <= ranges[1]; row++) {
    std::vector<std::pair<int, std::wstring>> row_cells;
    for (int col = ranges[2]; col <= ranges[3]; col++) {
      OpenXLSX::XLCellReference cell_ref(row, col);
      OpenXLSX::XLCellValue cell_value = wks.cell(cell_ref).value();
      std::wstring cell_string = get_cell_value(cell_value);
      if (!cell_string.empty()) {
        row_cells.emplace_back(col, std::move(cell_string));
      }
    }
    ProcessRow(row_cells, sheet_name, sheet_type);
  }
}

void ReadExcelCommand::ExecuteMultiThread(OpenXLSX::XLWorksheet& wks,
                                          const std::vector<int>& ranges,
                                          const std::wstring& sheet_name,
                                          const std::wstring& sheet_type) {
  ThreadPool pool;
  int row_count = ranges[1] - ranges[0] + 1;
  Logger::Log(L"Processing %d rows in multi thread mode\n", row_count);

  for (int row = ranges[0]; row <= ranges[1]; row++) {
    // Read all cells for this row on the main thread
    std::vector<std::pair<int, std::wstring>> row_cells;
    for (int col = ranges[2]; col <= ranges[3]; col++) {
      OpenXLSX::XLCellReference cell_ref(row, col);
      OpenXLSX::XLCellValue cell_value = wks.cell(cell_ref).value();
      std::wstring cell_string = get_cell_value(cell_value);
      if (!cell_string.empty()) {
        row_cells.emplace_back(col, std::move(cell_string));
      }
    }

    // Move the collected row_cells into the task so workers don't access OpenXLSX
    pool.EnqueueTask([this, row_cells = std::move(row_cells), sheet_name,
                      sheet_type]() mutable {
      ProcessRow(row_cells, sheet_name, sheet_type);
    });
  }
  // ThreadPool 소멸자에서 모든 작업이 완료될 때까지 대기
}

void ReadExcelCommand::ExecuteCuda(OpenXLSX::XLWorksheet& wks,
                                   const std::vector<int>& ranges,
                                   const std::wstring& sheet_name,
                                   const std::wstring& sheet_type) {
  Logger::Log(L"Processing %d rows in CUDA mode\n",
              ranges[1] - ranges[0] + 1);

#ifdef CUDA_ENABLED
  // CUDA 사용 가능 여부 확인
  if (!CudaProcessor::IsCudaAvailable()) {
    Logger::Log(L"CUDA is not available, falling back to multi thread mode\n");
    ExecuteMultiThread(wks, ranges, sheet_name, sheet_type);
    return;
  }

  // CUDA 디바이스 정보 출력
  CudaProcessor::PrintCudaDeviceInfo();

  // 먼저 모든 행 데이터를 메모리로 읽어들임
  std::vector<std::vector<std::pair<int, std::wstring>>> all_row_data;
  all_row_data.reserve(ranges[1] - ranges[0] + 1);

  for (int row = ranges[0]; row <= ranges[1]; row++) {
    std::vector<std::pair<int, std::wstring>> row_cells;
    for (int col = ranges[2]; col <= ranges[3]; col++) {
      OpenXLSX::XLCellReference cell_ref(row, col);
      OpenXLSX::XLCellValue cell_value = wks.cell(cell_ref).value();
      std::wstring cell_string = get_cell_value(cell_value);
      if (!cell_string.empty()) {
        row_cells.emplace_back(col, std::move(cell_string));
      }
    }
    all_row_data.push_back(std::move(row_cells));
  }

  // CUDA로 데이터 처리
  bool cuda_success = CudaProcessor::ProcessRowsWithCuda(all_row_data);

  if (!cuda_success) {
    Logger::Log(L"CUDA processing failed, falling back to single thread mode\n");
    ExecuteSingleThread(wks, ranges, sheet_name, sheet_type);
    return;
  }

  // CUDA 처리 후 결과를 ProcessRow로 전달
  for (const auto& row_cells : all_row_data) {
    ProcessRow(row_cells, sheet_name, sheet_type);
  }

  Logger::Log(L"CUDA processing completed successfully\n");
#else
  // CUDA가 비활성화된 경우 multi thread로 fallback
  Logger::Log(L"CUDA is not enabled in this build, falling back to multi thread mode\n");
  ExecuteMultiThread(wks, ranges, sheet_name, sheet_type);
#endif
}

void ReadExcelCommand::Execute(const YAML::Node& command_data) {
  OpenXLSX::XLDocument doc;
  std::string excel_name = command_data["name"].as<std::string>();
  Logger::Log(L"Read %ls\n", Ctw(excel_name).c_str());
  doc.open(excel_name);

  for (const auto& sheet : command_data["sheets"]) {
    std::wstring sheet_name = Ctw(sheet["name"].as<std::string>()),
                 range = Ctw(sheet["range"].as<std::string>()),
                 sheet_type = Ctw(sheet["type"].as<std::string>());
    std::vector<int> ranges;
    Logger::Log(L"sheet name : %ls type : %ls\n", sheet_name.c_str(),
                sheet_type.c_str());
    ranges = ExcelUtils::ParseExcelRange(range);
    auto wks = doc.workbook().worksheet(Cts(sheet_name));

    // 실행 모드에 따라 다른 처리 방식 사용
    switch (execution_mode_) {
      case ExecutionMode::SINGLE_THREAD:
        ExecuteSingleThread(wks, ranges, sheet_name, sheet_type);
        break;
      case ExecutionMode::MULTI_THREAD:
        ExecuteMultiThread(wks, ranges, sheet_name, sheet_type);
        break;
      case ExecutionMode::CUDA:
        ExecuteCuda(wks, ranges, sheet_name, sheet_type);
        break;
    }

    data_helper_->PrintData(sheet_name);
  }
}
