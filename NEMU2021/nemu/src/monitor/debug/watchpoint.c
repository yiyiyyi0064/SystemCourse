#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "cpu/reg.h"
#include<stdlib.h>
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
		//wp_pool[i].expr_watching='\0';
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
bool check_(){
	bool check = false;
	bool *success = false;
	WP *temp = head;
	int expr_temp=0;
	while(temp != NULL){
		//printf("%s\n",temp->expr_watching);
		expr_temp = expr(temp->expr_watching, success);
		if (expr_temp != temp->value_watching){
			check = true;
			printf ("Hint watchpoint %d at address 0x%08x\n", temp->NO, cpu.eip);
			temp = temp->next;
			continue;
		}
		printf ("Watchpoint %d: %s\n",temp->NO,temp->expr_watching);
		printf ("Old value = %d\n",temp->value_watching);
		printf ("New value = %d\n",expr_temp);
		temp->value_watching = expr_temp;
		temp = temp->next;
	}
	return check;
}

static WP* new_wp(){
	WP *temp;
	temp = free_;
	free_ = free_->next;
	temp->next = NULL;
	if (head == NULL){
		head = temp;
	} else {
		WP* temp2;
		temp2 = head;
		while (temp2->next != NULL){
			temp2 = temp2->next;
		}
		temp2->next = temp;
	}
	return temp;
}
//这个就更是考验基本的链表清空操作了
//分情况讨论 
void free_wp(WP* wp){
	if (wp == NULL){
		assert(0);
	}
	if (wp == head){
		head = head->next;
	} else {
		WP* temp = head;
		while (temp != NULL && temp->next != wp){
			temp = temp->next;
		}
		temp->next = temp->next->next;
	}
	wp->next =free_;
	free_ = wp;
	wp->value_watching = 0;
	wp->expr_watching[0] = '\0';
}

void printf_watching(){
	WP* tmp=head;
	//开始遍历监视点
	if(head==NULL){
		printf("No watching points!\n");
	}else{
	printf("         NO              Address               Enable\n");
	while(tmp!=NULL){
	printf("%d        %s          %d\n",tmp->NO,tmp->expr_watching,tmp->value_watching);
		tmp=tmp->next;
	}
	}
}
WP* delete_wp(int p,bool *key){
	//同样要遍历
	WP* tmp=head;
	while(tmp!=NULL&&tmp->NO!=p){
		tmp=tmp->next;
	}
	if(tmp==NULL){
		*key=false;
	}
	return tmp;//no号的节点没有返回 从列表中消失了
}
WP* set_watchpoint(char *args){
	uint32_t val;
	bool*success=false;
	val=expr(args,success);
	WP* tmp=new_wp();
	tmp->expr_watching=args;
	//printf("%s\n",tmp->expr_watching);
	//printf("%d\n",val);
	tmp->value_watching=val;
	return tmp;
}