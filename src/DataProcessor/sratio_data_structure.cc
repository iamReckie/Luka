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
#include "DataProcessor/sratio_data_structure.h"

#include <algorithm>
#include <any>
#include <array>
#include <memory>
#include <unordered_map>
#include <vector>

#include "Calculation/actuarial_calculator.h"
#include "DataProcessor/code_data_structure.h"
#include "DataProcessor/data_helper.h"
#include "DataProcessor/expense_data_structure.h"
#include "DataProcessor/qx_data_structure.h"
#include "Logger/logger.h"
void SRatioDataStructure::ConstructDataStructure(std::any& context, const std::vector<std::any>& args, std::wstring& key) {
  if (args.empty()) {
    PostProcess(context);
    return;
  }
  auto& sratio_table = std::any_cast<SRatioTableMap&>(context);
  std::wstring input = std::any_cast<std::wstring>(args[0]);
  int column = std::any_cast<int>(args[1]);
  int key_to_int{0};
  if (column == 1) {
    key = input;
    last_dnum_ = std::stoi(input);
    return;
  }
  auto toInt = [](const std::wstring& str) -> int { return std::stoi(str); };
  auto toDouble = [](const std::wstring& str) -> float {
    return std::stod(str);
  };
  key_to_int = toInt(key);
  auto& current_sratio_table = sratio_table[key_to_int];
  std::shared_ptr<SRatioTable> new_sratio_table;
  switch (column) {
    case 2:
      new_sratio_table = std::make_shared<SRatioTable>();
      current_sratio_table.emplace_back(new_sratio_table);
      current_sratio_table.back()->dname = input;
      break;
    case 3:
      current_sratio_table.back()->amt = toDouble(input);
      break;
    case 6:
      current_sratio_table.back()->renewal = toInt(input);
      break;
    case 7:
      current_sratio_table.back()->sex = toInt(input);
      break;
    case 8:
      current_sratio_table.back()->x = toInt(input);
      break;
    case 11:
      current_sratio_table.back()->category = toInt(input);
      break;
    case 12:
      current_sratio_table.back()->real_category = toInt(input);
      break;
    case 13:
      current_sratio_table.back()->due = toInt(input);
      break;
    case 14:
      current_sratio_table.back()->nn = toInt(input);
      break;
    case 15:
      current_sratio_table.back()->adjust = toDouble(input);
      break;
    case 16:
      current_sratio_table.back()->mm = toDouble(input);
      break;
    case 17:
      current_sratio_table.back()->sratio = toDouble(input);
      break;
    case 18:
      current_sratio_table.back()->min_s = toDouble(input);
      break;
    // case 19:
    //   current_sratio_table.back()->applied_alpha = toDouble(input);
    //   break;
    // case 20:
    //   current_sratio_table.back()->standard_alpha = toDouble(input);
    //   break;
    case 21:
      if (input == L"미초과") {
        current_sratio_table.back()->reverse = false;
      } else {
        current_sratio_table.back()->reverse = true;
      }
      break;
    default:
      break;
  }
}

void SRatioDataStructure::MergeDataStructure(std::any& target, const std::any& source) {
  auto& target_map = std::any_cast<SRatioTableMap&>(target);
  const auto& source_map = std::any_cast<const SRatioTableMap&>(source);

  for (const auto& [key, val] : source_map) {
    auto& target_vec = target_map[key];
    target_vec.insert(target_vec.end(), val.begin(), val.end());
  }
}

void SRatioDataStructure::PrintDataStructure(const std::any& context) const {
  const auto& sratio_table = std::any_cast<const SRatioTableMap&>(context);
  for (const auto& entry : sratio_table) {
    int dnum = entry.first;
    auto current_sratio_table = entry.second;
    Logger::Log(L"sratio dnum: %d\n", dnum);
    for (const auto& iter : current_sratio_table) {
      Logger::Log(L"  dname: %ls", iter->dname.c_str());
      Logger::Log(L" amt: %lf", iter->amt);
      Logger::Log(L" renewal: %d", iter->renewal);
      Logger::Log(L" sex: %d", iter->sex);
      Logger::Log(L" x: %d", iter->x);
      Logger::Log(L" category: %d", iter->category);
      Logger::Log(L" real_category: %d", iter->real_category);
      Logger::Log(L" due: %d", iter->due);
      Logger::Log(L" nn: %d", iter->nn);
      Logger::Log(L" adjust: %lf", iter->adjust);
      Logger::Log(L" mm: %lf", iter->mm);
      Logger::Log(L" sratio: %lf", iter->sratio);
      Logger::Log(L" min_s: %lf", iter->min_s);
      Logger::Log(L" applied_alpha: %lf", iter->applied_alpha);
      Logger::Log(L" standard_alpha: %lf", iter->standard_alpha);
      Logger::Log(L" reverse: %d\n", iter->reverse);
      for (int c1 = 0; c1 < 5; ++c1) {
        Logger::Log(L"  Qx[C1=%d]:", c1);
        for (int age = 0; age < 120; ++age) {
          Logger::Log(L" [%d]=%.6f", age, iter->qx[age][c1]);
        }
        Logger::Log(L"\n");
      }
    }
  }
}

