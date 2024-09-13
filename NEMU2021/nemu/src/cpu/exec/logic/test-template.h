#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
  DATA_TYPE ret = op_dest -> val & op_src -> val; 
  //执行AND操作决定标志位
  //op_dest op_src 目标操作数与源操作数 
  cpu.eflags.SF = ret >> ((DATA_BYTE << 3) - 1); 
  //((DATA_BYTE << 3) - 1) 将databyte转化为以位为单位 
  //取结果的最高位 表示结果的符号
  cpu.eflags.ZF = !ret; 
  //如果ret==0 那么zf=1
  cpu.eflags.CF = 0; 
  cpu.eflags.OF = 0; 
  //test指令不影响这两个
  ret ^= ret >> 4;
  //将高四位与第四位一起折叠，确定整个低八位的1的奇偶
  ret ^= ret >> 2;
  ret ^= ret >> 1;
  ret &= 1; 
  //逐步折叠的操作！！！
  //折叠过后相当于这八位全都进行过组合 因为我的目标是数奇偶 那必然遍历。
  //又因为：偶数个1会产生0 奇数个1会产生1
  //故最后结果为1 则是奇数
  //pf 如果最低字节中1的个数是偶数则设置为1
  cpu.eflags.PF = !ret; // PF == 1, even number, Pf == 0, odd number
  DATA_TYPE result = op_dest -> val & op_src -> val;
  update_eflags_pf_zf_sf((DATA_TYPE_S)result);
  cpu.eflags.CF = cpu.eflags.OF = 0;
  print_asm_template1();
}

make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
#include "cpu/exec/template-end.h"
