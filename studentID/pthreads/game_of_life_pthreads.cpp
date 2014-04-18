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

int change_list_size = 0;
int next_change_list_size = 0;

cells ***cube;
list *change_list;
list *next_change_list = NULL;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutexattr_t attr;


long size_to_handle;

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
					pthread_mutex_lock(&lock);
					cube[i][j][k].num_of_neighbours = cube[i][j][k].num_of_neighbours + value;
					pthread_mutex_unlock(&lock);

				}

			}
		}
	}

}

void read_world(char* filename){
	FILE *fp;
	char *buffer;
	buffer = (char *)malloc(sizeof(char)* MAXBUF);
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
	num_of_threads = atoi(strtok(NULL, " "));
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
			fgets(buffer, MAXBUF, fp);
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

	printf("%d %d\n", cube[0][0][25].num_of_neighbours, cube[0][0][23].status);

	fclose(fp);
}


void init_change_list(){
	int i, j, k;
	int add = 0;				//0 no write to change array, 1 write to array
	list *current_node;

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
					current_node = (list *)malloc(sizeof(struct list));
					//	printf("Adding of node %d %d %d st  %d ns %d\n", cube[i][j][k].x, cube[i][j][k].y, cube[i][j][k].z, cube[i][j][k].status, cube[i][j][k].num_of_neighbours);					
					current_node->cell = &cube[i][j][k];
					current_node->next = change_list;
					change_list = current_node;
					cube[i][j][k].next_gen_status = 1;
					change_list_size++;
					add--;
				}
			}
		}
	}
	current_node = change_list;

}
void print_lists(){
	int i;
	printf("change list elements\n");
	list *l = change_list;
	for (i = 0; i < change_list_size; i++)
	{
		if (l->cell->x == 0 && l->cell->y == 0 && l->cell->z == 2){
			printf("print list %d %d %d %d %d\n", l->cell->x, l->cell->y, l->cell->z, l->cell->status, l->cell->num_of_neighbours);
		}
		l = l->next;


	}
	printf("\n");
	//printf("next change list elements\n");
	//l = next_change_list;
	//for (i = 0; i < next_change_list_size; i++)
	//{
	//		printf("print list %d %d %d %d %d\n", l->cell->x, l->cell->y, l->cell->z, l->cell->status, l->cell->next_gen_status);
	//	l = l->next;
	//}

	//printf("");
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

int check_next_gen(cells *cell, int next_change_list_size){
	int i, j, k;
	list *temp_node;
	//	printf("next gen check %d %d %d\n", cell->x, cell->y, cell->z);
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
							//	printf("Adding of node %d %d %d st  %d ns %d l1 %d l2 %d\n", cube[i][j][k].x, cube[i][j][k].y, cube[i][j][k].z, cube[i][j][k].status, cube[i][j][k].num_of_neighbours, L1,L2);
							temp_node = (list *)malloc(sizeof(struct list));
							if (temp_node == NULL){
								printf("neni misto");
							}
							pthread_mutex_lock(&lock);
							temp_node->cell = &cube[i][j][k];
							temp_node->next = next_change_list;
							next_change_list = temp_node;
							cube[i][j][k].next_gen_status = 1;
							next_change_list_size++;
							pthread_mutex_unlock(&lock);
						}
					}
					else{
						if (cube[i][j][k].num_of_neighbours < D1 || cube[i][j][k].num_of_neighbours > D2){
							//	printf("Adding of node %d %d %d st %d ns %d d1 %d d2 %d \n", cube[i][j][k].x, cube[i][j][k].y, cube[i][j][k].z, cube[i][j][k].status, cube[i][j][k].num_of_neighbours, D1, D2);
							temp_node = (list *)malloc(sizeof(struct list));
							if (temp_node == NULL){
								printf("neni misto");
							}
							pthread_mutex_lock(&lock);
							temp_node->cell = &cube[i][j][k];
							temp_node->next = next_change_list;
							next_change_list = temp_node;
							cube[i][j][k].next_gen_status = 1;
							next_change_list_size++;
							pthread_mutex_unlock(&lock);
						}
					}
				}
			}
		}
	}
	return next_change_list_size;
}

void *next_generation(void *rank){
	int count = 0;
	list *node = (list*)rank;

	while (node && count < size_to_handle)
	{
		node->cell->next_gen_status = 0;
		update_num_of_neighbours(node->cell, node->cell->status == 0 ? 1 : -1);
		//	printf("next generation first loop %d %d %d\n", node->cell->x, node->cell->y, node->cell->z);
		pthread_mutex_lock(&lock);
		node->cell->status = node->cell->status == 0 ? 1 : 0;
		pthread_mutex_unlock(&lock);
		count++;
		node = node->next;
	}
	return NULL;
}




