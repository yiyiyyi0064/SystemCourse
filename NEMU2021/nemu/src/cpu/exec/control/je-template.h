#include "cpu/exec/template-start.h"

#define instr je
//je 如果等于  zf==1
static void do_execute(){
	DATA_TYPE_S displacement=op_src->val;
	print_asm("je %x",cpu.eip+1+DATA_BYTE+displacement);
	if(cpu.eflags.ZF==1)cpu.eip+=displacement;
    

}

make_instr_helper(i)

#include "cpu/exec/template-end.h"