#include "cpu/exec/template-start.h"

#define instr jl
//jl 如果小于  sf!=of||zf==1
static void do_execute(){
	DATA_TYPE_S displacement=op_src->val;
	print_asm("jl %x",cpu.eip+1+DATA_BYTE+displacement);
	if(cpu.eflags.SF!=cpu.eflags.OF)cpu.eip+=displacement;
    

}

make_instr_helper(i)

#include "cpu/exec/template-end.h"