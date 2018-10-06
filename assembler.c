#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <ctype.h>
#include <string.h>

//a structure that contains name and address for a label
struct Labels{
	char labelName[6];
	int address;
};

void print_usage();
int containLabel(char* line);
struct Labels createLabels(int addr, char* line);
void print_label(struct Labels label);


int main(int argc, char **argv){
	char *inputFilename;
	char *outputFilename;

	//IFLAG, as boolean to ensure user enter a input file name
	int IFLAG=0;
	int opt;

	char line[100];
	int address;

	//Collection for the label in assembly language
	//no sure the size at this point
	int collectionSize = 30;
	struct Labels collection[collectionSize];
	int labelIterator = 0;

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
	
	while((fgets(line, 100, rf)) != NULL) {
		//check if it is a label
		if (containLabel(line)) {
			//create a label using "Labels" structure
			collection[labelIterator] = createLabels(address, line);
			++labelIterator;
		}

		++address;
	}

	fclose(wf);
	fclose(rf);

	labelIterator = 0;
	while(labelIterator < collectionSize){

		//if the labelName is empty, stop printing.
		if(strlen(collection[collectionSize].labelName) == 0){ break;}

		printf("Index: %d, Label: %s, Address: %d\n", labelIterator, collection[labelIterator].labelName,
			   collection[labelIterator].address);
		++labelIterator;
	}
}


void print_usage(){
    printf("Usage: ./assembler -i program.as > program.mc | ./assembler -i program.ac -o program.mc | ./assembler -i program.ac\n");
    exit(2);
}

int containLabel(char* line){
	if (!isspace((int)line[0])){
		return 1;
	}else{
		return 0;
	}
}

struct Labels createLabels(int addr, char* line){
	char* label;
	struct Labels newLabel;

	label = strtok(line, " ");

	strcpy(newLabel.labelName, label);
	newLabel.address = addr;
	print_label(newLabel);
	return newLabel;
}

void print_label(struct Labels label){
	printf("Label: %s, Address: %d\n", label.labelName, label.address);

}
