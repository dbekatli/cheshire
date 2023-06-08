// Generated register defines for CLINT

// Copyright information found in source file:
// Copyright 2020 ETH Zurich and University of Bologna.

// Licensing information found in source file:
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

#ifndef _CLINT_REG_DEFS_
#define _CLINT_REG_DEFS_

#ifdef __cplusplus
extern "C" {
#endif
// Number of cores
#define CLINT_PARAM_NUM_CORES 4

// Register width
#define CLINT_PARAM_REG_WIDTH 32

// Machine Software Interrupt Pending  (common parameters)
#define CLINT_MSIP_P_FIELD_WIDTH 1
#define CLINT_MSIP_P_FIELDS_PER_REG 32
#define CLINT_MSIP_MULTIREG_COUNT 1

// Machine Software Interrupt Pending
#define CLINT_MSIP_REG_OFFSET 0x0
#define CLINT_MSIP_P_0_BIT 0
#define CLINT_MSIP_P_1_BIT 1
#define CLINT_MSIP_P_2_BIT 2
#define CLINT_MSIP_P_3_BIT 3

// Machine Timer Compare
#define CLINT_MTIMECMP_LOW0_REG_OFFSET 0x4000

// Machine Timer Compare
#define CLINT_MTIMECMP_HIGH0_REG_OFFSET 0x4004

// Machine Timer Compare
#define CLINT_MTIMECMP_LOW1_REG_OFFSET 0x4008

// Machine Timer Compare
#define CLINT_MTIMECMP_HIGH1_REG_OFFSET 0x400c

// Machine Timer Compare
#define CLINT_MTIMECMP_LOW2_REG_OFFSET 0x4010

// Machine Timer Compare
#define CLINT_MTIMECMP_HIGH2_REG_OFFSET 0x4014

// Machine Timer Compare
#define CLINT_MTIMECMP_LOW3_REG_OFFSET 0x4018

// Machine Timer Compare
#define CLINT_MTIMECMP_HIGH3_REG_OFFSET 0x401c

// Timer Register Low
#define CLINT_MTIME_LOW_REG_OFFSET 0xbff8

// Timer Register High
#define CLINT_MTIME_HIGH_REG_OFFSET 0xbffc

#ifdef __cplusplus
}  // extern "C"
#endif
#endif  // _CLINT_REG_DEFS_
// End generated register defines for CLINT