#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FILENAME "random.txt"
int main(int argc, char *argv[]){
	srand(time(NULL));
	int N;
	int list[N];
	int i;
	FILE *output;
	if(!strncmp(argv[1], "-h",6)){
		printf("some help\n");
		exit(0);
	}
	N = atoi(argv[1]);
	for(i = 0;i<N;i++){
		list[i] = rand();
	}

	if(argc>2){
		printf("%s\n", argv[2]);
		if(!strncmp(argv[2], "-f",6)){
			output=fopen(argv[3], "w");
		}
		else{
			printf("Unsupported parameter [%s]\n",argv[2]);
			printf("Use random -h for help\n");
			exit(1);
		}
	}
	else{
		printf("Using default file name [%s]\n", FILENAME);
		output = fopen(FILENAME, "w");
	}
	if(output == NULL){
		printf("Error opening file");
		exit(1);
	}
	for(i = 0; i < N ;i++){
		fprintf(output,"%d ",list[i]);
	}
	exit(0);
}