void SRatioDataStructure::PostProcess(std::any& context) {
  auto& sratio_table = std::any_cast<SRatioTableMap&>(context);

  // Get Code context
  auto* code_data_any = GetDataHelper()->GetDataContext(L"Code");
  if (!code_data_any) {
    Logger::Log(L"Warning: Code context not found in SRatioDataStructure::PostProcess\n");
    return;
  }
  const auto& code_context = std::any_cast<const CodeDataContext&>(*code_data_any);

  // Process only the current row (last_key_ row just added), matching VBA's per-row logic
  auto rows_it = sratio_table.find(last_dnum_);
  if (rows_it == sratio_table.end() || rows_it->second.empty()) {
    return;
  }

  auto code_it = std::find_if(code_context.code_table.begin(), code_context.code_table.end(),
                              [&](const auto& pair) { return pair.second->dnum == last_dnum_; });
  if (code_it == code_context.code_table.end()) {
    Logger::Log(L"Warning: CodeTable not found for sratio key=%d\n", last_dnum_);
    return;
  }
  const auto* code_table = code_it->second.get();

  // Get Qx context: select sheet based on qx_ku (0="Qx", 1="Qx1", ...)
  // VBA: Case 0 → Qx_Table, Case 1 → Qx_Table1
  std::wstring qx_sheet_name = (code_table->qx_ku == 0)
                                   ? L"Qx"
                                   : L"Qx" + std::to_wstring(code_table->qx_ku);
  // VBA: Select Case Qx_Ku(Dnum)
  Logger::Log(L"[Qx_SetUp] Dnum=%d, Qx_Ku(Dnum)=%d\n", last_dnum_, code_table->qx_ku);
  if (code_table->qx_ku == 0) {
    Logger::Log(L"  \u2192 Selected Case 0 (Qx sheet)\n");
  } else {
    Logger::Log(L"  \u2192 Selected Case %d (Qx%d sheet)\n", code_table->qx_ku, code_table->qx_ku);
  }
  auto* qx_data_any = GetDataHelper()->GetDataContext(qx_sheet_name);
  if (!qx_data_any) {
    Logger::Log(L"Warning: Qx context not found for sheet '%ls' in SRatioDataStructure::PostProcess\n",
                qx_sheet_name.c_str());
    return;
  }
  const auto& qx_table_map = std::any_cast<const QxDataStructure::QxTableMap&>(*qx_data_any);

  // Call QxDistribution for the current row only
  auto& row_ref = rows_it->second.back();

  // Excel/VBA sex convention: 1=male, 2=female
  // C++ qx_in storage: [0]=male, [1]=female → convert with sex - 1
  int sex_excel = row_ref->sex;
  int sex_index = sex_excel - 1;

  // Build qx_in[C1][sex][age] = Qx_in(Dnum, ii, Sex, jj) using per-Dnum qx_key_map
  std::array<std::array<std::array<double, 120>, 2>, 5> qx_in{};
  for (const auto& [c1, qx_key] : code_table->qx_key_map) {
    auto it = qx_table_map.find(qx_key);
    if (it != qx_table_map.end()) {
      qx_in[c1] = it->second->qx_in;
      // VBA 포맷과 동일: Qx_in(Dnum, C1, Sex, age)
      // age=0 (첫 행), age=15, age=40, age=65
      for (int age : {0, 15, 40, 65}) {
        Logger::Log(L"  Qx_in(%d,%d,1,%d)=%.6f\n",
                    last_dnum_, c1, age, it->second->qx_in[0][age]);  // Sex=1 male
        Logger::Log(L"  Qx_in(%d,%d,2,%d)=%.6f\n",
                    last_dnum_, c1, age, it->second->qx_in[1][age]);  // Sex=2 female
      }
    }
  }

  row_ref->qx = ActuarialCalculator::QxDistribution(code_table->M_count, qx_in, sex_index);

  // VBA: am = Application.Min(nn, 20)
  row_ref->am = std::min(row_ref->nn, 20);
  // VBA: w = IIf(Sex = 1, 110, 112)
  row_ref->w = (row_ref->sex == 1) ? 110 : 112;
  row_ref->nn1 = row_ref->x + row_ref->nn;

  // VBA: Alp = Alp_in(Dnum, mm), Beta1 = Beta1_in(Dnum, mm), ...
  auto* expense_data_any = GetDataHelper()->GetDataContext(L"Expense");
  if (!expense_data_any) {
    Logger::Log(L"Warning: Expense context not found in SRatioDataStructure::PostProcess\n");
    return;
  }
  const auto& expense_table_map = std::any_cast<const ExpenseDataStructure::ExpenseTableMap&>(*expense_data_any);
  auto expense_it = expense_table_map.find(last_dnum_);
  if (expense_it == expense_table_map.end()) {
    Logger::Log(L"Warning: Expense not found for dnum=%d\n", last_dnum_);
    return;
  }
  const auto* expense_table = expense_it->second.get();
  int mm_int = static_cast<int>(row_ref->mm);
  row_ref->alp = expense_table->alp_in[last_dnum_][mm_int];
  row_ref->beta1 = expense_table->beta1_in[last_dnum_][mm_int];
  row_ref->beta2 = expense_table->beta2_in[last_dnum_][mm_int];
  row_ref->beta3 = expense_table->beta3_in[last_dnum_][mm_int];
  row_ref->gamma = expense_table->gamma_in[last_dnum_][mm_int];

  row_ref->mhj = code_table->mhj;
  row_ref->dnum = last_dnum_;
  // VBA: STD_SRT(Dnum) - 1차 루프에서 계산된 값을 복사
  // (1차 루프가 완료된 후 PostProcess 2차 호출 시 유효한 값이 들어있음)
  row_ref->std_srt = std_srt_[last_dnum_];

  if (code_table->mhj == 0) {
    row_ref->jhj_flag = 0;
    ActuarialCalculator::Computation(row_ref);
    ActuarialCalculator::Pv(row_ref);
  } else {
    // Handle mhj != 0 case if needed
  }
}
