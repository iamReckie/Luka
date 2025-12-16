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
#include <cuda_runtime.h>
#include <stdio.h>

#include <iostream>

#include "Utility/cuda_processor.h"

namespace CudaProcessor {

// CUDA 에러 체크 매크로
#define CUDA_CHECK(call)                                               \
  do {                                                                 \
    cudaError_t error = call;                                          \
    if (error != cudaSuccess) {                                        \
      fprintf(stderr, "CUDA error at %s:%d: %s\n", __FILE__, __LINE__, \
              cudaGetErrorString(error));                              \
      return false;                                                    \
    }                                                                  \
  } while (0)

// CUDA 커널: 각 행의 데이터를 병렬 처리
// 예시로 간단한 계산을 수행 (실제 사용 시 필요한 처리로 변경)
__global__ void ProcessRowKernel(int* row_indices, int* col_indices,
                                 float* values, int* results,
                                 int total_cells) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;

  if (idx < total_cells) {
    // 예시: 간단한 계산 수행
    // 실제로는 여기서 복잡한 데이터 처리 로직이 들어감
    results[idx] = row_indices[idx] * 100 + col_indices[idx];
  }
}

bool IsCudaAvailable() {
  int device_count = 0;
  cudaError_t error = cudaGetDeviceCount(&device_count);

  if (error != cudaSuccess || device_count == 0) {
    return false;
  }

  return true;
}

void PrintCudaDeviceInfo() {
  int device_count = 0;
  cudaGetDeviceCount(&device_count);

  std::cout << "CUDA Device Count: " << device_count << std::endl;

  for (int i = 0; i < device_count; i++) {
    cudaDeviceProp prop;
    cudaGetDeviceProperties(&prop, i);

    std::cout << "Device " << i << ": " << prop.name << std::endl;
    std::cout << "  Compute Capability: " << prop.major << "." << prop.minor
              << std::endl;
    std::cout << "  Total Global Memory: "
              << prop.totalGlobalMem / (1024 * 1024) << " MB" << std::endl;
    std::cout << "  Multiprocessor Count: " << prop.multiProcessorCount
              << std::endl;
  }
}

bool ProcessRowsWithCuda(
    const std::vector<std::vector<std::pair<int, std::wstring>>>& row_data) {
  if (!IsCudaAvailable()) {
    fprintf(stderr, "CUDA is not available\n");
    return false;
  }

  // 전체 셀 개수 계산
  int total_cells = 0;
  for (const auto& row : row_data) {
    total_cells += row.size();
  }

  if (total_cells == 0) {
    return true;  // 처리할 데이터 없음
  }

  // 호스트 메모리 할당
  std::vector<int> h_row_indices(total_cells);
  std::vector<int> h_col_indices(total_cells);
  std::vector<float> h_values(total_cells);
  std::vector<int> h_results(total_cells);

  // 데이터를 평탄화(flatten)하여 배열로 변환
  int cell_idx = 0;
  int current_row = 0;
  for (const auto& row : row_data) {
    for (const auto& [col, cell_string] : row) {
      h_row_indices[cell_idx] = current_row;
      h_col_indices[cell_idx] = col;

      // 문자열을 숫자로 변환 시도 (예시)
      try {
        h_values[cell_idx] = std::stof(std::string(cell_string.begin(),
                                                   cell_string.end()));
      } catch (...) {
        h_values[cell_idx] = 0.0f;
      }

      cell_idx++;
    }
    current_row++;
  }

  // 디바이스 메모리 할당
  int *d_row_indices, *d_col_indices, *d_results;
  float* d_values;

  CUDA_CHECK(cudaMalloc(&d_row_indices, total_cells * sizeof(int)));
  CUDA_CHECK(cudaMalloc(&d_col_indices, total_cells * sizeof(int)));
  CUDA_CHECK(cudaMalloc(&d_values, total_cells * sizeof(float)));
  CUDA_CHECK(cudaMalloc(&d_results, total_cells * sizeof(int)));

  // 호스트에서 디바이스로 데이터 복사
  CUDA_CHECK(cudaMemcpy(d_row_indices, h_row_indices.data(),
                        total_cells * sizeof(int), cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_col_indices, h_col_indices.data(),
                        total_cells * sizeof(int), cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_values, h_values.data(), total_cells * sizeof(float),
                        cudaMemcpyHostToDevice));

  // 커널 실행 설정
  int threads_per_block = 256;
  int blocks = (total_cells + threads_per_block - 1) / threads_per_block;

  // CUDA 커널 실행
  ProcessRowKernel<<<blocks, threads_per_block>>>(
      d_row_indices, d_col_indices, d_values, d_results, total_cells);

  // 커널 실행 완료 대기 및 에러 체크
  CUDA_CHECK(cudaDeviceSynchronize());

  // 결과를 디바이스에서 호스트로 복사
  CUDA_CHECK(cudaMemcpy(h_results.data(), d_results, total_cells * sizeof(int),
                        cudaMemcpyDeviceToHost));

  // 디바이스 메모리 해제
  cudaFree(d_row_indices);
  cudaFree(d_col_indices);
  cudaFree(d_values);
  cudaFree(d_results);

  // 결과 처리 (예시로 출력)
  printf("CUDA processed %d cells successfully\n", total_cells);

  return true;
}

}  // namespace CudaProcessor
