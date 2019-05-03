#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/mman.h>

struct tcb
{
    int thread_id;
    int thread_priority;
    ucontext_t *thread_context;
};

void t_init();
void t_create(void (*fct)(int), int id, int pri);
void t_shutdown();
void t_terminate();
void t_yield();

typedef struct tcb tcb;

struct tcbNode
{
    tcb *tcb;
    struct tcbNode *next;
};
typedef struct tcbNode tcbNode;

struct NodeList
{
    struct tcbNode *head, *tail;
};
typedef struct NodeList NodeList;

struct tcbNode *newNode(tcb *tcb);
struct NodeList *createNodeList();
void AddNode(struct NodeList *q, tcb *tcb);
struct tcbNode *RemoveNode(struct NodeList *q);

