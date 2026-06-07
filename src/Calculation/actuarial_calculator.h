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

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "DataProcessor/sratio_data_structure.h"
// Forward declaration
struct InsuranceResult;
struct InsuranceOutput;

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

// Overloaded version for InsuranceOutput pointer
void MxStep(std::shared_ptr<InsuranceOutput>& output_ptr, const int& nn, const int& x);

// Overloaded Computation for InsuranceOutput pointer
void Computation(std::shared_ptr<SRatioTable>& sratio_table);

// V0 discount calculation (present value factor)
double V0Calculation(double n);

// V1 discount calculation (present value factor with half-year adjustment)
double V1Calculation(double n);

void Pv(std::shared_ptr<SRatioTable> sratio_table);

double BenefitSUMx(const int& nn,
                   const int& w,
                   const int& dnum,
                   const int& x,
                   const std::vector<std::vector<double>>& pay,
                   const std::vector<std::vector<double>>& fst,
                   const std::vector<double>& c0x,
                   const std::vector<double>& c1x,
                   const std::vector<double>& m0x,
                   const std::vector<double>& m1x);

double WBenefitSUMx(const int& x,
                    const int& nn,
                    const int& jhj_flag,
                    const int& amt,
                    const int& sex,
                    const std::vector<double>& qxw,
                    const std::vector<double>& rxw,
                    const std::vector<double>& lx,
                    const std::vector<std::vector<double>>& qx,
                    const std::vector<std::vector<double>>& tvn_std_pj,
                    const std::vector<std::vector<double>>& tvn_pj,
                    std::vector<double>& w_cx,
                    std::vector<double>& w_mx);

double BenefitSUMxt(const int& t,
                    const int& nn1,
                    const int& nn,
                    const int& dnum,
                    const int& x,
                    const double& sumx,
                    const std::vector<double>& m0x,
                    const std::vector<double>& m1x,
                    const std::vector<std::vector<double>>& pay);

void PV(const int& nn1,
        const int& nn,
        const int& w,
        const int& am,
        const int& x,
        const int& sex,
        const int& amt,
        const int& mm,
        const int& alp,
        const int& jhj_flag,
        const int& dnum,
        const int& t,
        const double& beta1,
        const double& beta2,
        const double& beta3,
        const double& gamma,
        const std::vector<double>& nx,
        const std::vector<double>& dx,
        const std::vector<double>& npx,
        const std::vector<double>& dpx,
        const std::vector<double>& std_srt,
        const std::vector<double>& c0x,
        const std::vector<double>& c1x,
        const std::vector<double>& m0x,
        const std::vector<double>& m1x,
        const std::vector<double>& lx,
        const std::vector<double>& rxw,
        const std::vector<double>& qxw,
        const std::vector<std::vector<double>>& pay,
        const std::vector<std::vector<double>>& fst,
        const std::vector<std::vector<double>>& tvn_std_pj,
        const std::vector<std::vector<double>>& tvn_pj,
        const std::vector<std::vector<double>>& qx,
        double& fpi,
        double& tVnn,
        std::vector<double>& std_np,
        std::vector<double>& np_beta,
        std::vector<double>& applied_alpha,
        std::vector<double>& standard_alpha,
        std::vector<double>& w_cx,
        std::vector<double>& w_mx,
        std::vector<std::vector<double>>& nnx,
        std::vector<std::vector<double>>& np1,
        std::vector<std::vector<double>>& np,
        std::vector<std::vector<double>>& ss1x,
        std::vector<std::vector<double>>& ss2x,
        std::vector<std::vector<double>>& gp,
        std::vector<std::vector<double>>& gp1,
        std::vector<std::vector<double>>& sum_t,
        std::vector<std::vector<double>>& w_sum_t,
        std::vector<std::vector<double>>& tVn);

// Distribute Qx values from input table
// Sub Qx_Distribution() in VBA
// @param m_count: Number of mortality tables (M(Dnum) in VBA)
// @param qx_in: Mortality rate table - qx_in[C1][sex][age] = Qx_in(Dnum, C1, Sex, age)
// @param sex: Gender (0=male, 1=female)
// @return: 2D map of Qx values [C1][age]
std::array<std::array<double, 5>, 120> QxDistribution(const int& m_count,
                                                      const std::array<std::array<std::array<double, 120>, 2>, 5>& qx_in,
                                                      const int& sex);

void HjyDistribution(std::vector<double>& qxw,
                     std::vector<double>& rxw,
                     const std::vector<std::vector<double>>& wx_b,
                     const int& mm,
                     const int& x,
                     const int& nn,
                     const int& jhj_flag);

void HjyStdDistribution(std::vector<double>& qxw,
                        std::vector<double>& rxw,
                        const std::vector<std::vector<double>>& wx_b,
                        const int& am,
                        const int& x,
                        const int& nn,
                        const int& jhj_flag);

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
