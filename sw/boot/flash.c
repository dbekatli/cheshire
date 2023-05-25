// Copyright 2023 ETH Zurich and University of Bologna.
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0
//
// Nicole Narr <narrn@student.ethz.ch>
// Christopher Reinwardt <creinwar@student.ethz.ch>
//
// Boot disk flasher for Cheshire; writes a disk image to the boot-mode-selected disk.

#include <stdint.h>
#include "util.h"
#include "params.h"
#include "regs/cheshire.h"
#include "spi_host_regs.h"
#include "dif/clint.h"
#include "hal/i2c_24fc1025.h"
#include "hal/spi_s25fs512s.h"
#include "hal/spi_sdcard.h"
#include "hal/uart_debug.h"
#include "gpt.h"
#include "printf/h"

int flash_spi_sdcard(uint64_t core_freq, uint64_t rtc_freq, uint64_t len, uint64_t offs) {
    // Initialize device handle
    spi_sdcard_t device = {
        .spi_freq = 24 * 1000 * 1000, // 24MHz (maximum is 25MHz)
        .csid = 0,
        .csid_dummy = SPI_HOST_PARAM_NUM_C_S - 1 // Last physical CS is designated dummy
    };
    CHECK_CALL(spi_sdcard_init(&device, core_freq))
    // Wait for device to be initialized (1ms, round up extra tick to be sure)
    clint_spin_until((1000 * rtc_freq) / (1000 * 1000) + 1);
    // TODO: Sector writing code here!
}

int flash_spi_s25fs512s(uint64_t core_freq, uint64_t rtc_freq, uint64_t len, uint64_t offs) {
    // Initialize device handle
    spi_s25fs512s_t device = {
        .spi_freq = MIN(40 * 1000 * 1000, core_freq / 4), // Up to quarter core freq or 40MHz
        .csid = 1};
    CHECK_CALL(spi_s25fs512s_init(&device, core_freq))
    // Wait for device to be initialized (t_PU = 300us, round up extra tick to be sure)
    clint_spin_until((350 * rtc_freq) / (1000 * 1000) + 1);
    // TODO: Sector writing code here!
}

int flash_i2c_24fc1025(uint64_t core_freq, uint64_t len, uint64_t offs) {
    // Initialize device handle
    dif_i2c_t i2c;
    CHECK_CALL(i2c_24fc1025_init(&i2c, core_freq))
    // TODO: Sector writing code here!
}

int main() {
    // Read reference frequency and compute core frequency
    uint32_t rtc_freq = *reg32(&__base_regs, CHESHIRE_RTC_FREQ_REG_OFFSET);
    uint64_t core_freq = clint_get_core_freq(rtc_freq, 2500);
    // TODO: read arguments from scratch registers (length and offset in 512B sectors)
    uint64_t len, offs;
    void* img_base;
    switch (tgtmode) {
    case 1:
        return flash_spi_sdcard(core_freq, rtc_freq, len, offs);
    case 2:
        return flash_spi_s25fs512s(core_freq, rtc_freq, len, offs);
    case 3:
        return flash_i2c_24fc1025(core_freq, len, offs);
    default:
        printf("[FLASH] Unsupported autonomous target mode %d; exitting...\r\n");
    }
    return -1;
}
