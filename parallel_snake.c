#include "main.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <omp.h>


#define LENGHT_ONE -2

//Redefine modulo so it work on negative numbers as expected
#define modulo(a,b) ((a)<0 ? (a+b)%b : (a)%b)

void run_simulation(int num_lines, int num_cols, int **world, int num_snakes,
	struct snake *snakes, int step_count, char *file_name) 
{
	// TODO: Implement Parallel Snake simulation using the default (env. OMP_NUM_THREADS) 
	// number of threads.
	//
	// DO NOT include any I/O stuff here, but make sure that world and snakes
	// parameters are updated as required for the final state.

	int i;
	int collision = 0;

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
			snakes[i].prevTail = snakes[i].tail;
			world[snakes[i].tail.line][snakes[i].tail.col] = 0;

			//Check North
			if (world[modulo(snakes[i].tail.line - 1, num_cols)][snakes[i].tail.col] == snakes[i].encoding)
				snakes[i].tail.line = modulo(snakes[i].tail.line - 1, num_cols);

			//Check South
			else if (world[modulo(snakes[i].tail.line + 1, num_lines)][snakes[i].tail.col] == snakes[i].encoding)
				snakes[i].tail.line = modulo(snakes[i].tail.line + 1, num_lines);

			//Check East
			else if (world[snakes[i].tail.line][modulo(snakes[i].tail.col - 1, num_cols)] == snakes[i].encoding)
				snakes[i].tail.col = modulo(snakes[i].tail.col - 1, num_cols);

			//Check West
			else if (world[snakes[i].tail.line][modulo(snakes[i].tail.col + 1, num_cols)] == snakes[i].encoding)
				snakes[i].tail.col = modulo(snakes[i].tail.col + 1, num_cols);

			//Check if Snakes tail coresponds with head
			else 
				snakes[i].tail.line = LENGHT_ONE;

		}

		//Move Heads
		#pragma omp parallel for private(i)
		for (i = 0; i < num_snakes; i++) {
			switch (snakes[i].direction) {
			case 'N':
				snakes[i].head.line = modulo(snakes[i].head.line - 1, num_lines);
				break;
			case 'S':
				snakes[i].head.line = modulo(snakes[i].head.line + 1, num_lines);
				break;
			case 'E':
				snakes[i].head.col = modulo(snakes[i].head.col + 1, num_cols);
				break;
			case 'V':
				snakes[i].head.col = modulo(snakes[i].head.col - 1, num_cols);
				break;
			}

			//Update tail if necesary
			if (snakes[i].tail.line == LENGHT_ONE)
				snakes[i].tail = snakes[i].head;

			#pragma omp atomic
			world[snakes[i].head.line][snakes[i].head.col] += snakes[i].encoding;
		}

		//Check Collision
		#pragma omp parallel for private(i) shared(collision)
		for (i = 0; i < num_snakes; i++) {
			if (world[snakes[i].head.line][snakes[i].head.col] != snakes[i].encoding) {
				#pragma omp atomic
				collision++;
			}
		}

		if (!collision) {
			
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
	}
}
