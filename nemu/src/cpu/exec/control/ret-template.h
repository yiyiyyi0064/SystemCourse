#include"cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_,SUFFIX)){
    //从栈中 读指令
    cpu.eip=MEM_R(reg_l(R_ESP));
    if(DATA_BYTE==2){
        cpu.eip&=0xffff;
    }
    reg_l(R_ESP)+=DATA_BYTE;
    //找下一条
    print_asm("ret");
    return 1;
    //命令长度为1
}

make_helper(concat(ret_i_,SUFFIX)){
    int value=instr_fetch(eip+1,2);
    // eip + 1 开始的位置读取2个字节的数据
    int i;
    cpu.eip=MEM_R(reg_l(R_ESP));
    //读下一条 这里就是返回地址
    if(DATA_BYTE==2){
        cpu.eip&=0xffff;//高16位清0
    }
     REG(R_ESP)+=DATA_BYTE;
     for(i=0;i<value;i+=DATA_BYTE){
        MEM_W(REG(R_ESP)+i,0);//全部清零
     }
     REG(R_ESP)+=value;
     //将栈指针恢复至基地址
     print_asm("ret $0x%x",value);
     return 1;
}  

#include "cpu/exec/template-end.h"