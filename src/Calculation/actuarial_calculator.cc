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

double BenefitSUMx(const int& nn,
                   const int& w,
                   const int& dnum,
                   const int& x,
                   const std::vector<std::vector<double>>& pay,
                   const std::vector<std::vector<double>>& fst,
                   const std::vector<double>& c0x,
                   const std::vector<double>& c1x,
                   const std::vector<double>& m0x,
                   const std::vector<double>& m1x) {
  // Sub Benefit_SUMx() in VBA
  // SUMx = Pay(Dnum,0)*(Fst(Dnum,0)*C0x(x) + M0x(x+1) - [M0x(x+nn)])
  //      + Pay(Dnum,1)*(Fst(Dnum,1)*C1x(x) + M1x(x+1) - [M1x(x+nn)])
  // nn < w : finite insurance period (M0x/M1x terms include terminal subtraction)
  // nn >= w: whole life (no terminal subtraction)
  double sumX = 0.0;
  if (nn < w) {
    sumX = pay[dnum][0] * (fst[dnum][0] * c0x[x] + m0x[x + 1] - m0x[x + nn]) +
           pay[dnum][1] * (fst[dnum][1] * c1x[x] + m1x[x + 1] - m1x[x + nn]);
  } else {
    sumX = pay[dnum][0] * (fst[dnum][0] * c0x[x] + m0x[x + 1]) +
           pay[dnum][1] * (fst[dnum][1] * c1x[x] + m1x[x + 1]);
  }
  return sumX;
}

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
                    std::vector<double>& w_mx) {
  for (int i = 0; i < nn; ++i) {
    if (jhj_flag == 2) {
      w_cx[x + i] = rxw[x + i] * ((tvn_std_pj[sex][i] + tvn_std_pj[sex][i + 1]) / 2.0 / amt) * lx[x + i] * qxw[x + i] * (1 - 0.5 * qx[0][x + i] * V1Calculation(i));
    } else if (jhj_flag == 3) {
      w_cx[x + i] = rxw[x + i] * ((tvn_pj[sex][i] + tvn_pj[sex][i + 1]) / 2.0 / amt) * lx[x + i] * qxw[x + i] * (1 - 0.5 * qx[0][x + i] * V1Calculation(i));
    } else {
      w_cx[x + i] = 0.0;
    }
  }
  for (int i = x + nn; i > x; --i) {
    if (i == x + nn) {
      w_mx[i] = w_cx[i];
    } else {
      w_mx[i] = w_mx[i + 1] + w_cx[i];
    }
  }
  return w_mx[x] - w_mx[x + nn];
}

// Sub Benefit_SUMxt() in VBA
double BenefitSUMxt(const int& t,
                    const int& nn1,
                    const int& nn,
                    const int& dnum,
                    const int& x,
                    const double& sumx,
                    const std::vector<double>& m0x,
                    const std::vector<double>& m1x,
                    const std::vector<std::vector<double>>& pay) {
  if (t == 0) {
    return sumx;
  }
  if (nn1 < 110) {
    return pay[dnum][0] * (m0x[x + t] - m0x[x + nn]) + pay[dnum][1] * (m1x[x + t] - m1x[x + nn]);
  } else {
    return pay[dnum][0] * m0x[x + t] + pay[dnum][1] * m1x[x + t];
  }
}

