// Copyright 2022 ETH Zurich and University of Bologna.
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51
//
// Nicole Narr <narrn@student.ethz.ch>
// Christopher Reinwardt <creinwar@student.ethz.ch>

module tb_cheshire_soc;

  cheshire_soc_fixture fix();

  string binary;
  logic [1:0]  bootmode;
  logic        testmode;
  logic [63:0] entry;
  longint      entry_int;
  int          exit_status = 0;

  initial begin

    #1000000000ns

    $finish;
  end

endmodule
