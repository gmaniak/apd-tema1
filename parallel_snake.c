#include "main.h"
//#include "linkedList.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//List Function Definitions
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

List getNext(List element) {
	return element->next;
}

void update(List element, struct coord point) {
	element->point = point;
}

List freeList(List element) {
	List next, current;
	current = element;
	next = element->next;
	while (element != NULL) {
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
//End List Function


//Custom Modulo Function, made to work with -1
int modulo(int nrA, int nrB) {
	if (nrA < 0)
		return (nrA + nrB) % nrB;
	return nrA % nrB;
}


//Gets Next Point location based on current point and direction;
struct coord getNextLocation(struct coord point, char direction, int num_lines, int num_cols) {
	struct coord nextPoint;
	nextPoint = point;
	switch (direction) {
	case 'N':
		nextPoint.line = modulo(point.line - 1,num_lines);
		break;
	case 'S':
		nextPoint.line = modulo(point.line + 1 , num_lines);
		break;
	case 'E':
		nextPoint.col = modulo(point.col + 1 , num_cols);
		break;
	case 'V':
		nextPoint.col = modulo(point.col - 1 , num_cols);
		break;
	}
	return nextPoint;
}


//Gets the next Tail point based on the current tail location
struct coord getNextTailPoint(struct snake snake, int** world,int num_lines,int num_cols)
{
	struct coord currentTail,returnPoint;
	currentTail = snake.tail;
	returnPoint = currentTail;

	//Check North
	if (world[modulo(currentTail.line - 1 , num_cols)][currentTail.col] == snake.encoding)
		returnPoint.line = modulo(currentTail.line - 1, num_cols);

	//Check South
	else if (world[modulo(currentTail.line + 1 , num_lines)][currentTail.col] == snake.encoding)
		returnPoint.line = modulo(currentTail.line + 1, num_lines);

	//Check East
	else if (world[currentTail.line][modulo(currentTail.col - 1, num_cols)] == snake.encoding)
		returnPoint.col = modulo(currentTail.col - 1, num_cols);

	//Check West
	else if (world[currentTail.line][modulo(currentTail.col + 1, num_cols)] == snake.encoding)
		returnPoint.col = modulo(currentTail.col + 1, num_cols);

	//Check if Snakes tail coresponds with head
	else if (snake.head.line == snake.tail.line && snake.head.col == snake.tail.col)
		returnPoint = getNextLocation(snake.head, snake.direction, num_lines, num_cols);

	return returnPoint;
}

void run_simulation(int num_lines, int num_cols, int **world, int num_snakes,
	struct snake *snakes, int step_count, char *file_name) 
{
	// TODO: Implement Parallel Snake simulation using the default (env. OMP_NUM_THREADS) 
	// number of threads.
	//
	// DO NOT include any I/O stuff here, but make sure that world and snakes
	// parameters are updated as required for the final state.


	//Allocate World Buffer Matrix;
	int **bufferWorld = NULL;
	bufferWorld = (int**)malloc(num_lines * sizeof(int*));
	if (bufferWorld == NULL)
		return;
	for (int i = 0; i < num_lines; i++) {
		bufferWorld[i] = (int*)malloc(num_cols * sizeof(int));
		if (bufferWorld[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(bufferWorld[j]);
			return;
		}
	}
	//Allocate Snake buffer
	struct snake* bufferSnakes = NULL;
	bufferSnakes = (struct snake*)malloc(num_snakes * sizeof(struct snake));
	if (bufferSnakes == NULL) {
		for (int i = 0; i < num_lines; i++)
			free(bufferWorld[i]);
		free(bufferWorld);
		return;
	}


	//Compute Linked List for Snake Points
	for (int i = 0; i < num_snakes; i++) {
		snakes[i].points = newList(snakes[i].head);
		List lastElem = snakes[i].points;

		int done = 0;
		
		struct coord current;
		current = snakes[i].head;

		char prevCell;
		prevCell = snakes[i].direction;

		while (!done) {
			//Check next Cell N
			if (prevCell != 'N' && world[modulo(current.line - 1, num_lines)][current.col] == snakes[i].encoding) {
				prevCell = 'S';
				current.line = modulo(current.line - 1, num_lines);
			}

			//Check South Direction
			else if (prevCell != 'S' && world[modulo(current.line + 1, num_lines)][current.col] == snakes[i].encoding) {
				prevCell = 'N';
				current.line = modulo(current.line + 1, num_lines);
			}

			//Check West direction
			else if ( prevCell != 'V' && world[current.line][modulo(current.col - 1, num_cols)] == snakes[i].encoding) {
				prevCell = 'E';
				current.col = modulo(current.col - 1, num_cols);
			}

			//Check East Direction
			else if (prevCell != 'E' && world[current.line][modulo(current.col + 1, num_cols)] == snakes[i].encoding) {
				prevCell = 'V';
				current.col = modulo(current.col + 1, num_cols);
			}

			//Tail Found
			else {
				done = 1;
				snakes[i].tail = current;
				break;
			}
			
			//Add Point to List
			lastElem = addElement(lastElem, current);
		}

	}

	for (int current_step = 0; current_step < step_count; current_step++) {
		//Copy Current World to current step buffer
		for (int i = 0; i < num_lines; i++)
			memcpy(bufferWorld[i], world[i], num_cols * sizeof(int));

		//Copy snakes to snakes buffer
		memcpy(bufferSnakes, snakes, num_snakes * sizeof(struct snake));
		
		//Remove Tails
		for (int i = 0; i < num_snakes; i++) 
			bufferWorld[bufferSnakes[i].tail.line][bufferSnakes[i].tail.col] = 0;

		//Move Heads
		int colision = 0;
		for (int i = 0; i < num_snakes; i++) {
			struct coord nextHead = getNextLocation(bufferSnakes[i].head, bufferSnakes[i].direction, num_lines, num_cols);
			
			if (bufferWorld[nextHead.line][nextHead.col] != 0)
				colision++;
			else {
				bufferSnakes[i].head = nextHead;
				bufferWorld[bufferSnakes[i].head.line][bufferSnakes[i].head.col] = bufferSnakes[i].encoding;
			}
		}

		if (colision) {
			for (int i = 0; i < num_lines; i++)
				free(bufferWorld[i]);
			free(bufferWorld);
			free(bufferSnakes);
			for (int i = 0; i < num_snakes; i++)
				freeList(snakes[i].points);
			
			return;
		}

		//Update Snake Points Location
		for (int i = 0; i < num_snakes; i++) {
			struct coord newPoint,aux;
			List lastElement;

			lastElement = bufferSnakes[i].points;
			newPoint = bufferSnakes[i].head;
			
			//Update List for each snake
			while (lastElement != NULL) {
				//update values
				aux = getValue(lastElement);
				update(lastElement, newPoint);

				//Update Tail Value
				if (!hasNext(lastElement))
					bufferSnakes[i].tail = newPoint;

				//Iterate
				newPoint = aux;
				lastElement = getNext(lastElement);
			}
		}
			
		//Save Iteration
		for (int i = 0; i < num_lines; i++)
			memcpy(world[i], bufferWorld[i], num_cols * sizeof(int));
		memcpy(snakes, bufferSnakes, num_snakes * sizeof(struct snake));
	}

	//Free memory
	for (int i = 0; i < num_lines; i++)
		free(bufferWorld[i]);
	free(bufferWorld);
	for (int i = 0; i < num_snakes; i++)
		freeList(snakes[i].points);
}