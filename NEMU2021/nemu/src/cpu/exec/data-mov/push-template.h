#include "cpu/exec/template-start.h"

#define instr push

static void do_execute(){
    if(DATA_BYTE==1){
		op_src->val=(int8_t)op_src->val;
	}//这个是一个字节的情况
	reg_l(R_ESP)-=4;//栈指针下移
	//reg_l 获取对应寄存器32位值
	swaddr_write(reg_l(R_ESP),4,op_src->val);
	print_asm_template1();
}

make_instr_helper(i)
#if DATA_BYTE==2||DATA_BYTE==4
make_instr_helper(r)
make_instr_helper(rm)
#endif
#include "cpu/exec/template-end.h"