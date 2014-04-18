#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define MAXBUF 1024

typedef struct cells{
	int status;				//0 dead, 1 living
	int num_of_neighbours;	//Number of living neighbour cells 
	int next_gen_status;	//Next generation flag
	int x, y, z;			//position in cube

}cells;

typedef struct list{
	cells *cell;
	list *next;
}list;


int world_size = 0;
int D1 = 0;
int D2 = 0;
int L1 = 0;
int L2 = 0;
int num_of_steps = 0;
int num_of_threads = 0;


int semid = 0;

/* Holds all states */
cells ***cube = 0;

/* Holds all changes in current state */
cells **change_list = 0;
int change_list_size = 0;

/* Holds all changes in next state */
cells **next_change_list;
int next_change_list_size = 0;

/* Size to handle in single thread */
long size_to_handle = 0;

/* Mutex part */
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutexattr_t attr;



/*
Finds number of living neighbors for cell at x,y,z position
*/
void update_num_of_neighbours(cells *cell, int value){
	register int i, j, k;
	for (i = cell->x - 1; i <= cell->x + 1; i++)
	{
		/* Position out of cube? */
		if (i < 0 || i > world_size - 1){

			continue;
		}

		for (j = cell->y - 1; j <= cell->y + 1; j++)
		{	
			/* Position out of cube? */
			if (j < 0 || j > world_size - 1){
				continue;
			}
			for (k = cell->z - 1; k <= cell->z + 1; k++)
			{	
				/* Position out of cube? */
				if (k < 0 || k > world_size - 1){
					continue;
				}
				/* Cell [x,y,z] is updated so that we need to change num of neighbours in its neighbours*/
				if (!(i == cell->x && j == cell->y && k == cell->z)){
					pthread_mutex_lock(&lock);
					
					cube[i][j][k].num_of_neighbours += value;					
					pthread_mutex_unlock(&lock);
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

	/* Read problem parameters */
	world_size = atoi(strtok(buffer, " "));
	D1 = atoi(strtok(NULL, " "));
	D2 = atoi(strtok(NULL, " "));
	L1 = atoi(strtok(NULL, " "));
	L2 = atoi(strtok(NULL, " "));
	num_of_steps = atoi(strtok(NULL, " "));
	num_of_threads = atoi(strtok(NULL, " "));
	cube = (cells ***)malloc(world_size*sizeof(cells**));


	/* Initial state array allocation and data init */
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

	/* Initial state initialization of neightbour state */
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

/*
First generation change list creation
*/
void init_change_list(){
	int i, j, k;
	int add = 0;				//0 no write to change array, 1 write to array

	clock_t start = clock();
	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			for (k = 0; k < world_size; k++)
			{
				
				/* Accodring to status apply param for next state */
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
				/* Condition for change in next state is position so that put cell into change list */
				if (add > 0){					
					change_list[change_list_size] = &cube[i][j][k];
					cube[i][j][k].next_gen_status = 1;
					change_list_size++;
					add--;
				}
			}
		}
	}
	clock_t fnisih = clock();
	float time =float (fnisih - start)/ CLOCKS_PER_SEC;
//	printf("init %f\n", time);
}

void print_matricies(){
	int i, j, k;
	for (i = 0; i < world_size; i++)
	{
		for (j = 0; j < world_size; j++)
		{
			for (k = 0; k < world_size; k++)
			{

				printf("%d ", cube[i][j][k].status);
			}
			printf("\n");
		}
	}
	printf("\n");

}

/* 
Update next generation state and num of neightbours in next state
*/
void *next_generation(void *rank){
	int my_rank = (int)rank;
	int cell, my_first, my_last;
	my_first = my_rank*size_to_handle;
	my_last = my_first + size_to_handle > change_list_size ? change_list_size : my_first + size_to_handle;
//	clock_t start = clock();
	/* Update part of change list */
	
	for (cell = my_first; cell < my_last; cell++)	{		
		change_list[cell]->next_gen_status = 0;
		update_num_of_neighbours(change_list[cell], change_list[cell]->status == 0 ? 1 : -1);
		pthread_mutex_lock(&lock);
		
		
		change_list[cell]->status = change_list[cell]->status == 0 ? 1 : 0;
		pthread_mutex_unlock(&lock);
	}
	
//	clock_t finish = clock();
//	float time = (float)(finish - start) / CLOCKS_PER_SEC;
//	printf("update list time %f thread %d\n", time, my_rank);
	return NULL;
}



/*
Create next generation change list
*/
void *next_change_list_cr(void *rank){
	int my_rank = (int)rank;
	int my_first, my_last, cell;
	int i, j, k;
	my_first = my_rank*size_to_handle;
	my_last = my_first + size_to_handle > change_list_size ? change_list_size : my_first + size_to_handle;
	
	/*for (cell = my_first; cell < my_last; cell++)	{
		change_list[cell]->next_gen_status = 0;
		update_num_of_neighbours(change_list[cell], change_list[cell]->status == 0 ? 1 : -1);
		pthread_mutex_lock(&lock);
		change_list[cell]->status = change_list[cell]->status == 0 ? 1 : 0;
		pthread_mutex_unlock(&lock);
	}*/
	
//	clock_t start = clock();
	for (cell = my_first; cell < my_last; cell++)	{	
		for (i = change_list[cell]->x - 1; i <= change_list[cell]->x + 1; i++){
			/* Out of cube? */
			if (i < 0 || i > world_size - 1){ continue; }

			for (j = change_list[cell]->y - 1; j <= change_list[cell]->y + 1; j++){
				/* Out of cube? */
				if (j < 0 || j > world_size - 1){ continue; }

				for (k = change_list[cell]->z - 1; k <= change_list[cell]->z + 1; k++){
					/* Out of cube? */
					if (k < 0 || k > world_size - 1){ continue; }

					pthread_mutex_lock(&lock);
					/* Cell already in next change list */
					if (cube[i][j][k].next_gen_status == 0){

						/* Cell [x,y,z] is updated so that we need to change num of neighbours in its neighbours*/
						if (cube[i][j][k].status == 0){

							if (L1 < cube[i][j][k].num_of_neighbours && cube[i][j][k].num_of_neighbours < L2){								
								next_change_list[next_change_list_size] = &cube[i][j][k];
								cube[i][j][k].next_gen_status = 1;
								next_change_list_size++;								
							}
						}
						else{
							if (cube[i][j][k].num_of_neighbours < D1 || cube[i][j][k].num_of_neighbours > D2){								
								next_change_list[next_change_list_size] = &cube[i][j][k];
								cube[i][j][k].next_gen_status = 1;
								next_change_list_size++;							
							}
						}
					}
					pthread_mutex_unlock(&lock);
				}
			}
		}
	
	}	
//	clock_t finish = clock();
//	float time = (float)(finish - start) / CLOCKS_PER_SEC;
//	printf("next list gen time %f thread %d\n", time, my_rank);
	return NULL;
}

int main(void) {
	int i, j, k;
	double time;
	FILE *output;
	long thread, thread_c = 0;
	pthread_t *thread_handles;
	struct timeval lt, ll;
	int my_position;	

	//Muttex init
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_TIMED_NP);
	pthread_mutex_init(&lock, &attr);

	//Thread init
	thread_handles = (pthread_t *)malloc(num_of_threads*sizeof(pthread_t));



	/* allocate memory and read input data */
	read_world();
	change_list = (cells**)malloc(sizeof(cells*)* world_size*world_size*world_size);
	next_change_list = (cells**)malloc(sizeof(cells*)* world_size*world_size*world_size);

	/* set timer */
	gettimeofday(&lt, NULL);

	/* core part */
	init_change_list();	
	//clock_t start = clock();
	/* Whole lifecycle. hehe */
	for (i = 0; i < num_of_steps; i++)
	{
		printf("Generation number %d\n", i);
		size_to_handle = 1 + ((change_list_size - 1) / num_of_threads); //Single part to solve
		
		/*Update generation and every single neighbour count joint to change of state*/
		for (thread = 0; thread < num_of_threads; thread++)
		{
			pthread_create(&thread_handles[thread], NULL, next_generation, (void *)thread);
		}
		for (thread = 0; thread < num_of_threads; thread++)
		{
			pthread_join(thread_handles[thread], NULL);
		}

		/* Create new change list */
		for (thread = 0; thread < num_of_threads; thread++)
		{
			pthread_create(&thread_handles[thread], NULL, next_change_list_cr, (void *)thread);
		}
		for (thread = 0; thread < num_of_threads; thread++)
		{
			pthread_join(thread_handles[thread], NULL);
		}
		
		/* Copy next change list into change list */
		/*change_list = next_change_list;
		for (int  n= 0; n < next_change_list_size; n++)
		{
			change_list[n] = next_change_list[n];
		}*/
		//clock_t memstart = clock();
		
		memcpy(change_list, next_change_list, next_change_list_size*sizeof(cells**));
		
	//	clock_t memfinish = clock();
	//	float memtime = (float)(memfinish - memstart)/CLOCKS_PER_SEC;
	//	printf("time %f\n", memtime);

		//Reset for next generation
		change_list_size = next_change_list_size;
		next_change_list_size = 0;		
		if (change_list_size == 0){
			break;
		}
				
	}
//	clock_t end = clock();
//	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
//	printf("time %f", seconds);
	/* set timer and print measured time*/

	gettimeofday(&ll, NULL);
	time = (double)(ll.tv_sec - lt.tv_sec) + (double)(ll.tv_usec - lt.tv_usec) / 1000000.0;
	fprintf(stderr, "Time : %.6lf\n", time);

	/* write output file */

	output = fopen("output.life", "w");
	fprintf(output, "%d %d %d %d %d %d %d\n", world_size, D1, D2, L1, L2, num_of_steps, num_of_threads);
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