double BankersRound(double value, int decimal_places) {
  double factor = std::pow(10.0, decimal_places);
  double scaled = value * factor;
  double floored = std::floor(scaled);
  double diff = scaled - floored;
  if (diff < 0.5) {
    return floored / factor;
  }
  if (diff > 0.5) {
    return (floored + 1.0) / factor;
  }
  return ((std::fmod(floored, 2.0) == 0.0) ? floored : floored + 1.0) / factor;
}

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
        std::vector<std::vector<double>>& tVn) {
  std::vector<double> adjusted_sp;
  int kk = 0;
  double w_sum_x{0.0}, sum_x{0.0}, w_sumxt{0.0}, sumxt{0.0};
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
    sum_x = BenefitSUMx(nn, w, dnum, x, pay, fst, c0x, c1x, m0x, m1x);
    if (jhj_flag >= 2) {
      w_sum_x = WBenefitSUMx(x, nn, jhj_flag, amt, sex, qxw, rxw, lx, qx, tvn_std_pj, tvn_pj, w_cx, w_mx);
    } else {
      w_sum_x = 0;
    }
    if (jhj_flag < 3) {
      adjusted_sp.emplace_back((sum_x + w_sum_x) / (npx[x] - npx[x + am]));
      std_np[sex] = BankersRound(adjusted_sp[sex] * amt, 0);
    }
    nnx[sex][nb] = kk * (npx[x] - npx[x + mm] - (kk - 1) / ((2 * kk) * (dpx[x] - dpx[x + mm])));
    np1[sex][nb] = (sum_x + w_sum_x) / nnx[sex][nb];
    np[sex][nb] = BankersRound(np1[sex][nb] * amt, 0);

    ss1x[sex][nb] = (beta2 / kk) + beta3 * (nx[x + mm] - nx[x + nn]) / nnx[sex][nb];
    ss2x[sex][nb] = 1 - alp * dx[x] * kk / nnx[sex][nb] - beta1 - gamma;

    gp1[sex][nb] = (np1[sex][nb] + ss1x[sex][nb]) / ss2x[sex][nb];
    gp[sex][nb] = BankersRound(gp1[sex][nb] * amt, 0);
  }
  np_beta[sex] = np1[sex][1] + beta3 * (nx[x + mm] - nx[x + nn]) / (npx[x] - npx[x + mm]);
  applied_alpha[sex] = BankersRound(alp * 12 * gp[sex][4], 2);
  standard_alpha[sex] = BankersRound((std_np[sex] * 0.05 * am + std_srt[dnum] * 10 / (1000 * amt)) * 1, 2);

  for (int i = 0; i < nn; ++i) {
    sumxt = BenefitSUMxt(t, nn1, nn, dnum, x, sum_x, m0x, m1x, pay);
    if (jhj_flag >= 2) {
      w_sumxt = w_mx[x + i] - w_mx[x + nn];
    } else {
      w_sumxt = 0;
    }

    if (i < mm) {
      fpi = np_beta[sex] * (npx[x + i] - npx[x + mm]);
      tVnn = (sumxt + w_sumxt + beta3 * (nx[x + mm] - nx[x + nn]) - fpi) / dx[x + i];
    } else {
      tVnn = (sumxt + w_sumxt + beta3 * (nx[x + i] - nx[x + nn])) / dx[x + i];
    }
    sum_t[sex][i] = sumxt;
    w_sum_t[sex][i] = w_sumxt;
    tVn[sex][i] = BankersRound(tVnn * amt, 0);
  }
}

void HjyDistribution(std::vector<double>& qxw,
                     std::vector<double>& rxw,
                     const std::vector<std::vector<double>>& wx_b,
                     const int& mm,
                     const int& x,
                     const int& nn,
                     const int& jhj_flag) {
  for (int i = 0; i < nn; ++i) {
    qxw[x + i] = (i < 45) ? wx_b[(mm < 31 ? mm : 30)][i] : wx_b[(mm < 31 ? mm : 30)][44];
    rxw[x + i] = (i < mm) ? 0.0 : 0.5;
  }
  if (jhj_flag < 2) {
    qxw.clear();
    rxw.clear();
  }
}

void HjyStdDistribution(std::vector<double>& qxw,
                        std::vector<double>& rxw,
                        const std::vector<std::vector<double>>& wx_b,
                        const int& am,
                        const int& x,
                        const int& nn,
                        const int& jhj_flag) {
  for (int i = 0; i < nn; ++i) {
    qxw[x + i] = (i < 45) ? wx_b[(am < 31 ? am : 30)][i] : wx_b[(am < 31 ? am : 30)][44];
    rxw[x + i] = (i < am) ? 0.0 : 0.5;
  }
  if (jhj_flag < 2) {
    qxw.clear();
    rxw.clear();
  }
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

// Distribute Qx values from input table
// Sub Qx_Distribution() in VBA
std::map<int, std::map<int, double>> QxDistribution(const int& m_count,
                                                    const double qx_in[][2][120],
                                                    const int& sex) {
  std::map<int, std::map<int, double>> current_qx;

  // Loop through mortality tables: ii = 0 to M(Dnum) - 1
  for (int ii = 0; ii < m_count; ++ii) {
    // Loop through ages: jj = 0 to 112
    for (int jj = 0; jj <= 112; ++jj) {
      // Qx(ii, jj) = Qx_in(ii, Sex, jj)
      current_qx[ii][jj] = qx_in[ii][sex][jj];
    }
  }

  return current_qx;
}

}  // namespace ActuarialCalculator
