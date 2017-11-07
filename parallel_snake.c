#include "main.h"
#include <stdlib.h>
#include <string.h>


//Gets Next Point location based on current point and direction;
struct coord getNextLocation(struct coord point, char direction, int num_lines, int num_cols) {
	struct coord nextPoint;
	nextPoint = point;
	switch (direction) {
	case 'N':
		nextPoint.line = (point.line - 1) % num_lines;
		break;
	case 'S':
		nextPoint.line = (point.line + 1) % num_lines;
		break;
	case 'E':
		nextPoint.col = (point.col + 1) % num_cols;
		break;
	case 'W':
		nextPoint.col = (point.col - 1) % num_cols;
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
	if (world[(currentTail.line - 1) % num_cols][currentTail.col] == snake.encoding)
		returnPoint.line = (currentTail.line - 1) % num_cols;

	//Check South
	else if (world[(currentTail.line + 1) % num_lines][currentTail.col] == snake.encoding)
		returnPoint.line = (currentTail.line + 1) % num_lines;

	//Check East
	else if (world[currentTail.line][(currentTail.col - 1) % num_cols] == snake.encoding)
		returnPoint.col = (currentTail.col - 1) % num_cols;

	//Check West
	else if (world[currentTail.line][(currentTail.col + 1) % num_cols] == snake.encoding)
		returnPoint.col = (currentTail.col + 1) % num_cols;

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

	//Compute Tails - postion and direction
	for (int i = 0; i < num_snakes; i++) {
		int done = 0;
		
		struct coord current;
		current = snakes[i].head;

		char prevCell;
		prevCell = snakes[i].direction;

		while (!done) {
			//Check next Cell N
			if (prevCell != 'N' && world[(current.line - 1) % num_lines][current.col] == snakes[i].encoding) {
				prevCell = 'S';
				current.line = (current.line - 1) % num_lines;
			}

			//Check South Direction
			else if (prevCell != 'S' && world[(current.line + 1) % num_lines][current.col] == snakes[i].encoding) {
				prevCell = 'N';
				current.line = (current.line + 1) % num_lines;
			}

			//Check West direction
			else if ( prevCell != 'W' && world[current.line][(current.col - 1) % num_cols] == snakes[i].encoding) {
				prevCell = 'E';
				current.col = (current.col - 1) % num_cols;
			}

			//Check East Direction
			else if (prevCell != 'E' && world[current.line][(current.col + 1) % num_cols] == snakes[i].encoding) {
				prevCell = 'W';
				current.col = (current.col + 1) % num_cols;
			}

			//Tail Found
			else {
				done = 1;
				snakes[i].tail = current;
			}
		}

	}

	for (int current_step = 0; current_step < step_count; current_step++) {
		//Copy Current World to current step buffer
		for (int i = 0; i < num_lines; i++)
			memcpy(bufferWorld[i], world[i], num_cols * sizeof(int));

		//Remove Tails
		for (int i = 0; i < num_snakes; i++) {
			struct coord newTail;
			newTail = getNextTailPoint(snakes[i], world, num_lines, num_cols);
			bufferWorld[snakes[i].tail.line][snakes[i].tail.col] = 0;
			snakes[i].tail = newTail;
		}

		//Move Heads
		int colision = 0;
		for (int i = 0; i < num_snakes; i++) {
			struct coord nextHead = getNextLocation(snakes[i].head, snakes[i].direction, num_lines, num_cols);
			
			if (bufferWorld[nextHead.line][nextHead.col] != 0)
				colision++;
			else {
				snakes[i].head = nextHead;
				bufferWorld[snakes[i].head.line][snakes[i].head.col] = snakes[i].encoding;
			}
		}

		if (colision) {
			for (int i = 0; i < num_lines; i++)
				free(bufferWorld[i]);
			free(bufferWorld);
			return;
		}
			
		//Save Iteration
		for (int i = 0; i < num_lines; i++)
			memcpy(world[i], bufferWorld[i], num_cols * sizeof(int));
	}

}