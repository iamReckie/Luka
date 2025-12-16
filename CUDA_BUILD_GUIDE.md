# CUDA 빌드 가이드

## 개요
Luka 프로젝트에서 CUDA를 사용한 GPU 가속 Excel 데이터 처리 기능이 추가되었습니다.

## 실행 모드

`ReadExcelCommand`는 세 가지 실행 모드를 지원합니다:

1. **SINGLE_THREAD**: 단일 스레드로 순차 처리
2. **MULTI_THREAD**: ThreadPool을 사용한 CPU 병렬 처리 (기본값)
3. **CUDA**: GPU를 사용한 병렬 처리

## 빌드 방법

### CUDA 없이 빌드 (기본)
```bash
cmake -S . -B build
cmake --build build
```

### CUDA 지원 빌드
CUDA Toolkit이 설치되어 있어야 합니다.

```bash
cmake -S . -B build -DENABLE_CUDA=ON
cmake --build build
```

## CUDA Toolkit 설치

### Linux (Ubuntu/Debian)
```bash
# NVIDIA 드라이버 설치 확인
nvidia-smi

# CUDA Toolkit 설치
wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.0-1_all.deb
sudo dpkg -i cuda-keyring_1.0-1_all.deb
sudo apt-get update
sudo apt-get install cuda
```

### Windows
1. [NVIDIA CUDA Toolkit](https://developer.nvidia.com/cuda-downloads) 다운로드
2. 설치 프로그램 실행
3. 환경 변수 자동 설정 확인

## 사용 예시

```cpp
auto data_helper = std::make_shared<DataHelper>();
auto command = std::make_shared<ReadExcelCommand>(data_helper);

// 실행 모드 설정
command->SetExecutionMode(ExecutionMode::CUDA);

// Excel 파일 처리
YAML::Node config = YAML::LoadFile("config.yaml");
command->Execute(config["read_excel"]);
```

## 성능 비교

대용량 Excel 파일 처리 시 예상 성능:

- **SINGLE_THREAD**: 기준 (1x)
- **MULTI_THREAD**: 4-8x 빠름 (CPU 코어 수에 따라)
- **CUDA**: 10-100x 빠름 (GPU 성능에 따라)

## 주의사항

1. CUDA 모드는 ENABLE_CUDA=ON으로 빌드된 경우에만 사용 가능
2. CUDA가 없는 환경에서 CUDA 모드 선택 시 자동으로 MULTI_THREAD로 fallback
3. GPU 메모리가 부족한 경우 처리 실패 가능

## 파일 구조

- `src/Utility/cuda_processor.h`: CUDA 처리 인터페이스
- `src/Utility/cuda_processor.cu`: CUDA 커널 및 구현
- `src/CommandProcessor/read_excel.h`: 실행 모드 enum 및 메서드 선언
- `src/CommandProcessor/read_excel.cc`: 각 모드별 구현

## 커스터마이징

CUDA 커널을 수정하여 실제 데이터 처리 로직을 구현할 수 있습니다:

[cuda_processor.cu](src/Utility/cuda_processor.cu)의 `ProcessRowKernel` 함수를 수정하세요.

```cuda
__global__ void ProcessRowKernel(int* row_indices, int* col_indices,
                                   float* values, int* results,
                                   int total_cells) {
  int idx = blockIdx.x * blockDim.x + threadIdx.x;
  
  if (idx < total_cells) {
    // 여기에 실제 처리 로직 구현
    results[idx] = /* 계산 결과 */;
  }
}
```
