#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define FILENAME "random.txt"

void print_help(){
    printf("some help\n");
		exit(0);
}


void open_file(char *filename, FILE *file){
      file=fopen(filename, "w");
      if(file == NULL){
        printf("Error opening file [%s]", filename);
        exit(1);
      }
      
}
int main(int argc, char *argv[]){
	srand(time(NULL));
	int N;
	int list[N];
	int i;
	FILE *output;
  
  //-h as argument
	if(strncmp(argv[1], "-h",6) == 0){
    print_help();
	}
  
  //lenght of array
	N = atoi(argv[1]);
  
	//checks wheather there is more than one argument
	if(argc>2){
    //filename as argument		
		if(strncmp(argv[2], "-f",6) == 0){
			open_file(argv[3], output);
		}
		else{
    
			printf("Unsupported parameter [%s]\n",argv[2]);
			printf("Use random -h for help\n");
			exit(1);
		}
	}
	else{   
		printf("Using default file name [%s]\n", FILENAME);
		open_file(FILENAME, output);
	}
  //make random and write to file	
	for(i = 0; i < N ;i++){
    list[i] = rand();
		fprintf(output,"%d\n",list[i]);
	}
	exit(0);
}
