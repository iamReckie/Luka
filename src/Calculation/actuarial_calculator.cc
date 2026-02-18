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
#include <memory>

#include "DataProcessor/insurance_output_data_structure.h"

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

double V0Calculation(double n) {
  return 1.0 / std::pow(1.0 + 0.025, n);
}

double V1Calculation(double n) {
  return 1.0 / std::pow(1.0 + 0.025, n + 0.5);
}

void PV(const int& nn1,
        const int& w,
        const double& current_pay1,
        const double& current_pay2,
        const double& current_fst1,
        const double& current_fst2,
        const double& current_C0x,
        const double& current_C1x,
        const double& current_M0x1,
        const double& current_M0x2,
        const double& current_M1x1,
        const double& current_M1x2,
        const int& JHJ_Flag) {
  int kk = 0;
  for (int nb = 1; nb < 5; ++nb) {
    switch (nb) {
      case 1:
        kk = 1;
        break;
      case 2:
        kk = 2;
        break;
      case 3:
        kk = 4;
        break;
      case 4:
        kk = 12;
        break;
      default:
        break;
    }
    double SUMx = Benefit_SUMx(nn1, w, current_pay1, current_pay2, current_fst1, current_fst2, current_C0x, current_C1x, current_M0x1, current_M0x2, current_M1x1, current_M1x2);
    (void)SUMx;
  }
}

double Benefit_SUMx(const int& nn1,
                    const int& w,
                    const double& current_pay1,
                    const double& current_pay2,
                    const double& current_fst1,
                    const double& current_fst2,
                    const double& current_C0x,
                    const double& current_C1x,
                    const double& current_M0x1,
                    const double& current_M0x2,
                    const double& current_M1x1,
                    const double& current_M1x2) {
  // Sub Benefit_SUMx() in VBA
  // SUMx = Pay(Dnum,0)*(Fst(Dnum,0)*C0x(x) + M0x(x+1) - [M0x(x+nn)])
  //      + Pay(Dnum,1)*(Fst(Dnum,1)*C1x(x) + M1x(x+1) - [M1x(x+nn)])
  // nn1 < w : finite insurance period (M0x/M1x terms include terminal subtraction)
  // nn1 >= w: whole life (no terminal subtraction)
  double sumX = 0.0;
  if (nn1 < w) {
    sumX = current_pay1 * (current_fst1 * current_C0x + current_M0x1 - current_M0x2) +
           current_pay2 * (current_fst2 * current_C1x + current_M1x1 - current_M1x2);
  } else {
    sumX = current_pay1 * (current_fst1 * current_C0x + current_M0x1) +
           current_pay2 * (current_fst2 * current_C1x + current_M1x1);
  }
  return sumX;
}

void MxStep(std::shared_ptr<InsuranceOutput>& output_ptr, const int& nn, const int& x) {
  // Backward loop: B = x + nn to x
  for (int B = x + nn; B >= x; --B) {
    int idx = B - x;
    if (B == x + nn) {
      output_ptr->Nx.push_back(output_ptr->Dx[idx]);
      output_ptr->Npx.push_back(output_ptr->Dpx[idx]);
      output_ptr->M0x.push_back(output_ptr->C0x[idx]);
    } else {
      output_ptr->Nx.insert(output_ptr->Nx.begin(), output_ptr->Nx.front() + output_ptr->Dx[idx]);
      output_ptr->Npx.insert(output_ptr->Npx.begin(), output_ptr->Npx.front() + output_ptr->Dpx[idx]);
      output_ptr->M0x.insert(output_ptr->M0x.begin(), output_ptr->M0x.front() + output_ptr->C0x[idx]);
    }
  }
}

void Computation(std::shared_ptr<InsuranceOutput>& output_ptr, const int& nn, const int& x) {
  for (int i = x; i <= x + nn; ++i) {
    int xt = i - x;
    if (xt == 0) {
      output_ptr->lx.push_back(100000.0);
      output_ptr->lpx.push_back(100000.0);
    } else {
      double lx_val = output_ptr->lx.back();
      output_ptr->lx.push_back(lx_val);
      double lpx_val = output_ptr->lpx.back();
      output_ptr->lpx.push_back(lpx_val);
    }
    output_ptr->Dx.push_back(output_ptr->lx.back() * V0Calculation(xt));
    output_ptr->Dpx.push_back(output_ptr->lpx.back() * V0Calculation(xt));
    output_ptr->C0x.push_back(output_ptr->lx.back() * output_ptr->Qx[0][i] * V1Calculation(xt) * (1.0 - output_ptr->Qxw[i] / 2.0));
  }

  // Call Mx_Step
  MxStep(output_ptr, nn, x);
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
  std::map<int, std::map<int, double>> current_qx;

  // Loop through mortality tables: ii = 0 to M(Dnum) - 1
  for (int ii = 0; ii < m_count; ++ii) {
    // Loop through ages: jj = 0 to 112
    for (int jj = 0; jj <= 112; ++jj) {
      // Qx(ii, jj) = Qx_in(Dnum, ii, Sex, jj)
      current_qx[ii][jj] = qx_in(dnum, ii, sex, jj);
    }
  }

  return current_qx;
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
