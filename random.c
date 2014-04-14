#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef struct node{
	int value;
	struct node *llink;
	struct node *rlink;	
}node;

typedef struct double_queue{
	struct node *head;
}double_queue;

void double_queue_create(double_queue *dq){
	dq->head = malloc(sizeof(struct node));	
	dq->head->value = 0;
	dq->head->llink = dq->head->rlink = dq->head;
}

void double_queue_insert(double_queue *dq, node *node)
{
	
	node->llink = dq->head;
	node->rlink = dq->head->rlink;
	dq->head->rlink->llink = node;
	dq->head->rlink = node;
}

void double_queue_delete(double_queue *dq, node *node)
{
	if (dq->head == node)
		printf("Deletion of head not alloweded\n");
	else {
		node->llink->rlink = node->rlink;
		node->rlink->llink = node->llink;
		free(node);
	}
}

void double_queue_add(double_queue *dq, int value)
{
	node *next_num;		
	next_num = malloc(sizeof(struct node));
	next_num->value = value;	
	double_queue_insert(dq, next_num);
	
}

void print_help(){
	printf(" [random] program is made for generating file with random numbers.\n");
	printf("Its output is two files. Second one has reverse list of number from fisrt one\n");
	printf("1 -h  Prints help\n\n");
	printf("2 number [-f filename]  Makes filename and filename_r with [number] random integers.\n");
	printf("Default names are random.txt and random_r.txt\n");
		exit(0);
}

void save_to_file(int *array, char *filename, int N){
	FILE *output;
	int i;
	output = fopen(filename, "w");
	if (output == NULL){
		printf("Error opening file [%s]", filename);
		exit(1);
	}
	printf("%d", array[3]);
	for (i = 0; i<N; i++){
		fprintf(output, "%d\n", array[i]);
	}
	fclose(output);
}

int main(int argc, char *argv[]){
	srand(time(NULL));
	int N, i;
	double_queue list;
	char *filename = "random.txt";

	//-h as argument
	if (strncmp(argv[1], "-h", 6) == 0){
		print_help();
	}

	//checks wheather there is more than one argument
	if (argc>2){
		//filename as argument		
		if (strncmp(argv[2], "-f", 2) == 0){
			filename = argv[3];
		}
		else{
			printf("Unsupported parameter [%s]\n", argv[2]);
			printf("Use random -h for help\n");
			exit(1);
		}
	}
	N = atoi(argv[1]);
	double_queue_create(&list);

	//make random number array	
	for (i = 0; i < N; i++){
		double_queue_add(&list, rand());
	}
	//save_to_file(list.values, filename, N);
	//free(list.values);
	exit(0);
}
