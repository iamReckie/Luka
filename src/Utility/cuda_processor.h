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
#ifndef SRC_UTILITY_CUDA_PROCESSOR_H_
#define SRC_UTILITY_CUDA_PROCESSOR_H_

#include <string>
#include <utility>
#include <vector>

namespace CudaProcessor {

// Parallel processing of Excel row data using CUDA
// row_data: pairs of (column index, cell value) per row
// Returns: true if processing succeeded, false otherwise
bool ProcessRowsWithCuda(const std::vector<std::vector<std::pair<int, std::wstring>>>& row_data);

// Check if a CUDA device is available
bool IsCudaAvailable();

// Print CUDA device information
void PrintCudaDeviceInfo();

}  // namespace CudaProcessor

#endif  // SRC_UTILITY_CUDA_PROCESSOR_H_
