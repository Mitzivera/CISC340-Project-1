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
	int value;
};

void print_usage();
int containLabel(char* line);
void createLabels(int addr, char* line, struct Labels *labelArray, int index);
void print_labelArray(struct Labels *labelArray, int length);

int main (int argc, char **argv){
	char *inputFilename = (char*)malloc(sizeof(char)*256);
	char *outputFilename = (char*)malloc(sizeof(char)*256);
	int numberOfLines;

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

/////////////// 1st time  read and write to files //////////////////////
	FILE *rf = fopen(inputFilename, "r");
	numberOfLines = 0;
	
	while((fgets(line, 100, rf)) != NULL){
		++numberOfLines;
	}
////////////////////////////////////////////////////////////////

	struct Labels labelArray[numberOfLines];
	int labelArrayIndex = 0;

/////////////// 2nd time  read and write to files //////////////////////
	rewind(rf);
	//FILE *wf = fopen(outputFilename, "w");

	address = 0;

	while((fgets(line, 100, rf)) != NULL){

		if(containLabel(line)){
			createLabels(address, line, labelArray, labelArrayIndex);
			++labelArrayIndex;
		}

		++address;
	}

	print_labelArray(labelArray, labelArrayIndex);

	//fclose(wf);
	fclose(rf);

///////////////////////////////////////////////////////////////

}

void print_usage(){
    printf("Usage: ./assembler -i program.as > program.mc | ./assembler -i program.as -o program.mc | ./assembler -i program.as\n");
    exit(2);
}

int containLabel(char* line){
	if(!isspace((int)line[0])){
		return 1;
	}else{
		return 0;
	}
}

void createLabels(int addr, char* line, struct Labels *labelArray, int index){
	int labelsize = 6;
	char* label = (char*)malloc(sizeof(char)*(labelsize+1));	
	char* secWord = (char*)malloc(sizeof(char)*11);	
	char* thirdWord = (char*)malloc(sizeof(char)*11);
	int immediateValue = 0;	
	
	//create and initialize a new label
	struct Labels newLabel;
	newLabel.labelName = (char*)malloc(sizeof(char)*(labelsize+1));
	newLabel.isDotFill = 0;
	newLabel.address = addr;
	newLabel.value = 0;

	label = strtok(line, " ");
	strcpy(newLabel.labelName, label);

	//check if this is a .fill line
	secWord = strtok(NULL, " ");	
	//strcmp(char *a, char*b) returns 0 if these two string are equal, otherwise non-zero.
	if(!strcmp(secWord, ".fill")){
		newLabel.isDotFill = 1;
		
		thirdWord = strtok(NULL, " ");
	
		immediateValue = atoi(thirdWord);

		//if immediateValue != 0, it means that this is a number not a label
		if(immediateValue != 0){
			newLabel.value = immediateValue;
		}else{
			if(strcmp(thirdWord, "0")==0){
				//this means the thirdWord is actual 0
				newLabel.value = immediateValue;
			}else{
				//this means the thirdWord is a label
				//then we find the label in the labelArray, and take its address
				int i;
				for(i=0; i<index; i++){
					printf("str1 : %s , str2 :%s\n", strtok(thirdWord, "\n"), labelArray[i].labelName);
					if(strcmp(strtok(thirdWord, "\n"), labelArray[i].labelName) == 0){
						//the label matched
						newLabel.value = labelArray[i].address;
					}
				}
			}
		}
	}

	labelArray[index] = newLabel;	
}

void print_labelArray(struct Labels *labelArray, int length){
	int i;
	for(i=0; i<length; i++){
		printf("Label :: %s, Address :: %d, isDotFill? :: %d, Immediate Value :: %d \n", labelArray[i].labelName, labelArray[i].address, labelArray[i].isDotFill, labelArray[i].value);
	}
}


