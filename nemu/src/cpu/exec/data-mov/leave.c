#include "cpu/exec/helper.h"

//无需泛型
make_helper(leave) {
	cpu.esp=cpu.ebp;
	//将基址指针（ebp）的值赋值给栈指针->释放当前函数栈帧
	//从头开始
	cpu.ebp=swaddr_read(cpu.esp,4);
	//读取保存ebp值 恢复调用者栈帧 后续再次调用则从此开始
	cpu.esp+=4;
	//将栈指针向上移动4字节，完成弹栈操作。
	//相当于pop 再往上移动
	print_asm("leave");
	//输出调试信息 显示执行leave指令
	return 1;
	//表示leave占用1个字节 len
}