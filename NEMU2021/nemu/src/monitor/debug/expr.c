#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>   //这是c语言自带的正则表达式的库
int eval(int p,int q);
enum {
	NOTYPE = 256, 
	EQ=1,
	NUM=2,
	HEX=3,
	REG=4,
	NOTEQ=5,
	OR=6,
	AND=7,
	/* TODO: Add more token types */
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{" +",	NOTYPE},

	{"\\-",'-'},						//minor
	{"\\+", '+'},
	{"\\/",'/'},						//divide
	{"\\*",'*'},						//multiple
	
	{"\\$[a-z]+",REG},							//decimal
	{"0[xX][0-9a-fA-F]+",HEX},
	{"[0-9]+",NUM},
	
	{"==", EQ},	
	{"!=",NOTEQ},

	
	{"\\)",')'},
	{"\\(",'('},
			
	{"\\|\\|",OR},
	{"&&",AND},
	{"!",'!'},		// equal
	
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position=0 ;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {//识别成功才会进入
				char *substr_start = e + position;//这里是入口 这时候position还没更新 即rm.so
				int substr_len = pmatch.rm_eo;// 识别得到的字符段(子串) rm_eo是匹配结束的位置

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				int j=0;
				for(;j<32;j++){
					tokens[nr_token].str[j]='\0';
					//执行清空操作
				}
				switch(rules[i].token_type) {
					case 256:
						break;
					case 1:
						tokens[nr_token].type=1;
						strcpy(tokens[nr_token].str,"==");
						nr_token++;
						break;
					case 2:
						tokens[nr_token].type=2;
						strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
						nr_token++;
						break;		
					case 3:
						tokens[nr_token].type=3;
						strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
						nr_token++;
						break;
		            case 4:
						tokens[nr_token].type=4;
						strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
						nr_token++;
						break;
					case 5:
						tokens[nr_token].type=5;
						strcpy(tokens[nr_token].str,"!=");
						nr_token++;
						break;
					case 6:
						tokens[nr_token].type=6;
						strcpy(tokens[nr_token].str,"||");
						nr_token++;
						break;
					case 7:
						tokens[nr_token].type=5;
						strcpy(tokens[nr_token].str,"&&");
						nr_token++;
						break;
					case '+':
						tokens[nr_token].type='+';//因为没有str 故不需要复制了
						nr_token++;
						break;
					case '-':
						tokens[nr_token].type='-';
						nr_token++;
						break;
					case '*':
						tokens[nr_token].type='*';
						nr_token++;
						break;
					case '/':
						tokens[nr_token].type='/';
						nr_token++;
						break;
					case '(':
						tokens[nr_token].type='(';
						nr_token++;
						break;
					case ')':
						tokens[nr_token].type=')';
						nr_token++;
						break;
					case '!':
						tokens[nr_token].type='!';
						nr_token++;
						break;
					default: 
						assert(0);
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	return eval(0,nr_token);

}
int domi_position(int p,int q){	
	int j=0;
	int step=0;
	int pri=0;
	//char signal_list[100]={0};
	int op=0;
	for (j = p; j <= q; j++){
		if (tokens[j].type == '('){
			step++;
		}
		if (tokens[j].type == ')'){
			step--;
		}
	
		if (step == 0){
		if (tokens[j].type == OR){
			if (pri < 9){
				op = j;
				pri = 9;
			}
		} else if (tokens[j].type == AND){
			if (pri < 8){
				op = j;
				pri = 8;
			}
		} else if (tokens[j].type == EQ || tokens[j].type == NOTEQ){
			if (pri < 7){
				op = j;
				pri = 7;
			}
		} else if (tokens[j].type == '+' || tokens[j].type == '-'){
			if (pri <6){
				op = j;
				pri = 6;
			}
		} else if (tokens[j].type == '*' || tokens[j].type == '/'){
			if (pri < 5){
				op = j;
				pri = 5;
			}
		}
		else if (step < 0){
			return -2;
		}
	}
	}
	return op;
}
bool check_parentheses(int p, int q){
	int a;
	int j = 0, k = 0;
	if (tokens[p].type == '(' || tokens[q].type == ')'){
		for (a = p; a <= q; a++){
			if (tokens[a].type == '('){
				j++;
			}
			if (tokens[a].type == ')'){
				k++;
			}
			if (a != q && j == k){
				return false;
			}
		}
		if (j == k){
				return true;
			} else {
				return false;
			}
	}
	return false;
}

	



int eval(int p,int q){
	int result=0;
	if(p>q){
		assert(0);
	}else if(p==q){
		if(tokens[p].type==NUM){
		sscanf(tokens[p].str,"%d",&result);
		return result;
		}else if(tokens[p].type==HEX){
			int i=2;
			while(tokens[p].str[i]!='\0')//从高位到低位
			{
				result*=16;
				result+=tokens[p].str[i]<58?tokens[p].str[i]-'0':tokens[p].str[i]-'a'+10;//这里只默认会只有a出现
				i++;
			}
			return result;
		}else if(tokens[p].type==REG){
			if(!strcmp(tokens[p].str,"$eax")){
				result =cpu.eax;
				return result;
			}else if(!strcmp(tokens[p].str,"$ecx")){
				result =cpu.ecx;
				return result;
			}else if(!strcmp(tokens[p].str,"$edx")){
				result =cpu.edx;
				return result;
			}else if(!strcmp(tokens[p].str,"$ebx")){
				result =cpu.ebx;
				return result;
			}else if(!strcmp(tokens[p].str,"$esp")){
				result =cpu.esp;
				return result;
			}else if(!strcmp(tokens[p].str,"$ebp")){
				result =cpu.ebp;
				return result;
			}else if(!strcmp(tokens[p].str,"$esi")){
				result =cpu.esi;
				return result;
			}else if(!strcmp(tokens[p].str,"$edi")){
				result =cpu.edi;
				return result;
			}else if(!strcmp(tokens[p].str,"$eip")){
				result =cpu.eip;
				return result;
			}else{
				return 0;
			}
		}else{
			assert(0);
		}
	}
	else if(check_parentheses(p,q)==true){
		return eval(p+1,q-1);
	}else {
		int op=domi_position(p,q);
		//printf("%d\n",op);
		if(tokens[p].type=='!'){
			sscanf(tokens[p].str,"%d",&result);
			return !result;
		}else if(tokens[p].type==REG){
			if (!strcmp(tokens[p].str, "$eax")){
					result = cpu.eax;
					return result;
				} else if (!strcmp(tokens[p].str, "$ecx")){
					result = cpu.ecx;
					return result;
				} else if (!strcmp(tokens[p].str, "$edx")){
					result = cpu.edx;
					return result;
				} else if (!strcmp(tokens[p].str, "$ebx")){
					result = cpu.ebx;
					return result;
				} else if (!strcmp(tokens[p].str, "$esp")){
					result = cpu.esp;
					return result;
				} else if (!strcmp(tokens[p].str, "$ebp")){
					result = cpu.ebp;
					return result;
				} else if (!strcmp(tokens[p].str, "$esi")){
					result = cpu.esi;
					return result;
				} else if (!strcmp(tokens[p].str, "$edi")){
					result = cpu.edi;
					return result;
				} else if (!strcmp(tokens[p].str, "$eip")){
					result = cpu.eip;
					return result;
				} else {
					assert(0);
					return 0;
				}
		}
		int val1=eval(p,op-1);
		int val2=eval(op+1,q);
		switch(tokens[op].type){
			case '+': return val1+val2;
			case '-': return val1-val2;
			case '/': return val1/val2;
			case '*': return val1*val2;
			case OR: return val1||val2;
			case AND:return val1&&val2;
			case EQ: 
				if(val1==val2) return 1;
				else return 0;
			case NOTEQ:
				if(val1==val2) return 0;
				else return 1;
			default:assert(0);
		}
	}
	return 0;
}
