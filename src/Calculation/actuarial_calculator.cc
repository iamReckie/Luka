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
#include "Calculation/actuarial_calculator.h"

#include <cmath>

namespace ActuarialCalculator {

// Calculate term insurance risk premium
// Sub Computation_Term_Pr() in VBA
double ComputationTermPr(const int& x, const int& nn, const int& mm, const int& AMT, const int& Sex,
                         QxFunction QPr, VFunction V0, VFunction V1) {
  CommutationFunctions cf;

  // Forward loop: A = x to x + nn
  for (int A = x; A <= x + nn; ++A) {
    int xt = A - x;

    // Calculate lx (number of survivors)
    if (A == x) {
      cf.lx[A] = 100000.0;
    } else {
      cf.lx[A] = cf.lx[A - 1] * (1.0 - QPr(0, Sex, A - 1));
    }

    // Calculate Dx (discounted survivors)
    cf.Dx[A] = cf.lx[A] * V0(xt);

    // Calculate Cx (present value of death benefit)
    cf.Cx[A] = cf.lx[A] * QPr(0, Sex, A) * V1(xt);
  }

  // Backward loop: B = x + nn to 0 (reverse accumulation)
  for (int B = x + nn; B >= 0; --B) {
    if (B == x + nn) {
      cf.Nx[B] = cf.Dx[B];
      cf.Mx[B] = cf.Cx[B];
    } else {
      cf.Nx[B] = cf.Nx[B + 1] + cf.Dx[B];
      cf.Mx[B] = cf.Mx[B + 1] + cf.Cx[B];
    }
  }

  // Calculate Term_Pr (monthly premium)
  double numerator = cf.Mx[x] - cf.Mx[x + nn];
  double denominator = 12.0 * (cf.Nx[x] - cf.Nx[x + mm] -
                               11.0 / 24.0 * (cf.Dx[x] - cf.Dx[x + mm]));
  double Term_Pr = std::round(numerator / denominator * AMT);

  return Term_Pr;
}

// Calculate general commutation functions
// Sub Computation() in VBA
CommutationFunctions Computation(const int& x, const int& nn,
                                 QFunction Qx, QwFunction Qxw, VFunction V0, VFunction V1) {
  CommutationFunctions cf;

  // Forward loop: A = x to x + nn
  for (int A = x; A <= x + nn; ++A) {
    int xt = A - x;

    // Initialize lx and lpx (survivors and premium payers)
    if (xt == 0) {
      cf.lx[A] = 100000.0;
      cf.lpx[A] = 100000.0;
    } else {
      cf.lx[A] = cf.lx[A - 1];
      cf.lpx[A] = cf.lpx[A - 1];
    }

    // Calculate Dx and Dpx (discounted survivors and premium payers)
    cf.Dx[A] = cf.lx[A] * V0(xt);
    cf.Dpx[A] = cf.lpx[A] * V0(xt);

    // Calculate C0x (pure present value of death benefit)
    cf.C0x[A] = cf.lx[A] * Qx(0, A) * V1(xt) * (1.0 - Qxw(A) / 2.0);
  }

  // Call Mx_Step for backward calculation
  MxStep(x, nn, cf);

  return cf;
}

// Calculate Mx step by step (reverse accumulation)
// Sub Mx_Step() in VBA
void MxStep(const int& x, const int& nn, CommutationFunctions& cf) {
  // Backward loop: B = x + nn to x
  for (int B = x + nn; B >= x; --B) {
    if (B == x + nn) {
      cf.Nx[B] = cf.Dx[B];
      cf.Npx[B] = cf.Dpx[B];
      cf.M0x[B] = cf.C0x[B];
    } else {
      cf.Nx[B] = cf.Nx[B + 1] + cf.Dx[B];
      cf.Npx[B] = cf.Npx[B + 1] + cf.Dpx[B];
      cf.M0x[B] = cf.M0x[B + 1] + cf.C0x[B];
    }
  }
}

// Distribute Qx values from input table
// Sub Qx_Distribution() in VBA
std::map<int, std::map<int, double>> QxDistribution(int m_count,
                                                    std::function<double(int, int, int, int)> qx_in,
                                                    int dnum,
                                                    int sex) {
  std::map<int, std::map<int, double>> Qx;

  // Loop through mortality tables: ii = 0 to M(Dnum) - 1
  for (int ii = 0; ii < m_count; ++ii) {
    // Loop through ages: jj = 0 to 112
    for (int jj = 0; jj <= 112; ++jj) {
      // Qx(ii, jj) = Qx_in(Dnum, ii, Sex, jj)
      Qx[ii][jj] = qx_in(dnum, ii, sex, jj);
    }
  }

  return Qx;
}

// Distribute lapse and reinstatement rates (HJY)
// Sub HJY_Distribution() in VBA
std::pair<std::map<int, double>, std::map<int, double>> HJYDistribution(int x, int nn, int mm,
                                                                        std::function<double(int, int)> Wx_B,
                                                                        int JHJ_Flag) {
  std::map<int, double> Qxw;  // Lapse rate
  std::map<int, double> Rxw;  // Reinstatement rate

  // If JHJ_Flag < 2, return empty maps (equivalent to Erase in VBA)
  if (JHJ_Flag < 2) {
    return {Qxw, Rxw};
  }

  // Loop: jj = 0 to nn
  for (int jj = 0; jj <= nn; ++jj) {
    // Qxw(x + jj) = IIf(jj < 45, Wx_B(IIf(mm < 31, mm, 30), jj), Wx_B(IIf(mm < 31, mm, 30), 44))
    int period = (mm < 31) ? mm : 30;
    int year = (jj < 45) ? jj : 44;
    Qxw[x + jj] = Wx_B(period, year);

    // Rxw(x + jj) = IIf(jj < mm, 0, 0.5)
    Rxw[x + jj] = (jj < mm) ? 0.0 : 0.5;
  }

  return {Qxw, Rxw};
}

// Distribute standard lapse and reinstatement rates (HJY_STD)
// Sub HJY_STD_Distribution() in VBA
std::pair<std::map<int, double>, std::map<int, double>> HJYSTDDistribution(int x, int nn, int am,
                                                                           std::function<double(int, int)> Wx_B,
                                                                           int JHJ_Flag) {
  std::map<int, double> Qxw;  // Lapse rate
  std::map<int, double> Rxw;  // Reinstatement rate

  // If JHJ_Flag < 2, return empty maps (equivalent to Erase in VBA)
  if (JHJ_Flag < 2) {
    return {Qxw, Rxw};
  }

  // Loop: jj = 0 to nn
  for (int jj = 0; jj <= nn; ++jj) {
    // Qxw(x + jj) = IIf(jj < 45, Wx_B(IIf(am < 31, am, 30), jj), Wx_B(IIf(am < 31, am, 30), 44))
    int period = (am < 31) ? am : 30;
    int year = (jj < 45) ? jj : 44;
    Qxw[x + jj] = Wx_B(period, year);

    // Rxw(x + jj) = IIf(jj < am, 0, 0.5)
    Rxw[x + jj] = (jj < am) ? 0.0 : 0.5;
  }

  return {Qxw, Rxw};
}

}  // namespace ActuarialCalculator
