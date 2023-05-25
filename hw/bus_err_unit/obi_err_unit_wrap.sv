// Copyright 2023 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

module obi_err_unit_wrap #(
  parameter int unsigned AddrWidth       = 32,
  parameter int unsigned ErrBits         = 1,
  parameter int unsigned NumOutstanding  = 2,
  parameter int unsigned NumStoredErrors = 1,
  parameter bit          DropOldest      = 1'b0,
  parameter type         reg_req_t       = logic,
  parameter type         reg_rsp_t       = logic
) (
  input  logic                 clk_i,
  input  logic                 rst_ni,
  input  logic                 testmode_i,

  input  logic                 obi_req_i,
  input  logic                 obi_gnt_i,
  input  logic                 obi_rvalid_i,
  input  logic [AddrWidth-1:0] obi_addr_i,
  input  logic [  ErrBits-1:0] obi_err_i,

  output logic                 err_irq_o,

  input  reg_req_t             reg_req_i,
  output reg_rsp_t             reg_rsp_o
);

  bus_err_unit #(
    .AddrWidth      (AddrWidth),
    .ErrBits        (ErrBits),
    .NumOutstanding (NumOutstanding),
    .NumStoredErrors(NumStoredErrors),
    .NumChannels    (1),
    .DropOldest     (DropOldest),
    .reg_req_t      (reg_req_t),
    .reg_rsp_t      (reg_rsp_t)
  ) i_err_unit (
    .clk_i,
    .rst_ni,
    .testmode_i,

    .req_hs_valid_i   ( obi_req_i & obi_gnt_i ),
    .req_addr_i       ( obi_addr_i            ),
    .rsp_hs_valid_i   ( obi_rvalid_i          ),
    .rsp_burst_last_i ( obi_rvalid_i          ),
    .rsp_err_i        ( obi_err_i             ),

    .err_irq_o,

    .reg_req_i,
    .reg_rsp_o
  );

endmodule
