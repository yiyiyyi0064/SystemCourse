#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	if(DATA_BYTE==1){
		swaddr_write(op_src->addr,4,MEM_R(reg_l (R_ESP)));
		//将指针指向的data拷贝到src指向地址
		MEM_W(reg_l (R_ESP),0);
		//MEM_W addr data 将data写入addr中 
		//这里意思是将指针所指地址处data归零 出栈
		reg_l (R_ESP)+=4;
		//指针上移
	}else{
		OPERAND_W(op_src,MEM_R(REG(R_ESP)));
		//#define OPERAND_W(op, src) concat(write_operand_, SUFFIX) (op, src)
		MEM_W(REG(R_ESP),0);
		REG(R_ESP)+=DATA_BYTE;
		//与reg_l(R_ESP)+=DATA_BYTE 有什么区别？
		//区别：reg_l(R_ESP)专门访问32位寄存器
		//而REG(R_ESP)可适用于不同位宽的寄存器
	}
	
	print_asm_template1();
}
#if DATA_BYTE == 2||DATA_BYTE==4
make_instr_helper(r)
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"