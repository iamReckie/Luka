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

// Excel 행 데이터를 CUDA로 병렬 처리
// row_data: 각 행의 (열 번호, 셀 값) 쌍들
// Returns: 처리 성공 여부
bool ProcessRowsWithCuda(const std::vector<std::vector<std::pair<int, std::wstring>>>& row_data);

// CUDA 디바이스 사용 가능 여부 확인
bool IsCudaAvailable();

// CUDA 디바이스 정보 출력
void PrintCudaDeviceInfo();

}  // namespace CudaProcessor

#endif  // SRC_UTILITY_CUDA_PROCESSOR_H_
