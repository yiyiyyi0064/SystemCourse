#include "cpu/exec/template-start.h"

#define instr call

make_helper(concat(call_i_, SUFFIX)) {
	//concat用于连接两个字符串 suffix是后缀，用于确认call所处理的操作数类型
	//生成函数名字 这里使用拼接就是泛型编程
	int len = concat(decode_i_, SUFFIX)(eip + 1); 
	//生成解码函数名 并从eip+1处开始解码 
	//注意这里eip应该在上一条指令的末尾 eip+1才是当前指令开始
	//并且返回指令的长度 (concat之后就会对应一个函数名（比如add_i_v这种）然后就会调用对应函数，其结果是返回一个instr的长度)
	//注意这里返回的是当前指令长度 包括操作码和操作数 为计算出下一条指令的地址
	reg_l(R_ESP) -= DATA_BYTE; 
	//regl访问esp寄存器的值，databyte表示数据字节数
	//将栈指针下移 为保存返回地址腾出空间
	MEM_W(reg_l(R_ESP), cpu.eip + len + 1); 
	//写入当前的下一条指令 即返回地址
	print_asm("call: 0x%x", cpu.eip + len + 1); 
	//打印指令信息 显示调用的地址
	return len + 1; 
	//返回总长度 更新eip 以确保能够正确读出下一条指令
	//len+1 直接跳转到下一条指令开头？
}

make_helper(concat(call_rm_, SUFFIX)) {
	int len = concat(decode_rm_, SUFFIX)(eip + 1);
	reg_l(R_ESP) -= DATA_BYTE;
	MEM_W(reg_l(R_ESP), cpu.eip + len + 1);
	cpu.eip = (DATA_TYPE_S)op_src->val - len - 1;
	print_asm("call: %s", op_src->str);
	return len + 1;
}

#include "cpu/exec/template-end.h"
