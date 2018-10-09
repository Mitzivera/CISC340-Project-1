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
unsigned long long int* translateLine(char *line);


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
//purpose: to get the length of the filename by counting the number of lines	
	FILE *rf = fopen(inputFilename, "r");
	numberOfLines = 0;
	
	while((fgets(line, 100, rf)) != NULL){
		++numberOfLines;
	}
////////////////////////////////////////////////////////////////


	//use the number of lines in the file to create the array of labels
	struct Labels labelArray[numberOfLines];
	int labelArrayIndex = 0;

/////////////// 2nd time  read and write to files //////////////////////
//purpose: to fill the labelArray with the label in assembly language	

	rewind(rf);
	address = 0;

	while((fgets(line, 100, rf)) != NULL){

		if(containLabel(line)){
			createLabels(address, line, labelArray, labelArrayIndex);
			++labelArrayIndex;
		}

		++address;
	}


///////////////////////////////////////////////////////////////

	//copy label array to a new array to remove the empty label, then free the pervious array.
	struct Labels finalLabels[labelArrayIndex];
	memcpy(finalLabels, labelArray, labelArrayIndex*sizeof(struct Labels));

///////////// 3rd time read and write to files////////////////////////
	
	rewind(rf);
	address = 0;
	
	while((fgets(line, 100, rf)) != NULL){
		unsigned long long int *outputNumber = (unsigned long long int*)malloc(sizeof(unsigned long long int));	
		
		++address;	
		
		outputNumber = translateLine(line);
	}



	fclose(rf);
	

///////////////////////////////////////////////////////////////
}

void print_usage(){
    printf("Usage: ./assembler -i program.as > program.mc | ./assembler -i program.as -o program.mc | ./assembler -i program.as\n");
    exit(2);
}

unsigned long long int* translateLine(char *line){
	char opcodes[9][5] = {"add", "nand", "lw", "sw", "beq", "jalr", "halt", "noop", ".fill"};		
	char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
	strcpy(strCopy, line);
	unsigned long long int *outputNumber = (unsigned long long int*)malloc(sizeof(unsigned long long int));
	//this FLAG indicates the program found the opcode
	int opcodeFLAG = 0;
	//this index indicate which opcode is using
	int opcodeIndex;

	char *firstWord = (char*)malloc(6*sizeof(char));
	strcpy(firstWord,strtok(strCopy, " \t\n\v\f\r"));

	//This for loop tries to matched first word with one of the opcode
	int i;
	for(i=0; i<9; i++){
		if(strcmp(firstWord, opcodes[i]) == 0){
			//opcode matched
			opcodeFLAG = 1;
			opcodeIndex = i;
			break;
		}
	}	


	if(opcodeFLAG == 0){
		char *secondWord = (char*)malloc(6*sizeof(char));
		strcpy(secondWord, strtok(NULL, " \t\n\v\f\r"));
		
		for(i=0; i<9; i++){
			if(strcmp(secondWord, opcodes[i]) == 0){
				opcodeFLAG = 1;
				opcodeIndex = i;
				break;
			}
		}

		printf("opcode :: %s, Line :: %s\n", opcodes[opcodeIndex], line);
	}else{
		printf("opcode :: %s, Line :: %s\n", opcodes[opcodeIndex], line);
	}
	*outputNumber = 1;

	return outputNumber;
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

	label = strtok(line, " \t\n\v\f\r");
	strcpy(newLabel.labelName, label);

	//check if this is a .fill line
	secWord = strtok(NULL, " \t\n\v\f\r");	
	//strcmp(char *a, char*b) returns 0 if these two string are equal, otherwise non-zero.
	if(!strcmp(secWord, ".fill")){
		newLabel.isDotFill = 1;
		
		thirdWord = strtok(NULL, " \t\n\v\f\r");
	
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
					if(strcmp(strtok(thirdWord, " \t\n\v\f\r"), labelArray[i].labelName) == 0){
						//the label matched
						newLabel.value = labelArray[i].address;
						break;
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


