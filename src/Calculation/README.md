# Calculation Module - 보험수리 계산 모듈

## Overview
이 모듈은 보험수리 계산을 위한 교차함수(Commutation Functions)와 위험보험료를 계산하는 기능을 제공합니다.

This module provides actuarial calculations including commutation functions and risk premium calculations.

## Files
- `actuarial_calculator.h` - 보험수리 계산 함수 인터페이스
- `actuarial_calculator.cc` - 보험수리 계산 함수 구현
- `example_usage.h` - 사용 예시 인터페이스
- `example_usage.cc` - 사용 예시 구현

## Functions

### 1. ComputationTermPr
정기보험 위험보험료를 계산합니다.

**Parameters:**
- `x` (int): 가입나이 (entry age)
- `nn` (int): 보험기간 (insurance period in years)
- `mm` (int): 납입기간 (premium payment period in years)
- `Sex` (int): 성별 (0=남성/male, 1=여성/female)
- `AMT` (int): 보험가입금액 (insurance amount)
- `QPr` (function): 위험률 함수 QPr(risk_class, sex, age) -> double
- `V0` (function): 만기시점 할인함수 V0(t) -> double
- `V1` (function): 1년 후 할인함수 V1(t) -> double

**Returns:**
- `double`: 월납 보험료 (monthly premium)

**Example:**
```cpp
auto QPr = [](int risk_class, int sex, int age) -> double {
    // QxTable에서 위험률 가져오기
    return GetQPrFromTable(risk_class, sex, age);
};

auto V0 = [](int t) -> double {
    double interest_rate = 0.025; // 2.5%
    return std::pow(1.0 + interest_rate, -t);
};

auto V1 = [](int t) -> double {
    double interest_rate = 0.025;
    return std::pow(1.0 + interest_rate, -(t + 1));
};

double premium = ActuarialCalculator::ComputationTermPr(
    30,        // x: 30세 가입
    20,        // nn: 20년 보험기간
    10,        // mm: 10년 납입기간
    0,         // Sex: 남성
    100000000, // AMT: 1억원
    QPr, V0, V1
);
```

### 2. Computation
일반적인 교차함수를 계산합니다.

**Parameters:**
- `x` (int): 가입나이 (entry age)
- `nn` (int): 보험기간 (insurance period in years)
- `Qx` (function): 사망률 함수 Qx(risk_class, age) -> double
- `Qxw` (function): 실효율 함수 Qxw(age) -> double
- `V0` (function): 할인함수 V0(t) -> double

**Returns:**
- `CommutationFunctions`: 계산된 교차함수 구조체

**Example:**
```cpp
auto Qx = [](int risk_class, int age) -> double {
    return GetQxFromTable(risk_class, age);
};

auto Qxw = [](int age) -> double {
    // 실효율 (lapse rate)
    if (age < 30) return 0.05;
    if (age < 40) return 0.03;
    return 0.02;
};

auto V0 = [](int t) -> double {
    double interest_rate = 0.025;
    return std::pow(1.0 + interest_rate, -t);
};

CommutationFunctions cf = ActuarialCalculator::Computation(
    30,  // x: 30세 가입
    20,  // nn: 20년 보험기간
    Qx, Qxw, V0
);

// 결과 사용
double lx_30 = cf.lx[30];
double Dx_30 = cf.Dx[30];
double Nx_30 = cf.Nx[30];
```

### 3. MxStep
Mx 계산을 역순으로 수행합니다 (내부 함수로 Computation에서 자동 호출됨).

## CommutationFunctions Structure

교차함수 구조체는 다음 맵들을 포함합니다:

- `lx`: 생존자수 (number of survivors)
- `lpx`: 보험료 납입자수 (number of premium payers)
- `Dx`: 할인된 생존자수 (discounted survivors)
- `Dpx`: 할인된 납입자수 (discounted premium payers)
- `Cx`: 사망보험금 현가 (present value of death benefit)
- `C0x`: 순수 사망보험금 현가 (pure present value of death benefit)
- `Nx`: Dx의 누적합 (cumulative sum of Dx)
- `Npx`: Dpx의 누적합 (cumulative sum of Dpx)
- `Mx`: Cx의 누적합 (cumulative sum of Cx)
- `M0x`: C0x의 누적합 (cumulative sum of C0x)

## Integration with DataProcessor

이 모듈은 DataProcessor의 데이터 구조와 함께 사용할 수 있습니다:

```cpp
#include "Calculation/actuarial_calculator.h"
#include "DataProcessor/qx_data_structure.h"
#include "DataProcessor/code_data_structure.h"

// QxTable 데이터를 사용한 QPr 함수
auto QPr = [&qx_map](int risk_class, int sex, int age) -> double {
    for (const auto& [key, qx_vector] : qx_map) {
        for (const auto& qx_entry : qx_vector) {
            if (qx_entry->risk_class == risk_class && qx_entry->age == age) {
                return (sex == 0) ? qx_entry->male : qx_entry->female;
            }
        }
    }
    return 0.0;
};
```

## Variable Mapping from VBA to C++

VBA 변수들의 C++ 매핑:

| VBA Variable | C++ Equivalent | Description |
|--------------|----------------|-------------|
| lx(A) | cf.lx[A] | 생존자수 |
| lpx(A) | cf.lpx[A] | 납입자수 |
| Dx(A) | cf.Dx[A] | 할인된 생존자수 |
| Dpx(A) | cf.Dpx[A] | 할인된 납입자수 |
| Cx(A) | cf.Cx[A] | 사망보험금 현가 |
| C0x(A) | cf.C0x[A] | 순수 사망보험금 현가 |
| Nx(B) | cf.Nx[B] | Dx 누적합 |
| Npx(B) | cf.Npx[B] | Dpx 누적합 |
| Mx(B) | cf.Mx[B] | Cx 누적합 |
| M0x(B) | cf.M0x[B] | C0x 누적합 |
| QPr(0, Sex, A) | QPr(0, Sex, A) | 위험률 함수 |
| V0(xt) | V0(xt) | 할인함수 (만기) |
| V1(xt) | V1(xt) | 할인함수 (1년 후) |
| Qx(0, A) | Qx(0, A) | 사망률 함수 |
| Qxw(A) | Qxw(A) | 실효율 함수 |

## Usage in Your Project

1. Include the header:
```cpp
#include "Calculation/actuarial_calculator.h"
```

2. Define your callback functions using data from DataProcessor

3. Call the calculation functions

4. Use the results in your insurance calculations

## Notes

- 모든 함수는 `ActuarialCalculator` 네임스페이스 안에 있습니다
- 콜백 함수(QPr, V0, V1, Qx, Qxw)는 std::function으로 전달됩니다
- 교차함수는 std::map을 사용하여 효율적으로 저장됩니다
- 예시 코드는 `example_usage.cc`를 참조하세요

## Build

CMakeLists.txt에 이미 포함되어 있습니다:
```cmake
file(GLOB CALCULATION_SOURCES
src/Calculation/*.cc
src/Calculation/*.h
)
```

프로젝트를 빌드하면 자동으로 포함됩니다:
```bash
cmake --build Debug
```
