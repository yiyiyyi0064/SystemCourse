#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>   //这是c语言自带的正则表达式的库

enum {
	NOTYPE = 256, 
	EQ=1,
	NUM=2,
	HEX=3,
	REG=4,
	/* TODO: Add more token types */
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{"[0-9]+",NUM},							//decimal
	{"0[xX][0-9a-fA-F]+",HEX},
	{"\\$[a-z]+",REG},
	{"\\)",')'},
	{"\\(",'('},
	{"\\/",'/'},						//divide
	{"\\*",'*'},						//multiple
	{"\\-",'-'},						//minor
	{" +",	NOTYPE},				// spaces 这里用了+其实就是可以表示多个空格也可以只表示一个spaces
	{"\\+", '+'},					// plus
	{"==", EQ}						// equal
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
	int position = 0;
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
					default: panic("please implement me");
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

	panic("please implement me");
	return 0;
}
Token *domi_position(Token* p,Token* q){	
	int j=0;
	//char signal_list[100]={0};
	
	for(;j<=q-p;j++){
		if((q-j)->type==')'){
			for(;j<=q-p;j++){
				if((q-j)->type=='('){
					break;
				}
			}
		}
		if((q-j)->type=='+'||(q-j)->type=='-'){
			return q-j;
		}
	}
	j=0;
	for(;j<=q-p;j++){
		if((q-j)->type==')'){
			for(;j<=q-p;j++){
				if((q-j)->type=='('){
					break;
				}
			}
		}
		if((q-j)->type=='*'||(q-j)->type=='/'){
			return q-j;
		}
	}
	return 0;
}
bool check_parentheses(Token* p,Token* q){
	//principle: leftmost and rightmost should be matched.
	char parenthe[100]={0};
	int num_pare=0;
	for(;p<=q;p++){
		if(p->type=='('||p->type==')'){
			parenthe[num_pare]=p->type;
			num_pare++;
		}
	}
	int j=num_pare-1;
	int k=0;
	for(;j>=0;j--){
		if(parenthe[j]==')'&&parenthe[k]=='('){
			k++;
		}else{
			return false;
		}
	}
	return true;

}


int eval(Token* p,Token* q){
	if(p>q){
		return 0;
	}else if(p==q){
		int result;
		sscanf(p->str,"%d",&result);
		return result;
	}
	else if(check_parentheses(p,q)==true){
		return eval(p+1,q-1);

	}else {
		Token *op=domi_position(p,q);
		int val1=eval(p,op-1);
		int val2=eval(op+1,q);
		switch(op->type){
			case '+': return val1+val2;
			case '-': return val1-val2;
			case '/': return val1/val2;
			case '*': return val1*val2;
			default:assert(0);
		}
	}
}
