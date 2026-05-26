# Actuarial Calculator - Usage Examples

## Overview
This document provides usage examples for all functions in the Actuarial Calculator module.

## Functions Implemented

### 1. ComputationTermPr - Term Insurance Risk Premium
Calculate monthly premium for term insurance.

**VBA Original**: `Sub Computation_Term_Pr()`

**C++ Signature**:
```cpp
double ComputationTermPr(
    const int& x,      // Entry age
    const int& nn,     // Insurance period
    const int& mm,     // Premium payment period
    const int& AMT,    // Insurance amount
    const int& Sex,    // Gender (0=male, 1=female)
    QxFunction QPr,    // Mortality rate function
    VFunction V0,      // Discount function at maturity
    VFunction V1);     // Discount function after 1 year
```

**Usage Example**:
```cpp
#include "Calculation/actuarial_calculator.h"

// Define callback functions
auto QPr = [](int risk_class, int sex, int age) -> double {
    // Get mortality rate from your data
    return GetMortalityRate(risk_class, sex, age);
};

auto V0 = [](int t) -> double {
    double interest_rate = 0.025;
    return std::pow(1.0 + interest_rate, -t);
};

auto V1 = [](int t) -> double {
    double interest_rate = 0.025;
    return std::pow(1.0 + interest_rate, -(t + 1));
};

// Calculate premium
double premium = ActuarialCalculator::ComputationTermPr(
    30,         // x: 30 years old
    20,         // nn: 20 year term
    10,         // mm: 10 year payment period
    100000000,  // AMT: 100 million KRW
    0,          // Sex: male
    QPr, V0, V1
);
```

---

### 2. Computation - General Commutation Functions
Calculate commutation functions for actuarial calculations.

**VBA Original**: `Sub Computation()`

**C++ Signature**:
```cpp
CommutationFunctions Computation(
    const int& x,      // Entry age
    const int& nn,     // Insurance period
    QFunction Qx,      // Mortality rate function
    QwFunction Qxw,    // Lapse rate function
    VFunction V0,      // Discount function
    VFunction V1);     // Discount function after 1 year
```

**Usage Example**:
```cpp
auto Qx = [](int risk_class, int age) -> double {
    return GetMortalityRate(risk_class, age);
};

auto Qxw = [](int age) -> double {
    // Lapse rate by age
    if (age < 30) return 0.05;
    if (age < 40) return 0.03;
    return 0.02;
};

auto V0 = [](int t) -> double {
    return std::pow(1.025, -t);
};

auto V1 = [](int t) -> double {
    return std::pow(1.025, -(t+1));
};

auto cf = ActuarialCalculator::Computation(30, 20, Qx, Qxw, V0, V1);

// Access results
double survivors = cf.lx[30];
double discounted = cf.Dx[30];
double cumulative = cf.Nx[30];
```

---

### 3. QxDistribution - Mortality Rate Distribution
Distribute mortality rates from input table to working array.

**VBA Original**: `Sub Qx_Distribution()`

**C++ Signature**:
```cpp
std::map<int, std::map<int, double>> QxDistribution(
    int M_count,                                      // Number of mortality tables
    std::function<double(int, int, int, int)> Qx_in, // Qx input function
    int Dnum,                                         // Table number
    int Sex);                                         // Gender
```

**Usage Example**:
```cpp
// Define Qx_in function to retrieve mortality rates
auto Qx_in = [&qx_table_map](int dnum, int table_idx, int sex, int age) -> double {
    // Retrieve from your QxTable data structure
    auto it = qx_table_map.find(dnum);
    if (it != qx_table_map.end()) {
        // Access the appropriate table and return rate
        return GetRateFromTable(it->second, table_idx, sex, age);
    }
    return 0.0;
};

// Get M_count from CodeTable
int M_count = code_table->M_count;
int Dnum = code_table->dnum;
int Sex = 0;  // male

// Distribute Qx values
auto Qx = ActuarialCalculator::QxDistribution(M_count, Qx_in, Dnum, Sex);

// Access distributed values
double qx_value = Qx[0][30];  // table 0, age 30
```

**VBA Code Reference**:
```vba
' Original VBA
For ii = 0 To M(Dnum) - 1
    For jj = 0 To 112
        Qx(ii, jj) = Qx_in(Dnum, ii, Sex, jj)
    Next
Next
```

---

### 4. HJYDistribution - Lapse Rate Distribution
Distribute lapse and reinstatement rates for policy year.

**VBA Original**: `Sub HJY_Distribution()`

**C++ Signature**:
```cpp
std::pair<std::map<int, double>, std::map<int, double>> HJYDistribution(
    int x,                                  // Entry age
    int nn,                                 // Insurance period
    int mm,                                 // Premium payment period
    std::function<double(int, int)> Wx_B,  // Withdrawal rate table
    int JHJ_Flag);                          // Lapse flag
```

**Usage Example**:
```cpp
// Define Wx_B function to retrieve withdrawal rates
auto Wx_B = [&withdrawal_table](int period, int year) -> double {
    // Get withdrawal rate from table
    return GetWithdrawalRate(withdrawal_table, period, year);
};

int x = 30;        // Entry age
int nn = 20;       // Insurance period
int mm = 10;       // Payment period
int JHJ_Flag = 2;  // Enable lapse processing

// Get lapse and reinstatement rates
auto [Qxw, Rxw] = ActuarialCalculator::HJYDistribution(x, nn, mm, Wx_B, JHJ_Flag);

// Access results
double lapse_rate_35 = Qxw[35];           // Lapse rate at age 35
double reinstate_rate_35 = Rxw[35];       // Reinstatement rate at age 35
```

