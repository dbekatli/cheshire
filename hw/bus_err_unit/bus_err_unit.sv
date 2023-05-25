// Copyright 2023 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

module bus_err_unit #(
  parameter int unsigned AddrWidth = 48,
  parameter int unsigned ErrBits = 3,
  parameter int unsigned NumOutstanding = 4,
  parameter int unsigned NumStoredErrors = 4,
  parameter int unsigned NumChannels = 1, // Channels are one-hot!
  parameter bit          DropOldest = 1'b0,
  parameter type         reg_req_t = logic,
  parameter type         reg_rsp_t = logic
) (
  input  logic                   clk_i,
  input  logic                   rst_ni,
  input  logic                   testmode_i,
  
  input  logic [NumChannels-1:0] req_hs_valid_i,
  input  logic [  AddrWidth-1:0] req_addr_i,
  input  logic [NumChannels-1:0] rsp_hs_valid_i,
  input  logic [NumChannels-1:0] rsp_burst_last_i,
  input  logic [    ErrBits-1:0] rsp_err_i,

  output logic                   err_irq_o,

  input  reg_req_t               reg_req_i,
  output reg_rsp_t               reg_rsp_o

);
  assert final ($onehot0(req_hs_valid_i)) else $fatal(1, "Bus Error unit requires one-hot!");
  assert final ($onehot0(rsp_hs_valid_i)) else $fatal(1, "Bus Error unit requires one-hot!");

  typedef struct packed {
    logic [ErrBits-1:0] err;
    logic [AddrWidth-1:0] addr;
  } err_addr_t;

  logic [NumChannels-1:0][AddrWidth-1:0] err_addr;
  err_addr_t read_err_addr;
  logic bus_unit_full;
  logic read_enable;
  logic err_fifo_empty;
  bus_err_unit_reg_pkg::bus_err_unit_reg2hw_t reg2hw;
  bus_err_unit_reg_pkg::bus_err_unit_hw2reg_t hw2reg;

  assign read_enable = reg2hw.err_code.re;
  assign hw2reg.err_addr.d = read_err_addr.addr[31:0];
  if (AddrWidth > 32) begin
    always_comb begin
      hw2reg.err_addr_top.d = '0;
      hw2reg.err_addr_top.d[AddrWidth-32-1:0] = read_err_addr.addr[AddrWidth-1:32];
    end
  end else begin
    assign hw2reg.err_addr_top.d = '0;
  end
  assign hw2reg.err_code.d = read_err_addr.err;
  assign err_irq_o = ~err_fifo_empty;

  bus_err_unit_reg_top #(
    .reg_req_t ( reg_req_t ),
    .reg_rsp_t ( reg_rsp_t )
  ) i_regs (
    .clk_i,
    .rst_ni,
    .reg_req_i,
    .reg_rsp_o,
    .reg2hw (reg2hw),
    .hw2reg (hw2reg),
    .devmode_i ('0)
  );

  for (genvar i = 0; i < NumChannels; i++) begin
    fifo_v3 #(
      .FALL_THROUGH ( 1'b0           ),
      .DATA_WIDTH   ( AddrWidth      ),
      .DEPTH        ( NumOutstanding )
    ) i_addr_fifo (
      .clk_i,
      .rst_ni,
      .flush_i   (1'b0),
      .testmode_i(testmode_i),
      .full_o    (),
      .empty_o   (),
      .usage_o   (),
      .data_i    (req_addr_i),
      .push_i    (req_hs_valid_i[i]),
      .data_o    (err_addr[i]),
      .pop_i     (rsp_burst_last_i[i])
    );
  end

  logic [cf_math_pkg::idx_width(NumChannels)-1:0] chan_select;

  onehot_to_bin #(
    .ONEHOT_WIDTH(NumChannels)
  ) i_rsp_chan_select (
    .onehot(rsp_hs_valid_i),
    .bin   (chan_select)
  );

  logic push_err_fifo, pop_err_fifo;
  err_addr_t fifo_data;

  assign push_err_fifo = (|rsp_hs_valid_i) & (DropOldest | ~bus_unit_full) & (|rsp_err_i);
  assign pop_err_fifo  = (read_enable & ~err_fifo_empty) | (DropOldest & bus_unit_full);

  assign fifo_data = '{err: rsp_err_i, addr: err_addr[chan_select]};

  fifo_v3 #(
    .FALL_THROUGH ( 1'b0            ),
    .dtype        ( err_addr_t      ),
    .DEPTH        ( NumStoredErrors )
  ) i_err_fifo (
    .clk_i,
    .rst_ni,
    .flush_i   ( 1'b0           ),
    .testmode_i( testmode_i     ),
    .full_o    ( bus_unit_full  ),
    .empty_o   ( err_fifo_empty ),
    .usage_o   (),
    .data_i    ( fifo_data      ),
    .push_i    ( push_err_fifo  ),
    .data_o    ( read_err_addr  ),
    .pop_i     ( pop_err_fifo   )
  );

endmodule
