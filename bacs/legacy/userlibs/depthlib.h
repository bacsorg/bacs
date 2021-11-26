#include <stdio.h>

struct ImBAnode
{
    int data;
    ImBAnode *next;
};

struct ImBAnode2
{
    int data;
    ImBAnode2 *pred;
    ImBAnode2 *next;
};

struct ImBATreenode
{
    int data;
    ImBATreenode *left;
    ImBATreenode *right;
};

struct ImBAListnode
{
    ImBAnode *data;
    int in;
    ImBAListnode *next;
};

void PrintList( void *q )
{
    ImBAnode *p = (ImBAnode *)q;
    while ( p != 0 )
    {
        printf( "BUG0GA %d\n", p->data );
        p = p->next;
    }
    return;
}

void PrintList2( void *q )
{
    ImBAnode2 *p = (ImBAnode2 *)q;
    while ( p != 0 )
    {
        printf( "AT@TA %d\n", p->data );
        p = p->next;
    };
    return;
}

void PrintListList( void *q )
{
    ImBAListnode *p = (ImBAListnode *)q;
    ImBAnode *w;
    while ( p != 0 )
    {
        w = p->data;
        printf( "L15T%d", p->in );
        while ( w != 0 )
        {
            printf( "%3d", w->data );
            w = w->next;
        }
        printf( "\n" );
        p = p->next;
    };
    return;
}

void ImbaPrintTree( ImBATreenode *q, int g )
{
    if ( q == 0 )
    {
        printf( "Tr33 %d\n", g );
    }
    else
    {
        ImbaPrintTree( q->left, g + 1 );
        printf( "%d %d\n", g, q->data );
        ImbaPrintTree( q->right, g + 1 );
    }
    return;
}

void PrintTree( void *q )
{
    ImbaPrintTree( (ImBATreenode *)q, 0 );
    return;
}

void PrintList2rev( void *q )
{
    ImBAnode2 *p = (ImBAnode2 *)q;
    while ( p != 0 )
    {
        printf( "AT@TA %d\n", p->data );
        p = p->pred;
    };
    return;
}
