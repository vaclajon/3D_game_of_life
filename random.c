#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>



void print_help(){
    printf("some help\n");
		exit(0);
}

void save_to_file(int *array[], char *filename, int N){
  FILE *output;
  int i;
  file=fopen(filename, "w");
  if(file == NULL){
    printf("Error opening file [%s]", filename);
    exit(1);
  }
  for ( i = 0; i<N;i++){
    fprintf(output,"%d\n",array[i]);
  }
}

int main(int argc, char *argv[]){
	srand(time(NULL));
	int N;
	int list[N];
	int i;
  char *filename = "random.txt";	
  
  //-h as argument
	if(strncmp(argv[1], "-h",6) == 0){
    print_help();
	}
  
  //lenght of array
	N = atoi(argv[1]);
  
	//checks wheather there is more than one argument
	if(argc>2){
    //filename as argument		
		if(strncmp(argv[2], "-f",2) == 0){
			filename = argv[3];
		}
		else{    
			printf("Unsupported parameter [%s]\n",argv[2]);
			printf("Use random -h for help\n");
			exit(1);
		}
	}	
  //make random number array
	for(i = 0; i < N ;i++){
		list[i] = rand();		
	}
  save_to_file(*list, filename, N);
	exit(0);
}
