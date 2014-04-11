#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#define MAXBUF 1024

int world_size = 0;
int D1 = 0;
int D2 = 0;
int L1 = 0;
int L2 = 0;
int num_of_steps = 0;
int ***array1;
int ***array2;


void read_world(char* filename){
	FILE *fp;
	char buffer[MAXBUF];
	int i, j, k, value;
	fp = fopen(filename, "rt");
	if (fp == NULL) {
		fprintf(stderr, "file not found [%s]\n", filename);
		exit(1);
	}
	fgets(buffer, MAXBUF, fp);
	printf("%s", buffer);
	world_size = atoi(strtok(buffer, " "));
	D1 = atoi(strtok(NULL, " "));
	D2 = atoi(strtok(NULL, " "));
	L1 = atoi(strtok(NULL, " "));
	L2 = atoi(strtok(NULL, " "));
	num_of_steps = atoi(strtok(NULL, " "));
	array1 = (int ***)malloc(world_size*sizeof(int**));
	array2 = (int ***)malloc(world_size*sizeof(int**));

	//Initial state array allocation
	for (i = 0; i< world_size; i++) {

		array1[i] = (int **)malloc(world_size*sizeof(int *));
		array2[i] = (int **)malloc(world_size*sizeof(int *));

		for (j = 0; j < world_size; j++) {

			array1[i][j] = (int *)malloc(world_size*sizeof(int));
			array2[i][j] = (int *)malloc(world_size*sizeof(int));
		}

	}
	//Initial state initialization
	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			fgets(buffer, MAXBUF, fp);
			value = atoi(strtok(buffer, " "));
			for (k = 0; k < world_size; k++)
			{
				array1[i][j][k] = value;
				array2[i][j][k] = value;
				value = atoi(strtok(NULL, " "));

			}
		}
	}
	fclose(fp);
}

/*
finds number of living neighbors for cell at x,y,z position
*/
int check_next_generation(int x, int y, int z, int ***array){
	int i, j, k;
	int counter = 0;
	for (i = x - 1; i <= x + 1; i++)
	{
		if (i < 0 || i > world_size - 1){
			continue;
		}
		for (j = y - 1; j <= y + 1; j++)
		{
			if (j < 0 || j > world_size - 1){
				continue;
			}
			for (k = z - 1; k <= z + 1; k++)
			{
				if (k < 0 || k > world_size - 1){
					continue;
				}
				if (array[i][j][k] == 1){
					counter++;
				}
			}
		}
	}
	return counter;
}

/*
finds new state for each cell
*/
void next_state(int ***array1, int ***array2){
	int i, j, k;
	int living_neighbors = 0;
	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			for (k = 0; k < world_size; k++)
			{
				living_neighbors = check_next_generation(i, j, k, array1);
				//	printf("%d for %s\n", living_neighbors, array[i][j][k].current_value == 0 ? "dead" : "living");
				if (array1[i][j][k] == 0){
					if (L1 < living_neighbors && living_neighbors < L2){
						array2[i][j][k] = 1;
					}
					else{
						array2[i][j][k] = 0;
					}
				}
				else{
					living_neighbors--; //we also counted current living cell
					if (living_neighbors < D1 || living_neighbors > D2){
						array2[i][j][k] = 0;
					}
					else{
						array2[i][j][k] = 1;
					}
				}

			}
		}
	}
}

void print_matricies(){
	int i, j, k;
	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			for (k = 0; k < world_size; k++)
			{

				printf("%d", array1[i][j][k]);
			}
			printf("\n");
		}
	}
	printf("\n");
	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			for (k = 0; k < world_size; k++)
			{

				printf("%d", array2[i][j][k]);
			}
			printf("\n");
		}
	}
	printf("\n");
}

int main(void) {
	int l;
	double time;
  struct timeval lt, ll;

	/* allocate memory and read input data */
	read_world("input.life");
	/* set timer */
  gettimeofday(&lt, NULL);

	/* core part */
	for (l = 0; l < num_of_steps; l++)
	{
		if (l % 2 == 0){
			next_state(array1, array2);
		}
		else{
			next_state(array2, array1);
		}
	}

	/* set timer and print measured time*/
	gettimeofday(&ll, NULL);
	time = (double)(ll.tv_sec - lt.tv_sec) + (double)(ll.tv_usec - lt.tv_usec) / 1000000.0;
	fprintf(stderr, "Time : %.6lf\n", time);
	/* write output file */

	return 0;
}
