#include "cpu/exec/template-start.h"

#define instr add
//对照sub来写
static void do_execute () {
	DATA_TYPE result = op_dest->val + op_src->val;
	//dest&src都是addr val是指针指向的值 addr是指针本身
	//类型由datatype来决定
	OPERAND_W(op_dest, result);
	update_eflags_pf_zf_sf((DATA_TYPE_S)result);
	cpu.eflags.CF = result > op_dest->val;
	cpu.eflags.OF = MSB((op_dest->val ^ op_src->val) & (op_dest->val ^ result));
	//这里是将result写入dest->val 即写到dest->addr中
	//注意有add需要看eflags寄存器标志位是否变化
	print_asm_template2();
}


#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(i2a)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"