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
#ifndef SRC_DATAPROCESSOR_EXCEL_COLUMNS_H_
#define SRC_DATAPROCESSOR_EXCEL_COLUMNS_H_

// Excel Column Indices for Qx Data Structure (1-indexed)
namespace QxColumns {
constexpr int FIRST_COLUMN = 1;
constexpr int RISK_CLASS = 2;
constexpr int DRIVER = 3;
constexpr int SUB1 = 4;
constexpr int SUB2 = 5;
constexpr int SUB3 = 6;
constexpr int SUB4 = 7;
constexpr int AGE = 8;
constexpr int MALE = 9;
constexpr int FEMALE = 10;
constexpr int QX_NAME = 11;
}  // namespace QxColumns

// Excel Column Indices for Code Data Structure (1-indexed)
namespace CodeColumns {
constexpr int FIRST_COLUMN = 1;
constexpr int DNUM = 2;
constexpr int NAME = 3;
constexpr int QX_KU = 4;
constexpr int MHJ = 5;
constexpr int RE = 6;
constexpr int M_COUNT = 11;
constexpr int QX_TABLE_START = 12;  // Columns 12-41 are qx key names
constexpr int QX_TABLE_END = 41;
constexpr int QX_VALUES_START = 42;  // Columns 42+ are pay/fst/snd values
}  // namespace CodeColumns

#endif  // SRC_DATAPROCESSOR_EXCEL_COLUMNS_H_
