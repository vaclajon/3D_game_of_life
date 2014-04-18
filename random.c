#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/*
Random integer node
*/
typedef struct node{
	int value;
	struct node *llink;
	struct node *rlink;
}node;


/*
Doubly linked list
*/
typedef struct double_queue{
	struct node *head;
}double_queue;

void double_queue_create(double_queue *dq){
	dq->head = (node *) malloc(sizeof(struct node));
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
		printf("Deletion of head is not alloweded\n");
	else {
		node->llink->rlink = node->rlink;
		node->rlink->llink = node->llink;
		free(node);
	}
}

void double_queue_add(double_queue *dq, int value)
{
	node *next_num;
	next_num = (node *) malloc(sizeof(struct node));
	next_num->value = value;
	double_queue_insert(dq, next_num);

}

void print_help(){
	printf(" [random] program is made for generating file with random numbers.\n");
	printf("Its output is two files. Second one has reverse list of number from fisrt one\n\n");
	printf("1 -h  Prints help\n\n");
	printf("2 number [-f filename]  Makes filename and filename_r with [number] random integers.\n\n");
	printf("Default names are random.txt and random_r.txt\n");
	exit(0);
}
/*
Finds last "." in filename and insert "_r" before.
If there is no "." in filename, "_r" is added at the end
*/
char * add_text(char *filename){	
	int i,j;
	char *ch;
	char *new_filename = (char *)malloc(strlen(filename) + 3);
	//Find first "." from the end
	for (i = strlen(filename); i > 0; i--){	

		if (filename[i] == '.'){
			break;
		}
	}
	//no "." in filename
	if (i == 0){
		i = strlen(filename);
	}
	//copy till "."
	for (j = 0; j < i; j++)
	{
		new_filename[j] = filename[j];
	}

	new_filename[j] = '_';
	new_filename[j+1] = 'r';
	//copy the rest
	for (i; i < strlen(new_filename); i++)
	{
		new_filename[i + 2] = filename[i];
	}	
	return new_filename;
	
}

/*
Saves queue into filename and queue in reversed order in filename_r
*/
void save_to_file(double_queue *dq, char *filename){
	FILE *output,*rev_output;
	int i;
	node *node, *rev_node;
	char * rev_filename = add_text(filename);
	
	output = fopen(filename, "w");
	rev_output = fopen(rev_filename, "w");

	if (output == NULL || rev_output == NULL){
		printf("Error opening file [%s] or [%s]\n", filename, rev_filename);
		exit(1);
	}	
	
	node = dq->head->rlink;
	rev_node = dq->head->llink;
	//Queue is symetric - we dont need to check both sides
	while (node != dq->head) {
		fprintf(output, "%d\n",node->value);
		fprintf(rev_output, "%d\n", rev_node->value);
		node = node->rlink;
		rev_node = rev_node->llink;
	}	

	fclose(output);
	fclose(rev_output);
}

int main(int argc, char *argv[]){	
	int N, i;
	double_queue list;					//stores random numbers
	char *filename = "random.txt";		//default filename init

	if (argc == 1){
		printf("No parameter provided.\n");
		exit(1);
	}

	
	if (strncmp(argv[1], "-h", 2) == 0){
		print_help();
	}

	//checks wheather there is more than one argument
	if (argc>2){
		//filename as argument		
		if (strncmp(argv[2], "-f", 2) == 0){
			//new filename
			filename = argv[3];
			printf("Using filename [%s]\n", filename);
		}
		else{
			printf("Unsupported parameter [%s]\nUse random -h for help\n", argv[2]);
			exit(1);
		}
	}
	else{
		printf("Using default file name [%s]\n", filename);
	}

	N = atoi(argv[1]);			//lenght of list of random integers
	double_queue_create(&list);
	srand(time(NULL));

	//make random number array	
	for (i = 0; i < N; i++){
		double_queue_add(&list, rand());
	}

	save_to_file(&list, filename);	

	exit(0);
}
