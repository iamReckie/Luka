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

#include "Environments/global_environment.h"
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

void ReadExcelCommand::ProcessRow(const std::vector<std::pair<int, std::wstring>>& cells, const std::wstring& sheet_name, const std::wstring& sheet_type, std::any* context) {
  std::wstring key = L"";
  for (const auto& [col, cell_string] : cells) {
    if (cell_string.empty()) {
      continue;
    }
    std::vector<std::any> args{cell_string, col};
    data_helper_->ExecuteData(sheet_name, key, sheet_type, args, context);
  }
}
void ReadExcelCommand::ExecuteSingleThread(OpenXLSX::XLWorksheet& wks,
                                           const std::vector<int>& ranges,
                                           const std::wstring& sheet_name,
                                           const std::wstring& sheet_type) {
  Logger::Log(L"Processing %d rows in single thread mode\n", ranges[1] - ranges[0] + 1);

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
  int row_count = ranges[1] - ranges[0] + 1;

  // 1. Prepare Processor
  auto processor = data_helper_->GetOrRegisterProcessor(sheet_name, sheet_type);
  if (!processor) {
    Logger::Log(L"Failed to get processor for %ls\n", sheet_name.c_str());
    return;
  }

  std::vector<std::any> local_contexts;

  {
    ThreadPool pool;
    int num_workers = pool.GetNumWorkers();
    Logger::Log(L"Processing %d rows in multi thread mode with %d workers\n", row_count, num_workers);

    local_contexts.resize(num_workers);
    for (int i = 0; i < num_workers; ++i) {
      local_contexts[i] = processor->CreateContext();
    }

    int rows_per_worker = row_count / num_workers;
    int remainder = row_count % num_workers;
    int current_row = ranges[0];

    for (int i = 0; i < num_workers; ++i) {
      int start_row = current_row;
      int count = rows_per_worker + (i < remainder ? 1 : 0);
      int end_row = start_row + count - 1;
      current_row += count;

      if (count == 0) {
        continue;
      }

      std::vector<std::vector<std::pair<int, std::wstring>>> chunk_data;
      chunk_data.reserve(count);

      for (int r = start_row; r <= end_row; ++r) {
        std::vector<std::pair<int, std::wstring>> row_cells;
        for (int col = ranges[2]; col <= ranges[3]; col++) {
          OpenXLSX::XLCellReference cell_ref(r, col);
          OpenXLSX::XLCellValue cell_value = wks.cell(cell_ref).value();
          std::wstring cell_string = get_cell_value(cell_value);
          if (!cell_string.empty()) {
            row_cells.emplace_back(col, std::move(cell_string));
          }
        }
        chunk_data.push_back(std::move(row_cells));
      }

      std::any* ctx_ptr = &local_contexts[i];

      pool.EnqueueTask([this, data = std::move(chunk_data), sheet_name, sheet_type, ctx_ptr]() {
        for (const auto& row : data) {
          ProcessRow(row, sheet_name, sheet_type, ctx_ptr);
        }
      });
    }
  }  // Pool destroyed, waits for all tasks.

  // 2. Merge Results
  data_helper_->MergeContexts(sheet_name, local_contexts);
}

void ReadExcelCommand::ExecuteCuda(OpenXLSX::XLWorksheet& wks,
                                   const std::vector<int>& ranges,
                                   const std::wstring& sheet_name,
                                   const std::wstring& sheet_type) {
  Logger::Log(L"Processing %d rows in CUDA mode\n",
              ranges[1] - ranges[0] + 1);

#ifdef CUDA_ENABLED
  // Check CUDA availability
  if (!CudaProcessor::IsCudaAvailable()) {
    Logger::Log(L"CUDA is not available, falling back to multi thread mode\n");
    ExecuteMultiThread(wks, ranges, sheet_name, sheet_type);
    return;
  }

  // Print CUDA device information
  CudaProcessor::PrintCudaDeviceInfo();

  // Read all row data into memory first
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

  // Process data with CUDA
  bool cuda_success = CudaProcessor::ProcessRowsWithCuda(all_row_data);

  if (!cuda_success) {
    Logger::Log(L"CUDA processing failed, falling back to single thread mode\n");
    ExecuteSingleThread(wks, ranges, sheet_name, sheet_type);
    return;
  }

  // Pass results to ProcessRow after CUDA processing
  for (const auto& row_cells : all_row_data) {
    ProcessRow(row_cells, sheet_name, sheet_type);
  }

  Logger::Log(L"CUDA processing completed successfully\n");
#else
  // Fall back to multi-thread mode when CUDA is disabled
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

    // Use different processing method depending on execution mode
    Environments::ExecutionMode core_type = Environments::GlobalEnvironment::GetInstance().GetCoreType();
    std::cout << "Current Execution Mode: " << static_cast<int>(core_type) << std::endl;
    switch (core_type) {
      case Environments::ExecutionMode::SINGLE_THREAD:
        ExecuteSingleThread(wks, ranges, sheet_name, sheet_type);
        break;
      case Environments::ExecutionMode::MULTI_THREAD:
        ExecuteMultiThread(wks, ranges, sheet_name, sheet_type);
        break;
      case Environments::ExecutionMode::CUDA:
        ExecuteCuda(wks, ranges, sheet_name, sheet_type);
        break;
    }

    data_helper_->PrintData(sheet_name);
  }
}
