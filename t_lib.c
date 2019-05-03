#define _XOPEN_SOURCE 600
#include "t_lib.h"
#include <signal.h>

struct tcb *running;
struct NodeList *ready;

void t_init()
{
    ready = createNodeList();
    tcb *tmp = (tcb *) malloc(sizeof(struct tcb));
    tmp->thread_id = 0;
    tmp->thread_priority = 0;
    ucontext_t *uc;
    uc = (ucontext_t *) malloc(sizeof(ucontext_t));
    
    getcontext(uc);
    tmp->thread_context = uc;
    running = tmp;
    free(tmp->thread_id);
    free(tmp->thread_priority);
    free(tmp);
    free(uc);
}

void t_shutdown()
{
    struct tcb *tmp;
    struct tcbNode *tmp2 = ready->head;

    while(tmp2 != NULL)
    {
        tmp = tmp2->tcb;
        tmp2 = tmp2->next;
    }
    
    free(tmp2->tcb);
    free(tmp2->next);
    free(tmp2);
    
    tmp = running;
    if(tmp->thread_id > 0) {free(tmp->thread_context->uc_stack.ss_sp);}
    
    free(tmp->thread_context);
    free(tmp->thread_id);
    free(tmp);
    free(ready);
}

void t_create(void (*fct)(int), int id, int pri)
{
    size_t sz = 0x10000;
    ucontext_t *uc;
    tcb *new_tcb;
    uc = (ucontext_t *) malloc(sizeof(ucontext_t));
    getcontext(uc);
    uc->uc_stack.ss_sp = malloc(sz);
    uc->uc_stack.ss_size = sz;
    uc->uc_stack.ss_flags = 0;
    uc->uc_link = running;
    makecontext(uc, (void (*)(void)) fct, 1, id);
    new_tcb = (tcb *) malloc(sizeof(tcb));
    new_tcb->thread_id = id;
    new_tcb->thread_priority = pri;
    new_tcb->thread_context = uc;
    AddNode(ready, new_tcb);
    free(uc);
}

void t_terminate()
{
    struct tcb *tmp;
    struct tcbNode *tmp2;
    
    tmp = running;
    
    free(tmp->thread_context->uc_stack.ss_sp);
    free(tmp->thread_context);
    free(tmp);
    
    tmp2 = RemoveNode(ready);
    
    running = tmp2->tcb;
    
    free(tmp2);
    free(tmp);
    setcontext(running->thread_context);
}


void t_yield()
{
    struct tcb *tmp;
    
    tmp = running;
    
    struct tcbNode *new_running = RemoveNode(ready);
    
    if(new_running != NULL)
    {
        running = new_running->tcb;
        AddNode(ready, tmp);
        swapcontext(tmp->thread_context, running->thread_context);
    }
}

////////////////////
// Nodes Creation //
////////////////////

struct tcbNode *newNode(tcb *tcb)
{
    struct tcbNode *temp = (struct tcbNode*)malloc(sizeof(struct tcbNode));
    
    temp->tcb = tcb;
    temp->next = NULL;

    return temp;
}

struct NodeList *createNodeList()
{
    struct NodeList *q = (struct NodeList*)malloc(sizeof(struct NodeList));
    q->head = q->tail = NULL;

    return q;
}

void AddNode(struct NodeList *q, tcb *tcb)
{
    struct tcbNode *temp = newNode(tcb);
    
    if (q->tail == NULL)
    {
        q->head = q->tail = temp;
        return;
    }

    q->tail->next = temp;
    q->tail = temp;
}

struct tcbNode *RemoveNode(struct NodeList *q)
{
    if (q->head == NULL)
    {
        return NULL;
    }
    
    struct tcbNode *temp = q->head;
    q->head = q->head->next;
    
    if (q->head == NULL)
    {
        q->tail = NULL;
    }
    
    return temp;
}

