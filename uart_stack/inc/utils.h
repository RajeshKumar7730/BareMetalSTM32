

#ifndef STACK_UTILS_H
#define STACK_UTILS_H
#include "isotp.h"
#include "uart_stack.h"


#define GET_HEAD(list)  (list)
#define REMOVE_HEAD(list)  {                \
    if(list==NULL) {}                       \
    else{list=list->next; }                  \
}                                           
#define ADD_TO_TAIL(list,ctx)   {           \
    if(list == NULL) {list=ctx;}             \
    else{                                   \
        isotp_ctx_t *tmp = list;            \
        while(tmp->next != NULL) {          \
            tmp = tmp->next;                \
        }                                   \
        tmp->next = ctx;                       \
     }                                          \
}                                               \

#endif