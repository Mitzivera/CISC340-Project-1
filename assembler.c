#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>

void print_usage(){
	printf("Usage: ./assembler -i program.as > program.mc | ./assembler -i program.ac -o program.mc | ./assembler -i program.ac\n");
	exit(2);
}

int main(int argc, char **argv){
	char *inputFilename;
	char *outputFilename;

	int IFLAG=0;
	int opt;

	char line[100];
	int address;
	char labels[50][6];
	int labelAddr[50];
	int tempIndex;

	while((opt = getopt(argc, argv, "i:o:")) != -1){
		switch(opt){
			case 'i':
				inputFilename = optarg;
				
				if(inputFilename != NULL){
					IFLAG = 1;
				}
				break;				
			
			case 'o':
				outputFilename = optarg;
				break;

			default:
				print_usage();
		}
	}

	if(!IFLAG) {print_usage();}

	FILE *rf = fopen(inputFilename, "r");
	FILE *wf = fopen(outputFilename, "w");	

	address = 0;
	
	while((fgets(line, 100, rf)) != NULL){
		//check if it is a label
		if(!isspace((int)line[0])){
			tempIndex = 1;
			while(!isspace((int)line[tempIndex])){
				++tempIndex;
			}
				
			printf("Line %d :: %s", address, line);
		}

		++address;
	}

	fclose(wf);
	fclose(rf);
}
