#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>

//a structure that contains name and address for a label
//value here store addresses for label; store actual value for any .fill variable
//isDotFill contains boolean value to indicate that this is a .fill label or not
struct Labels{
	char* labelName;
	int address;
	int isDotFill;
};

void print_usage();
int containLabel(char* line);
struct Labels createLabels(int addr, char* line);
void print_label(struct Labels label);

int main (int argc, char **argv){
	char *inputFilename = (char*)malloc(sizeof(char)*256);
	char *outputFilename = (char*)malloc(sizeof(char)*256);

	int IFLAG = 0;
	int opt;

	//IFLAG, as boolean to ensure user enter a input file name
	char *line = (char*)malloc(sizeof(char)*100);
	int address;
	
/////////////////     taking input argument ///////////////////////

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

/////////////////////////////////////////////////////////////////	

///////////////  read and write to files //////////////////////
	FILE *rf = fopen(inputFilename, "r");
	//FILE *wf = fopen(outputFilename, "w");

	address = 0;

	while((fgets(line, 100, rf)) != NULL){

		if(containLabel(line)){
			print_label(createLabels(address, line));
		}

		++address;
	}

	//fclose(wf);
	fclose(rf);

///////////////////////////////////////////////////////////////

}

void print_usage(){
    printf("Usage: ./assembler -i program.as > program.mc | ./assembler -i program.ac -o program.mc | ./assembler -i program.ac\n");
    exit(2);
}

int containLabel(char* line){
	if(!isspace((int)line[0])){
		return 1;
	}else{
		return 0;
	}
}

struct Labels createLabels(int addr, char* line){
	int labelsize = 6;
	char* label = (char*)malloc(sizeof(char)*(labelsize+1));	
	struct Labels newLabel;
	newLabel.labelName = (char*)malloc(sizeof(char)*(labelsize+1));

	label = strtok(line, " ");
	strcpy(newLabel.labelName, label);
	newLabel.address = addr;	

	return newLabel;
}

void print_label(struct Labels label){
	printf("Label: %s, address: %d\n", label.labelName, label.address);

}
