#include "cpu/exec/template-start.h"

#define instr ja
//ja 高于跳转  cf==0&&zf==0
static void do_execute(){
	DATA_TYPE_S displacement=op_src->val;
	print_asm("ja %x",cpu.eip+1+DATA_BYTE+displacement);
	if(cpu.eflags.ZF==0&&cpu.eflags.CF==0)cpu.eip+=displacement;
    

}

make_instr_helper(i)

#include "cpu/exec/template-end.h"