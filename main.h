#ifndef __MAIN_H__
#define __MAIN_H__
//#include "linkedList.h"

struct coord
{
	int line;
	int col;
};

typedef struct NodGeneric {
	struct coord point;
	struct NodGeneric* next;
}Nod, *List;

struct snake
{
	struct coord head;
	int encoding;
	char direction;

	//Added tail location
	struct coord tail;

	//Point Order
	List points;
};

void print_world(char *file_name, int num_snakes, struct snake *snakes,
	int num_lines, int num_cols, int **world);

void read_data(char *file_name, int *num_snakes, struct snake **snakes,
	int *num_lines, int *num_cols, int ***world);

void run_simulation(int num_lines, int num_cols, int **world, int num_snakes,
	struct snake *snakes, int step_count, char *file_name);

#endif