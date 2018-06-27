#ifndef __DECISION_H__
#define __DECISION_H__

#include "gf2d_list.h"

List *decision_list_new();

void dicision_list_free(List *decisions);

List *decision_list_load(const char *filename);

void  decision_list_save(const char *filename,List *decisionList);

void decision_list_insert_choice(List *decisions, const char *choice);

void decision_list_update_choice(List *decisions, const char *choice, float success);

const char *decision_list_get_best_choice(List *decisions);



#endif
