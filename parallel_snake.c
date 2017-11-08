#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <omp.h>

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
struct coord getNextTailPoint(struct snake snake, int** world, int num_lines, int num_cols)
{
	struct coord currentTail, returnPoint;
	currentTail = snake.tail;
	returnPoint = currentTail;

	//Check North
	if (world[modulo(currentTail.line - 1, num_cols)][currentTail.col] == snake.encoding)
		returnPoint.line = modulo(currentTail.line - 1, num_cols);

	//Check South
	else if (world[modulo(currentTail.line + 1, num_lines)][currentTail.col] == snake.encoding)
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


void undoWorld(int** world, struct snake* snakes, int num_lines, int num_cols, int num_snakes){ 
	int i;
	//undoHead Write and HeadPosition
	#pragma omp parallel for private(i)
	for (i = 0; i < num_snakes; i++) {
		#pragma omp atomic
		world[snakes[i].head.line][snakes[i].head.col] -= snakes[i].encoding;
	
		switch (snakes[i].direction)
		{
		case 'N':
			snakes[i].head.line = modulo(snakes[i].head.line + 1, num_lines);
			break;
		case 'S':
			snakes[i].head.line = modulo(snakes[i].head.line - 1, num_lines);
			break;
		case 'E':
			snakes[i].head.col = modulo(snakes[i].head.col - 1, num_cols);
			break;
		case 'V':
			snakes[i].head.col = modulo(snakes[i].head.col + 1, num_cols);
			break;
		}
	}

	//undoTails Delete

	#pragma omp parallel for private(i)
	for (i = 0; i < num_snakes; i++) 
		world[snakes[i].prevTail.line][snakes[i].prevTail.col] = snakes[i].encoding;
}

void run_simulation(int num_lines, int num_cols, int **world, int num_snakes,
	struct snake *snakes, int step_count, char *file_name) 
{
	// TODO: Implement Parallel Snake simulation using the default (env. OMP_NUM_THREADS) 
	// number of threads.
	//
	// DO NOT include any I/O stuff here, but make sure that world and snakes
	// parameters are updated as required for the final state.

	int i;

	//Compute Tails for Snakes
	int done;
	struct coord current;
	char prevCell;
	
	#pragma omp parallel for private(i,done,current,prevCell)
	for (i = 0; i < num_snakes; i++) {
		done = 0;
		current = snakes[i].head;
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
			}
		}

	}

	//Execute Each Step
	for (int current_step = 0; current_step < step_count; current_step++) {
		
		//Remove Tails
		struct coord newTail;
		#pragma omp parallel for private(i,newTail)
		for (int i = 0; i < num_snakes; i++) {
			newTail = getNextTailPoint(snakes[i], world, num_lines, num_cols);
			world[snakes[i].tail.line][snakes[i].tail.col] = 0;
			snakes[i].prevTail = snakes[i].tail;
			snakes[i].tail = newTail;
		}

		//Move Heads
		struct coord nextHead;
		#pragma omp parallel for private(i,nextHead)
		for (i = 0; i < num_snakes; i++) {
			nextHead = getNextLocation(snakes[i].head, snakes[i].direction, num_lines, num_cols);
			snakes[i].head = nextHead;

			#pragma omp atomic
			world[snakes[i].head.line][snakes[i].head.col] += snakes[i].encoding;
		}

		//Check Collision
		int collision = 0;
		#pragma omp parallel for private(i) shared(collision)
		for (i = 0; i < num_snakes; i++) {
			if (world[snakes[i].head.line][snakes[i].head.col] != snakes[i].encoding) {
				#pragma omp atomic
				collision++;
			}
		}

		if (!collision)
			undoWorld(world, snakes, num_lines, num_cols, num_snakes);			
	}
}
