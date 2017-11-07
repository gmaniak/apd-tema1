#include "main.h"
#include <stdlib.h>

struct coord getNextTailPoint(struct snake snake, int** world)
{
	struct coord currentTail;

	//Check next Cell N
	if (currentTail.line - 1 >= 0 && world[currentTail.line - 1][currentTail.col] == snake.encoding) {
		current.line -= 1;
	}
	else if (current.line - 1 < 0 && prevCell != 'N' && world[num_lines - 1][current.col] == snakes[i].encoding) {
		prevCell = 'S';
		current.line = num_lines - 1;
	}

	//Check South Direction
	else if (current.line + 1 < num_lines  && prevCell != 'S' && world[current.line + 1][current.col] == snakes[i].encoding) {
		prevCell = 'N';
		current.line += 1;
	}
	else if (current.line + 1 >= num_lines  && prevCell != 'S' && world[0][current.col] == snakes[i].encoding) {
		prevCell = 'N';
		current.line = 0;
	}

	//Check West direction
	else if (current.col - 1 >= 0 && prevCell != 'W' && world[current.line][current.col - 1] == snakes[i].encoding) {
		prevCell = 'E';
		current.col -= 1;
	}
	else if (current.col - 1 < 0 && prevCell != 'W' && world[current.line][num_cols - 1] == snakes[i].encoding) {
		prevCell = 'E';
		current.col = num_cols - 1;
	}

	//Check East Direction
	else if (current.col + 1 < num_cols && prevCell != 'E' && world[current.line][current.col + 1] == snakes[i].encoding) {
		prevCell = 'W';
		current.col += 1;
	}
	else if (current.col + 1 >= num_cols && prevCell != 'E' && world[current.line][0] == snakes[i].encoding) {
		prevCell = 'W';
		current.col = 0;
	}

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
	int **buffer_world = NULL;
	buffer_world = (int**)malloc(num_lines * sizeof(int*));
	if (buffer_world == NULL)
		return;
	for (int i = 0; i < num_lines; i++) {
		buffer_world[i] = (int*)malloc(num_cols * sizeof(int));
		if (buffer_world[i] == NULL) {
			for (int j = 0; j < i; j++)
				free(buffer_world[j]);
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
			if (current.line - 1 >= 0 && prevCell != 'N' && world[current.line - 1][current.col] == snakes[i].encoding) {
				prevCell = 'S';
				current.line -= 1;
			}
			else if (current.line - 1 < 0 && prevCell != 'N' && world[num_lines - 1][current.col] == snakes[i].encoding) {
				prevCell = 'S';
				current.line = num_lines - 1;
			}

			//Check South Direction
			else if (current.line + 1 < num_lines  && prevCell != 'S' && world[current.line + 1][current.col] == snakes[i].encoding) {
				prevCell = 'N';
				current.line += 1;
			}
			else if (current.line + 1 >= num_lines  && prevCell != 'S' && world[0][current.col] == snakes[i].encoding) {
				prevCell = 'N';
				current.line = 0;
			}

			//Check West direction
			else if (current.col - 1 >= 0 && prevCell != 'W' && world[current.line][current.col - 1] == snakes[i].encoding) {
				prevCell = 'E';
				current.col -= 1;
			}
			else if (current.col - 1 < 0 && prevCell != 'W' && world[current.line][num_cols - 1] == snakes[i].encoding) {
				prevCell = 'E';
				current.col = num_cols - 1;
			}

			//Check East Direction
			else if (current.col + 1 < num_cols && prevCell != 'E' && world[current.line][current.col + 1] == snakes[i].encoding) {
				prevCell = 'W';
				current.col += 1;
			}
			else if (current.col + 1 >= num_cols && prevCell != 'E' && world[current.line][0] == snakes[i].encoding) {
				prevCell = 'W';
				current.col = 0;
			}

			//Tail Found
			else {
				done = 1;
				snakes[i].tail = current;
				snakes[i].tailDirection = prevCell;
			}
		}

	}

	for (int current_step = 0; current_step < step_count; current_step++) {
		//Copy Current World to current step buffer
		for (int i = 0; i < num_lines; i++)
			memcpy(buffer_world[i], world[i], num_cols * sizeof(int));

		//Remove Tails

		//Move Heads

		//Save Iteration
		for (int i = 0; i < num_lines; i++)
			memcpy(world[i], buffer_world[i], num_cols * sizeof(int));
	}

}