void *next_change_list_cr(void *rank){
	list *node = (list*)rank;
	int count = 0;
	int local_next = 0;
	while (node&& count < size_to_handle)
	{
		//local_next = check_next_gen(node->cell, local_next);
		int i, j, k;
		list *temp_node;
		//	printf("next gen check %d %d %d\n", cell->x, cell->y, cell->z);		
		for (i = node->cell->x - 1; i <= node->cell->x + 1; i++){
			if (i < 0 || i > world_size - 1){ continue; }
			for (j = node->cell->y - 1; j <= node->cell->y + 1; j++){
				if (j < 0 || j > world_size - 1){ continue; }
				for (k = node->cell->z - 1; k <= node->cell->z + 1; k++){
					if (k < 0 || k > world_size - 1){ continue; }
					pthread_mutex_lock(&lock);
					if (cube[i][j][k].next_gen_status == 0){
						if (cube[i][j][k].status == 0){
							if (L1 < cube[i][j][k].num_of_neighbours && cube[i][j][k].num_of_neighbours < L2){
								//	printf("Adding of node %d %d %d st  %d ns %d l1 %d l2 %d\n", cube[i][j][k].x, cube[i][j][k].y, cube[i][j][k].z, cube[i][j][k].status, cube[i][j][k].num_of_neighbours, L1,L2);
								temp_node = (list *)malloc(sizeof(struct list));
								temp_node->cell = &cube[i][j][k];
								temp_node->next = next_change_list;
								next_change_list = temp_node;
								cube[i][j][k].next_gen_status = 1;
								next_change_list_size++;
								pthread_mutex_unlock(&lock);
							}
						}
						else{
							if (cube[i][j][k].num_of_neighbours < D1 || cube[i][j][k].num_of_neighbours > D2){
								//	printf("Adding of node %d %d %d st %d ns %d d1 %d d2 %d \n", cube[i][j][k].x, cube[i][j][k].y, cube[i][j][k].z, cube[i][j][k].status, cube[i][j][k].num_of_neighbours, D1, D2);
								temp_node = (list *)malloc(sizeof(struct list));
								temp_node->cell = &cube[i][j][k];
								pthread_mutex_lock(&lock);
								temp_node->next = next_change_list;
								next_change_list = temp_node;
								cube[i][j][k].next_gen_status = 1;
								next_change_list_size++;
								pthread_mutex_unlock(&lock);
							}
						}
					}
					pthread_mutex_unlock(&lock);
				}
			}
		}

		node = node->next;
		count++;
	}
	//	printf("pocet zmen %d local info %d \n", change_list_size, local_next);

	//print_lists();			
	return NULL;
}






int main(void) {
	int i, j, k, count;					//i,j,k writing to file. Count traverse in change_list_array
	double time;
	FILE *output;
	long thread, thread_c = 0;
	pthread_t *thread_handles;
	list * node;
		struct timeval lt, ll;


	//Muttex init
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&lock, &attr);

	//Thread init
	thread_handles = (pthread_t *)malloc(num_of_threads*sizeof(pthread_t));



	/* allocate memory and read input data */
	read_world("input-100.life");
	

	/* set timer */
		gettimeofday(&lt, NULL);

	/* core part */
	init_change_list();
	for (i = 0; i < num_of_steps; i++)
	{

		size_to_handle = 1 + ((change_list_size - 1) / num_of_threads);
		//	printf("size to handle %d\n", size_to_handle);
		node = change_list;
		for (thread = 0; thread < num_of_threads; thread++)
		{
			pthread_create(&thread_handles[thread], NULL, next_generation, (void *)node);
			//		printf("entering with node %d %d %d\n", node->cell->x, node->cell->y, node->cell->z);
			thread_c++;
			//	printf("%d %d %d\n", current_node_p->cell->x, current_node_p->cell->y, current_node_p->cell->z);
			count = 0;


			while (node && count < size_to_handle)
			{
				node = node->next;
				count++;
			}
			if (node == NULL){
				break;
			}
		}
		for (thread = 0; thread < thread_c; thread++)
		{
			pthread_join(thread_handles[thread], NULL);
		}
		thread_c = 0;
		node = change_list;
		for (thread = 0; thread < num_of_threads; thread++)
		{
			pthread_create(&thread_handles[thread], NULL, next_change_list_cr, (void *)node);
			thread_c++;
			int count = 0;
			while (node && count < size_to_handle)
			{
				node = node->next;
				count++;
			}
			if (node == NULL){
				break;
			}
		}

		for (thread = 0; thread < thread_c; thread++)
		{
			pthread_join(thread_handles[thread], NULL);
		}
		thread_c = 0;
		//	printf("checkcall %d totam sum %d\n", check_call,total_sum);
		change_list = next_change_list;
		change_list_size = next_change_list_size;
		next_change_list_size = 0;
		next_change_list = NULL;

		//next_generation();
		printf("Pruchod cislo %d\n", i);
		//	print_matricies();

	}

	/* set timer and print measured time*/
		gettimeofday(&ll, NULL);
	time = (double)(ll.tv_sec - lt.tv_sec) + (double)(ll.tv_usec - lt.tv_usec) / 1000000.0;
	fprintf(stderr, "Time : %.6lf\n", time);
	/* write output file */
	output = fopen("output_user.life", "w");
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
	//	printf("%d %d", cube[1][4][1].num_of_neighbours, cube[1][4][1].status);
	fclose(output);
	//print_matricies();
	return 0;
}
