#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>

#define MAXBUF 1024

typedef struct cell{
	int status;				//0 dead, 1 living
	int num_of_neighbours;	//Number of living neighbour cells 
	int next_gen_status;	//Next generation flag
	int x, y, z;			//position in cube

}cell;
typedef struct change list;
struct list{
	cell *cell;
	list *next;
};

int world_size = 0;
int D1 = 0;
int D2 = 0;
int L1 = 0;
int L2 = 0;
int num_of_steps = 0;

int change_list_size = 0;
int next_change_list_size = 0;
cell ***cube;
list *change_list;
list *current_node = NULL;
list *next_change_list = NULL;


/*
finds number of living neighbors for cell at x,y,z position
*/
void update_num_of_neighbours(cell *cell, int value){
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
					cube[i][j][k].num_of_neighbours = cube[i][j][k].num_of_neighbours + value;
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
	cube = (cell ***)malloc(world_size*sizeof(cell**));


	//Initial state array allocation
	for (i = 0; i < world_size; i++) {
		cube[i] = (cell **)malloc(world_size*sizeof(cell *));
		for (j = 0; j < world_size; j++) {
			cube[i][j] = (cell *)malloc(world_size*sizeof(cell));
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
					current_node = (list *)malloc(sizeof(struct list));
					//	printf("Adding of node %d %d %d st  %d ns %d\n", cube[i][j][k].x, cube[i][j][k].y, cube[i][j][k].z, cube[i][j][k].status, cube[i][j][k].num_of_neighbours);
					if (i == 0 && j == 0 && k == 2){
						printf("%d", i);
					}
					current_node->cell = &cube[i][j][k];
					current_node->next = change_list;
					change_list = current_node;
					cube[i][j][k].next_gen_status = 1;
					change_list_size++;
					if (i == 0 && j == 0 && k == 2){
						printf("%d %d %d\n", 0, 0, cube[i][j][k].next_gen_status);
					}
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

void check_next_gen(cell *cell){
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
							temp_node->cell = &cube[i][j][k];
							temp_node->next = next_change_list;
							next_change_list = temp_node;
							cube[i][j][k].next_gen_status = 1;
							next_change_list_size++;
						}
					}
					else{
						if (cube[i][j][k].num_of_neighbours < D1 || cube[i][j][k].num_of_neighbours > D2){
							//	printf("Adding of node %d %d %d st %d ns %d d1 %d d2 %d \n", cube[i][j][k].x, cube[i][j][k].y, cube[i][j][k].z, cube[i][j][k].status, cube[i][j][k].num_of_neighbours, D1, D2);
							temp_node = (list *)malloc(sizeof(struct list));
							if (temp_node == NULL){
								printf("neni misto");
							}
							temp_node->cell = &cube[i][j][k];
							temp_node->next = next_change_list;
							next_change_list = temp_node;
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
	current_node = change_list;
	for (i = 0; i < change_list_size; i++)
	{
		current_node->cell->next_gen_status = 0;
		update_num_of_neighbours(current_node->cell, current_node->cell->status == 0 ? 1 : -1);
		current_node->cell->status = current_node->cell->status == 0 ? 1 : 0;
		list *l = current_node;
		/*while (l)
		{
		printf("print list %d %d %d %d %d\n", l->cell->x, l->cell->y, l->cell->z, l->cell->status, l->cell->num_of_neighbours);
		l = l->next;
		}*/
		current_node = current_node->next;

		//	printf("\n");
	}

	current_node = change_list;
	for (i = 0; i < change_list_size; i++)
	{
		//	printf("node in list %d %d %d status %d  next %d", current_node->cell->x, current_node->cell->y, current_node->cell->z,current_node->cell->status, current_node->cell->next_gen_status);
		/*
		if (current_node->cell->status == 0){
		current_node->cell->status = 1;
		}
		else{
		current_node->cell->status = 0;
		}*/
		check_next_gen(current_node->cell);
		current_node = current_node->next;
	}

	current_node = change_list;
	//print_lists();
	change_list = next_change_list;
	current_node = next_change_list;
	//	printf("\n");
	change_list_size = next_change_list_size;
	next_change_list_size = 0;
	next_change_list = NULL;
}

int main(void) {
	int i, j, k;
	double time;
	FILE *output;
	struct timeval lt, ll;

	/* allocate memory and read input data */
	//read_world("input.life");
	read_world("input-100.life");

	/* set timer */
	gettimeofday(&lt, NULL);

	/* core part */
	init_change_list();
	for (i = 0; i < num_of_steps; i++)
	{
		next_generation();
		printf("Pruchod cislo %d\n", i);
		//print_matricies();

	}

	/* set timer and print measured time*/
	gettimeofday(&ll, NULL);
	time = (double)(ll.tv_sec - lt.tv_sec) + (double)(ll.tv_usec - lt.tv_usec) / 1000000.0;
	fprintf(stderr, "Time : %.6lf\n", time);
	/* write output file */
	output = fopen("output_user.life", "w");
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
	printf("%d %d", cube[0][0][2].num_of_neighbours, cube[0][0][2]);
	fclose(output);
	//print_matricies();
	return 0;
}
