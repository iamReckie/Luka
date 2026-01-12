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
#ifndef SRC_CALCULATION_ACTUARIAL_CALCULATOR_H_
#define SRC_CALCULATION_ACTUARIAL_CALCULATOR_H_

#include <functional>
#include <map>
#include <utility>

// Forward declaration
struct InsuranceResult;

namespace ActuarialCalculator {

// Function types for actuarial calculations
using QxFunction = std::function<double(int, int, int)>;  // QPr(risk_class, sex, age)
using VFunction = std::function<double(int)>;             // V0(t), V1(t) - discount function
using QFunction = std::function<double(int, int)>;        // Qx(risk_class, age)
using QwFunction = std::function<double(int)>;            // Qxw(age) - lapse rate

// Commutation functions structure
struct CommutationFunctions {
  std::map<int, double> lx;   // Number of survivors
  std::map<int, double> lpx;  // Number of premium payers
  std::map<int, double> Dx;   // Discounted survivors
  std::map<int, double> Dpx;  // Discounted premium payers
  std::map<int, double> Cx;   // Present value of death benefit
  std::map<int, double> C0x;  // Pure present value of death benefit
  std::map<int, double> Nx;   // Cumulative sum of Dx
  std::map<int, double> Npx;  // Cumulative sum of Dpx
  std::map<int, double> Mx;   // Cumulative sum of Cx
  std::map<int, double> M0x;  // Cumulative sum of C0x
};

// Calculate term insurance risk premium
// @param insurance_result: InsuranceResult containing x, nn, mm, AMT
// @param Sex: Gender (0=male, 1=female)
// @param QPr: Mortality rate function
// @param V0: Discount function at maturity
// @param V1: Discount function after 1 year
// @return: Monthly premium
double ComputationTermPr(const int& x, const int& nn, const int& mm, const int& AMT, const int& Sex,
                         QxFunction QPr, VFunction V0, VFunction V1);

// Calculate general commutation functions
// @param insurance_result: InsuranceResult containing x, nn
// @param Qx: Mortality rate function
// @param Qxw: Lapse rate function
// @param V0: Discount function
// @param V1: Discount function after 1 year
// @return: Commutation functions structure
CommutationFunctions Computation(const int& x, const int& nn,
                                 QFunction Qx, QwFunction Qxw, VFunction V0, VFunction V1);

// Calculate Mx step by step (reverse accumulation)
// @param x: Entry age
// @param nn: Insurance period
// @param cf: Commutation functions structure
void MxStep(const int& x, const int& nn, CommutationFunctions& cf);

// Distribute Qx values from input table
// Sub Qx_Distribution() in VBA
// @param M_count: Number of mortality tables (M(Dnum) in VBA)
// @param Qx_in: Function to retrieve Qx values - Qx_in(dnum, table_idx, sex, age)
// @param Dnum: Table number
// @param Sex: Gender (0=male, 1=female)
// @return: 2D map of Qx values [table_idx][age]
std::map<int, std::map<int, double>> QxDistribution(int m_count,
                                                    std::function<double(int, int, int, int)> qx_in,
                                                    int dnum,
                                                    int sex);

// Distribute lapse and reinstatement rates (HJY)
// Sub HJY_Distribution() in VBA
// @param x: Entry age
// @param nn: Insurance period
// @param mm: Premium payment period
// @param Wx_B: Withdrawal rate table function - Wx_B(period, year)
// @param JHJ_Flag: Lapse processing flag (< 2 means clear arrays)
// @return: Pair of maps (Qxw, Rxw) where Qxw is lapse rate, Rxw is reinstatement rate
std::pair<std::map<int, double>, std::map<int, double>> HJYDistribution(
    int x, int nn, int mm,
    std::function<double(int, int)> Wx_B,
    int JHJ_Flag);

// Distribute standard lapse and reinstatement rates (HJY_STD)
// Sub HJY_STD_Distribution() in VBA
// @param x: Entry age
// @param nn: Insurance period
// @param am: Standard payment period
// @param Wx_B: Withdrawal rate table function - Wx_B(period, year)
// @param JHJ_Flag: Lapse processing flag (< 2 means clear arrays)
// @return: Pair of maps (Qxw, Rxw) where Qxw is lapse rate, Rxw is reinstatement rate
std::pair<std::map<int, double>, std::map<int, double>> HJYSTDDistribution(
    int x, int nn, int am,
    std::function<double(int, int)> Wx_B,
    int JHJ_Flag);

}  // namespace ActuarialCalculator

#endif  // SRC_CALCULATION_ACTUARIAL_CALCULATOR_H_
