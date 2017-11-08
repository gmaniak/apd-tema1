#include "linkedList.h"
#include <stdlib.h>

List newList(struct coord point) {
	List result;
	result = (List)malloc(sizeof(Nod));
	if (result == NULL)
		return NULL;

	result->next = NULL;
	result->point = point;
	return result;
}

List addElement(List root, struct coord point) {
	List newElem;
	newElem = (List)malloc(sizeof(Nod));
	if (newElem == NULL)
		return NULL;
	newElem->next = NULL;
	newElem->point = point;

	List current = root;
	while (current->next != NULL)
		current = current->next;

	current->next = newElem;
	return newElem;
}

List getNext(List element){
	return element->next;
}

void update(List element, struct coord point) {
	element->point = point;
}

List freeList(List element) {
	List next, current;
	current = element;
	next = element->next;
	while (element != NULL){
		next = element->next;
		free(element);
		element = next;
	}
	return NULL;
}

struct coord getValue(List element) {
	return element->point;
}

int hasNext(List element) {
	if (element->next == NULL)
		return 0;
	return 1;
}