#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);
uint32_t eval(int p,int q);
int nr_token;
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args) {
	if(args!=NULL){
	char* num_exe=strtok(NULL," ");
	int num_exed;
	if(sscanf(num_exe,"%d",&num_exed)==1){
		//printf("%d",num_exed); 
		cpu_exec(num_exed);
	}
	}
	else{
	cpu_exec(1);
	}
	return 0;
}
static int cmd_info(char *args) {
	char* name_reg[]={"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
	char *instru=strtok(args," ");
	if(*instru=='r'){
		int i=0,j=0;
		for(;i<8;i++){
			printf("%s",name_reg[i]);
			j=0;
			for(;j<5;j++){
				printf(" ");
			}
			//printf("0x%08x",cpu.eax);
			printf("0x%08x",cpu.gpr[i]._32);
			j=0;
			for(;j<5;j++){
				printf(" ");
			}
			printf("%u\n",cpu.gpr[i]._32);
		}
			printf("%s","eip");
			j=0;
			for(;j<5;j++){
				printf(" ");
			}
			//printf("0x%08x",cpu.eax);
			printf("0x%08x",cpu.eip);
			j=0;
			for(;j<5;j++){
				printf(" ");
			}
			printf("%u\n",cpu.eip);
	}else if(*instru=='w'){
		printf_watching();
		return 0;
		//char *adress_=strtok(args," ");
		//cmd_w(adress_);
	}
	return 0;
}
static int cmd_x(char *args) {
	//char *num_check=strtok(NULL," ");
	//char *start_loc=strtok(NULL," ");
	//bool *success =false;
	//int result_num=expr(num_check,success);
	//uint32_t result_start=expr(start_loc,success);
	int num_checkd;
	unsigned int start_lochx;
	sscanf(args,"%d %x",&num_checkd,&start_lochx);
	int n_tmp=0;
	for(;n_tmp<num_checkd;n_tmp++){
		if(n_tmp%4==0){
			if(n_tmp==0){}
			else
			printf("\n");
			printf("0x%08x",start_lochx+n_tmp*4);
			printf(":");
		}
		printf("0x%08x",swaddr_read(start_lochx+n_tmp*4,4));
		printf(" ");

	}
	printf("\n");
	return 0;
}
static int cmd_p(char *args) {
	bool *success =false;
	int result=expr(args,success);
	if(!success){
	printf("0x%08x(%d)\n",result,result);
	}
	return 0;
}
static int cmd_w(char *args) {
	//WP* new_dot=new_wp();//申请空闲监视点结构
	//这里犯了个错误 应该直接保存expr
	args += strspn(args, " ");
	WP* new_dot=set_watchpoint(args);
	//new_dot->expr_watching=args;
	//这里的NO是本来就有的 并不是后来添加的 后来添加的只有expr
	printf("Set Watchpoint #%d: %s\n",new_dot->NO,new_dot->expr_watching);
	return 0;
}
static int cmd_d(char *args) {
	char* num_del=strtok(args," ");
	int num_d;
	bool key=true;
	sscanf(num_del,"%d",&num_d);
	WP* addr_d=delete_wp(num_d,&key);
	if(key){
		//printf("%s\n",addr_d->expr_watching);
		free_wp(addr_d);
		return 0;
	}else{
		printf("Watchpoint #%d does not exist\n",num_d);
		return 0;
	}
	return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	/* TODO: Add more commands */
	{"si","Single execuation",cmd_si},
	{"info","Print the present value of the register",cmd_info},
	{"x","Print the value stored in the memory",cmd_x},
	{"p","Evaluate the expression",cmd_p},
	{"w","Add a watching point.",cmd_w},
	{"d","Delete the watching point.",cmd_d},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
