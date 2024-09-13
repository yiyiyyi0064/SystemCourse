#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
	current_sreg = R_SS;
	OPERAND_W(op_src, swaddr_read(cpu.esp, 4));
	cpu.esp += 4;
	print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)

#include "cpu/exec/template-end.h"