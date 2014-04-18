#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#define MAXBUF 1024

typedef struct cells{
	int status;				//0 dead, 1 living
	int num_of_neighbours;	//Number of living neighbour cells 
	int next_gen_status;	//Next generation flag
	int x, y, z;			//position in cube

}cells;

int world_size = 0;
int D1 = 0;
int D2 = 0;
int L1 = 0;
int L2 = 0;
int num_of_steps = 0;
int num_of_threads = 0;

int change_list_size = 0;
int next_change_list_size = 0;
cells ***cube;
cells **change_list;
cells **next_change_list;

/*
finds number of living neighbors for cell at x,y,z position
*/
void update_num_of_neighbours(cells *cell, int value){
	int i, j, k;
	for (i = cell->x - 1; i <= cell->x + 1; i++)
	{
		if (i < 0 || i > world_size - 1){
			continue;
		}
		for (j = cell->y - 1; j <= cell->y + 1; j++)
		{
			if (j < 0 || j > world_size - 1){
				continue;
			}
			for (k = cell->z - 1; k <= cell->z + 1; k++)
			{
				if (k < 0 || k > world_size - 1){
					continue;
				}
				if (!(i == cell->x && j == cell->y && k == cell->z)){
					cube[i][j][k].num_of_neighbours += value;					
				}                                          
			}
		}
	}                                                
}

void read_world(){	
	char *buffer;
	int i, j, k, value;

	buffer = (char *)malloc(sizeof(char)* MAXBUF);	
	fgets(buffer, MAXBUF, stdin);

	world_size = atoi(strtok(buffer, " "));
	D1 = atoi(strtok(NULL, " "));
	D2 = atoi(strtok(NULL, " "));
	L1 = atoi(strtok(NULL, " "));
	L2 = atoi(strtok(NULL, " "));
	num_of_steps = atoi(strtok(NULL, " "));

	cube = (cells ***)malloc(world_size*sizeof(cells**));

	//Initial state array allocation
	for (i = 0; i < world_size; i++) {
		cube[i] = (cells **)malloc(world_size*sizeof(cells *));
		for (j = 0; j < world_size; j++) {
			cube[i][j] = (cells *)malloc(world_size*sizeof(cells));
			for (k = 0; k < world_size; k++)
			{
				cube[i][j][k].num_of_neighbours = 0;
				cube[i][j][k].status = 0;
				cube[i][j][k].next_gen_status = 0;
				cube[i][j][k].x = i;
				cube[i][j][k].y = j;
				cube[i][j][k].z = k;

			}
		}

	}
	//Initial state initialization
	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			fgets(buffer, MAXBUF, stdin);
			value = atoi(strtok(buffer, " "));
			for (k = 0; k < world_size; k++)
			{
				cube[i][j][k].status = value;
				if (value == 1){
					update_num_of_neighbours(&cube[i][j][k], 1);
				}
				value = atoi(strtok(NULL, " "));

			}
		}
	}
}


void init_change_list(){
	int i, j, k;
	int add = 0;

	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			for (k = 0; k < world_size; k++)
			{
				if (cube[i][j][k].status == 0){
					if (L1 < cube[i][j][k].num_of_neighbours && cube[i][j][k].num_of_neighbours < L2){
						add++;
					}
				}
				else{
					if (cube[i][j][k].num_of_neighbours < D1 || cube[i][j][k].num_of_neighbours > D2){
						add++;
					}
				}
				if (add > 0){
					change_list[change_list_size] = &cube[i][j][k];					
					cube[i][j][k].next_gen_status = 1;
					change_list_size++;
					add--;
				}
			}
		}
	}
}

void check_next_gen(cells *cell){
	int i, j, k;
	
	for (i = cell->x - 1; i <= cell->x + 1; i++)
	{
		if (i < 0 || i > world_size - 1){
			continue;
		}
		for (j = cell->y - 1; j <= cell->y + 1; j++)
		{
			if (j < 0 || j > world_size - 1){
				continue;
			}
			for (k = cell->z - 1; k <= cell->z + 1; k++)
			{
				if (k < 0 || k > world_size - 1){
					continue;
				}
				if (cube[i][j][k].next_gen_status == 0){
					if (cube[i][j][k].status == 0){						
						if (L1 < cube[i][j][k].num_of_neighbours && cube[i][j][k].num_of_neighbours < L2){					
							next_change_list[next_change_list_size] = &cube[i][j][k];														
							cube[i][j][k].next_gen_status = 1;
							next_change_list_size++;
						}
					}
					else{
						if (cube[i][j][k].num_of_neighbours < D1 || cube[i][j][k].num_of_neighbours > D2){
							next_change_list[next_change_list_size]= &cube[i][j][k];							
							cube[i][j][k].next_gen_status = 1;
							next_change_list_size++;
						}
					}
				}
			}
		}
	}
}

void next_generation(){
	int i, j, k;	
	for (i = 0; i < change_list_size; i++)
	{
		change_list[i]->next_gen_status = 0;
		update_num_of_neighbours(change_list[i], change_list[i]->status == 0 ? 1 : -1);
		change_list[i]->status = change_list[i]->status == 0 ? 1 : 0;		
	}	
	for (i = 0; i < change_list_size; i++)
	{
		check_next_gen(change_list[i]);		
	}
	memcpy(change_list, next_change_list, next_change_list_size*sizeof(cells**));
	change_list_size = next_change_list_size;
	next_change_list_size = 0;  
}

int main(void) {
	int i, j, k;
	double time;
	FILE *output;
  struct timeval lt, ll;

	/* allocate memory and read input data */
	
	read_world();
	change_list = (cells**)malloc(sizeof(cells*)* world_size*world_size*world_size);
	next_change_list = (cells**)malloc(sizeof(cells*)* world_size*world_size*world_size);

	/* set timer */
  gettimeofday(&lt, NULL);
	

	/* core part */
	init_change_list();	
	for (i = 0; i < num_of_steps; i++)
	{	
		next_generation();
    if (change_list_size == 0){
			break;
		}	
	}

	/* set timer and print measured time*/
	gettimeofday(&ll, NULL);
	time = (double)(ll.tv_sec - lt.tv_sec) + (double)(ll.tv_usec - lt.tv_usec) / 1000000.0;
	fprintf(stderr, "Time : %.6lf\n", time);
  
  /* write output file */
	output = fopen("output.life", "w");
	fprintf(output, "%d %d %d %d %d %d 1\n", world_size, D1, D2, L1, L2, num_of_steps);
	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			for (k = 0; k < world_size; k++)
			{
				fprintf(output, "%d ", cube[i][j][k].status);
			}
			fprintf(output, "\n");
		}
	}
	fclose(output);
	return 0;
}
