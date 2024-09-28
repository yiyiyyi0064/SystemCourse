#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char *expr_watching;
	int value_watching;
} WP;

WP* delete_wp(int p,bool *key);
void printf_watching();
void free_wp(WP* wp);
WP* set_watchpoint(char *args);
WP* new_wp();
#endif
