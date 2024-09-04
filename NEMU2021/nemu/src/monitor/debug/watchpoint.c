#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "cpu/reg.h"
#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
		wp_pool[i].expr_watching='\0';
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
	int expr_temp;
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

WP* new_wp(){
	//free_  闲置列表 head 使用列表
	WP* tmp1=free_ ;//从尾部取出闲置
	free_=free_->next;//free链表指向新的闲置
	tmp1->next=NULL;
	if(head==NULL){
		//head为空 那么直接插入即可
		head=tmp1;
	}else{
		//否则需要遍历找到尾部 插入
		//需要注意一点的是 并不是用i去遍历（这是物理上的） 
		//而是在链表的逻辑上next遍历
		WP* tmp2;
		tmp2=head;//遍历起点
		while(tmp2->next!=NULL){
			tmp2=tmp2->next;
		}
		tmp2->next=tmp1;//此时tmp2是head使用列表的最后一个 在这里尾插新的链表
	}
	return tmp1;//这里以及成功插入head 返回新插入的可使用列表
	/*for(i=0;i<NR_WP;i++){
		if(wp_pool[i].addr_watching==0){
			return wp_pool[i];
		}
		assert(0);
	}
	*/
}
//这个就更是考验基本的链表清空操作了
//分情况讨论 
void free_wp(WP* wp){
	if(head==NULL){
		assert(0);
	}else if(head==wp){
		head=head->next;
	}else{
		//先找到wp 找到指向wp的节点
		WP* tmp=head;
		while(tmp->next!=wp&&tmp!=NULL){
			tmp=tmp->next;
		}
		//此时tmp下一个是wp
		tmp->next=wp->next;
	}
	//这里用头插法插到free_中
	wp->next=free_;
	free_=wp;//之前的头成功变为wp的下一个
	wp->expr_watching[0]='\0';
	wp->value_watching=0;
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
	printf("%s\n",tmp->expr_watching);
	printf("%d\n",val);
	tmp->value_watching=val;
	return tmp;
}