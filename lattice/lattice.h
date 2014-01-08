
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static const int N=100;
#define LATTICE 4
#define NSAMPLE 10
#define MONOMER 0
#define DIMER 1

typedef struct point
{
	int x;
	int y;
}point;

typedef struct gcsize
{
	int maxsize;
	int monosize;
	int dimersize;
}gcsize;

typedef struct node
{
	//point id;
	point inter;
	point base[LATTICE];
	bool alive;
	int cluster;
	int type;
	//int gaint;
}node;

/*********************************************************************/
#define	STACK_INCREASEMENT N
#define STACK_INITIAL N*10
typedef struct stack
{
	point *head, *tail;
	int size;
	int empty;
}stack;

void stack_init(stack *ps)
{
	ps->empty = STACK_INITIAL;
	ps->size=STACK_INITIAL;
	ps->head=(point*)malloc(sizeof(point)*STACK_INITIAL);
	ps->tail=ps->head-1;//+s.len;
}

void stack_push(stack *ps, point pt)
{
	if (ps->empty<1) {
		ps->head = (point *)realloc(ps->head, sizeof(point) * (ps->size+STACK_INCREASEMENT));
		if (ps->head == NULL)
			printf("error");
		//memcpy(tmp, ps->head, ps->size);
		//ps->head = tmp;
		ps->tail = ps->head + ps->size - 1;
		ps->size += STACK_INCREASEMENT;
		ps->empty = STACK_INCREASEMENT;

	}
	ps->tail++;
	ps->tail->x = pt.x;
	ps->tail->y = pt.y;
	ps->empty--;
}

point stack_pop(stack *ps)
{
	point pt;
	pt.x = ps->tail->x;
	pt.y = ps->tail->y;
	ps->tail--;
	ps->empty++;
	return pt;
}

void stack_release(stack *ps)
{
	if (ps->head != NULL) {
		free(ps->head);
		ps->head = NULL;
		ps->tail = NULL;
		ps->size = 0;
		ps->empty = 0;
	}
}

int stack_len(stack *ps)
{
	return ps->tail - ps->head+1;
}


