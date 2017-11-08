#ifndef __LINKED_LIST__
#define __LINKED_LIST__
#include "main.h"
extern struct coord;

typedef struct NodGeneric{
	struct coord point;
	struct NodGeneric* next;
}Nod, *List;

List newList(struct coord point);
List addElement(List root, struct coord point);
List getNext(List element);
List update(List element, struct coord point);
List freeList(List element);
struct coord getValue(List element);
int hasNext(List element);

#endif
