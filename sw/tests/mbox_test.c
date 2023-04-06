// Copyright (c) 2022 ETH Zurich and University of Bologna
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
//

#include "regs/cheshire.h"
#include "dif/clint.h"
#include "dif/uart.h"
#include "params.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
/*
void external_irq_handler(void)  {           // mbox irq handler
  
  #define PLIC_BASE     0x04000000           // PLIC base address
  #define PLIC_CHECK    PLIC_BASE + 0x201004 // Irq claim memory region in PLIC
  
  volatile int * claim_irq, * doorbell;
  int mbox_id = 51;
  
  claim_irq = (int *) PLIC_CHECK;           
  doorbell = (int *) 0x40001024;

  *doorbell = 0x0;                           // deassert mbox irq
  *claim_irq = mbox_id;                      // claim mbox irq (according to PLIC protocol)
  
  return;
}
*/
int main(int argc, char const *argv[]) {

  #define PLIC_BASE     0x04000000                              // PLIC base address 
  #define PLIC_CHECK    PLIC_BASE + 0x200004                    // Irq claim memory region in PLIC
  #define PLIC_EN_BITS  PLIC_BASE + 0x2000                      // Irq enable memory region in PLIC

  int a, b, c, d, e;
  int mbox_id = 51;                                             // index of mbox irq in the irq vector input to the PLIC

  volatile int * plic_en, * plic_prio;

  // CVA6 Plic and Ira config
  
  plic_prio = (int *) (PLIC_BASE+mbox_id*4);
  plic_en   = (int *) (PLIC_EN_BITS+((int)(mbox_id/32))*4);
  unsigned global_irq_en   = 0x00001808;  
  unsigned external_irq_en = 0x00000800;  

  *plic_prio = 0x1;                                             // Set irq priority (must be non-zero to enable it)
  *plic_en   =  1<<(mbox_id%32);                                // Enable the Mbox Irq 
  asm volatile("csrw  mstatus, %0\n" : : "r"(global_irq_en  )); // Set global interrupt enable in CVA6 csr
  asm volatile("csrw  mie, %0\n"     : : "r"(external_irq_en)); // Set external interrupts
  
  // start mbox test
  // write/read sequence to check whether the mailbox is accessible
  
  axi_write(0x40001008, 0xBAADC0DE); 
  axi_write(0x40001010, 0xBAADC0DE);
  axi_write(0x40001014, 0xBAADC0DE);
  axi_write(0x40001018, 0xBAADC0DE);
  axi_write(0x4000101C, 0xBAADC0DE);
 
  a = axi_read(0x40001008);          
  b = axi_read(0x40001010);
  c = axi_read(0x40001014);
  d = axi_read(0x40001018);
  e = axi_read(0x4000101C); 
  
  if( a == 0xBAADC0DE && b == 0xBAADC0DE && c == 0xBAADC0DE && d == 0xBAADC0DE && e == 0xBAADC0DE){
     axi_write(0x40001020, 0x00000001);    // ring doorbell if mailbox is accessible
     while(axi_read(PLIC_CHECK)!=mbox_id)  // loop on wfi until the irq with the correct ID is asserted
       wfi();
  }

  return 0;
}

