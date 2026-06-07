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
#ifndef SRC_DATAPROCESSOR_SRATIO_DATA_STRUCTURE_H_
#define SRC_DATAPROCESSOR_SRATIO_DATA_STRUCTURE_H_
#include <algorithm>
#include <any>
#include <array>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataProcessor/data_processor.h"
struct SRatioTable {
  std::wstring dname;
  double amt;
  int renewal;
  int sex;
  int x;
  int category;
  int real_category;
  int due;
  int nn;
  double adjust;
  double mm;
  double sratio;
  double min_s;
  bool reverse;
  // QxDistribution result: Qx[C1][age]
  std::array<std::array<double, 5>, 120> qx;
  std::array<std::array<double, 2>, 4> np{};
  std::array<std::array<double, 2>, 4> gp{};
  std::array<double, 2> np_beta;
  std::array<double, 2> std_np;
  std::array<double, 2> applied_alpha;
  std::array<double, 2> standard_alpha;
  std::array<double, 200> qxw;
  std::array<double, 200> rxw;
  std::array<double, 120> lx{};
  std::array<double, 120> lpx{};
  std::array<double, 120> dx{};
  std::array<double, 120> dpx{};
  std::array<double, 120> c0x{};
  std::array<double, 120> c1x{};
  std::array<double, 120> m0x{};
  std::array<double, 120> m1x{};
  std::array<double, 120> w_cx{};
  std::array<double, 120> w_mx{};
  std::array<double, 120> nx{};
  std::array<double, 120> npx{};
  std::array<std::array<double, 120>, 2> tVn{};         // tVn[sex][t]
  std::array<std::array<double, 120>, 2> tVn_pj{};      // tVn_PJ[sex][t]
  std::array<std::array<double, 120>, 2> tVn_std_pj{};  // tVn_STD_PJ[sex][t]
  int mhj = 0;
  int jhj_flag = 0;
  // VBA: am = Application.Min(nn, 20), w = IIf(Sex=1, 110, 112)
  int am = 0;
  int w = 0;
  int nn1 = 0;
  // VBA: Alp = Alp_in(Dnum, mm), Beta1 = Beta1_in(Dnum, mm), ...
  double alp = 0.0;
  double beta1 = 0.0;
  double beta2 = 0.0;
  double beta3 = 0.0;
  double gamma = 0.0;
  // VBA: Dnum (행마다 저장), STD_SRT(Dnum) (1차 루프 후 복사)
  int dnum = 0;
  double std_srt = 0.0;
};

class SRatioDataStructure : public IDataStructure {
 public:
  using SRatioTableMap = std::unordered_map<int, std::vector<std::shared_ptr<SRatioTable>>>;

  explicit SRatioDataStructure(std::shared_ptr<DataHelper> data_helper)
      : IDataStructure(data_helper) {}
  void ConstructDataStructure(std::any& context,
                              const std::vector<std::any>& args,
                              std::wstring& key) override;
  void MergeDataStructure(std::any& target, const std::any& source) override;
  void PrintDataStructure(const std::any& context) const override;
  std::any CreateContext() const override { return SRatioTableMap(); }

 private:
  void PostProcess(std::any& context);
  int last_dnum_ = 0;
  // VBA: Public STD_SRT(10) As Double - Dnum별 표준위험률
  // 1차 루프에서 계산 후 2차 루프(Pv) 전에 SRatioTable에 복사
  std::array<double, 11> std_srt_{};
};

#endif  // SRC_DATAPROCESSOR_SRATIO_DATA_STRUCTURE_H_