**VBA Code Reference**:
```vba
' Original VBA
For jj = 0 To nn
    Qxw(x + jj) = IIf(jj < 45, Wx_B(IIf(mm < 31, mm, 30), jj), 
                                Wx_B(IIf(mm < 31, mm, 30), 44))
    Rxw(x + jj) = IIf(jj < mm, 0, 0.5)
Next jj

If JHJ_Flag < 2 Then Erase Qxw, Rxw
```

**Logic Explanation**:
- If `JHJ_Flag < 2`: Returns empty maps (no lapse processing)
- For each year `jj` from 0 to `nn`:
  - `Qxw[x+jj]`: Withdrawal rate from table, capped at year 44 and period 30
  - `Rxw[x+jj]`: Reinstatement rate is 0 during payment period, 0.5 after

---

### 5. HJYSTDDistribution - Standard Lapse Rate Distribution
Similar to HJYDistribution but uses standard payment period (`am` instead of `mm`).

**VBA Original**: `Sub HJY_STD_Distribution()`

**C++ Signature**:
```cpp
std::pair<std::map<int, double>, std::map<int, double>> HJYSTDDistribution(
    int x,                                  // Entry age
    int nn,                                 // Insurance period
    int am,                                 // Standard payment period
    std::function<double(int, int)> Wx_B,  // Withdrawal rate table
    int JHJ_Flag);                          // Lapse flag
```

**Usage Example**:
```cpp
auto Wx_B = [&withdrawal_table](int period, int year) -> double {
    return GetWithdrawalRate(withdrawal_table, period, year);
};

int x = 30;
int nn = 20;
int am = 15;       // Standard payment period (different from mm)
int JHJ_Flag = 2;

auto [Qxw_std, Rxw_std] = ActuarialCalculator::HJYSTDDistribution(
    x, nn, am, Wx_B, JHJ_Flag);

// Use for standard premium calculations
double std_lapse_rate = Qxw_std[35];
```

---

## Integration with DataProcessor

### Using with InsuranceResult

```cpp
#include "Calculation/actuarial_calculator.h"
#include "DataProcessor/insurance_result_data_structure.h"
#include "DataProcessor/code_data_structure.h"
#include "DataProcessor/qx_data_structure.h"

void CalculateInsurancePremium(
    const std::shared_ptr<InsuranceResult>& insurance_result,
    const std::shared_ptr<CodeTable>& code_table,
    const QxDataStructure::QxTableMap& qx_map) {
    
    // Extract parameters
    int x = insurance_result->x;
    int nn = insurance_result->nn;
    int mm = insurance_result->mm;
    int AMT = insurance_result->AMT;
    int Dnum = code_table->dnum;
    int M_count = code_table->M_count;
    int Sex = 0;  // From your input
    
    // Define Qx_in using QxTable data
    auto Qx_in = [&qx_map](int dnum, int table_idx, int sex, int age) -> double {
        for (const auto& [key, qx_vector] : qx_map) {
            for (const auto& qx_entry : qx_vector) {
                if (qx_entry->age == age) {
                    return (sex == 0) ? qx_entry->male : qx_entry->female;
                }
            }
        }
        return 0.0;
    };
    
    // Distribute mortality rates
    auto Qx = ActuarialCalculator::QxDistribution(M_count, Qx_in, Dnum, Sex);
    
    // Define QPr using distributed Qx
    auto QPr = [&Qx](int risk_class, int sex, int age) -> double {
        return Qx[risk_class][age];
    };
    
    // Define discount functions
    double interest_rate = 0.025;
    auto V0 = [interest_rate](int t) -> double {
        return std::pow(1.0 + interest_rate, -t);
    };
    auto V1 = [interest_rate](int t) -> double {
        return std::pow(1.0 + interest_rate, -(t + 1));
    };
    
    // Calculate premium
    double premium = ActuarialCalculator::ComputationTermPr(
        x, nn, mm, AMT, Sex, QPr, V0, V1);
    
    Logger::Log(L"Calculated Premium: %.0f KRW\n", premium);
}
```

---

## Variable Mapping from VBA to C++

| VBA Variable | C++ Type | Description |
|-------------|----------|-------------|
| `x` | `int` | Entry age |
| `nn` | `int` | Insurance period (years) |
| `mm` | `int` | Premium payment period (years) |
| `am` | `int` | Standard payment period (years) |
| `Sex` | `int` | Gender (0=male, 1=female) |
| `AMT` | `int` | Insurance amount |
| `Dnum` | `int` | Table number |
| `M(Dnum)` | `int M_count` | Number of mortality tables |
| `Qx(ii, jj)` | `std::map<int, std::map<int, double>>` | Mortality rates [table][age] |
| `Qxw(age)` | `std::map<int, double>` | Lapse rate by age |
| `Rxw(age)` | `std::map<int, double>` | Reinstatement rate by age |
| `lx(A)` | `cf.lx[A]` | Number of survivors at age A |
| `Dx(A)` | `cf.Dx[A]` | Discounted survivors |
| `Nx(B)` | `cf.Nx[B]` | Cumulative Dx |
| `Mx(B)` | `cf.Mx[B]` | Cumulative Cx |
| `JHJ_Flag` | `int` | Lapse processing flag |

---

## Notes

1. **Function callbacks**: All rate functions (QPr, Qx, Wx_B) are passed as `std::function` callbacks
2. **No global variables**: All data is passed as parameters or returned from functions
3. **Type safety**: Using `std::map` instead of VBA arrays for safer access
4. **Return values**: Functions return results instead of modifying global state
5. **Flag behavior**: `JHJ_Flag < 2` returns empty maps (equivalent to VBA's `Erase`)

---

## Build Instructions

The Calculation module is automatically included when building the project:

```bash
cd /home/iamreckie/Luka
cmake --build Debug
```

All functions are in the `ActuarialCalculator` namespace.
