#include <cstdio>

void print_queue2(void *q)
{
    struct State
    {
        int ip, next1, next, v;
        char com[5];
    };

    struct GG
    {
        State state1, state2;
        int vars;
    };

    struct List
    {
        GG data;
        List *next;
    };

    List *queue = (List *)q;
    List *t;
    for (t = queue; t != NULL; t = t->next)
        printf("|%d %d %d|", t->data.state1.ip, t->data.state2.ip, t->data.vars);
}

void print_queue1(void *q)
{
    struct State
    {
        int ip, next1, next, v;
        char com[5];
    };

    struct List
    {
        State data;
        List *next;
    };

    List *queue = (List *)q;
    List *t;
    for (t = queue; t != NULL; t = t->next)
        printf("|%d|", t->data.ip);
}
