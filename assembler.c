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
long int* translateLine(char *line, struct Labels *labelArray, int length);
long int* rType(char *line, struct Labels *labelArray, int length);
long int* iType(char *line, struct Labels *labelArray, int length);
long int* jType(char *line, struct Labels *labelArray, int length);
long int* oType(char *line, struct Labels *labelArray, int length);
long int* fillType(char *line, struct Labels *labelArray, int length);

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
		long int *outputNumber = (long int*)malloc(sizeof(long int));	
		
		++address;	
		
		outputNumber = translateLine(line, labelArray, labelArrayIndex);
	}



	fclose(rf);
	

///////////////////////////////////////////////////////////////
}

void print_usage(){
    printf("Usage: ./assembler -i program.as > program.mc | ./assembler -i program.as -o program.mc | ./assembler -i program.as\n");
    exit(2);
}

long int* translateLine(char *line, struct Labels *labelArray, int length){
	char opcodes[9][6] = {"add", "nand", "lw", "sw", "beq", "jalr", "halt", "noop", ".fill"};		
	char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
	strcpy(strCopy, line);
	long int *outputNumber = (long int*)malloc(sizeof(long int));
	//this FLAG indicates the program found the opcode
	int opcodeFLAG = 0;
	//this index indicate which opcode is using
	int opcodeIndex;

	char *firstWord = (char*)malloc(7*sizeof(char));
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
		char *secondWord = (char*)malloc(7*sizeof(char));
		strcpy(secondWord, strtok(NULL, " \t\n\v\f\r"));

			
	
		for(i=0; i<9; i++){
			if(strcmp(secondWord, opcodes[i]) == 0){
				opcodeFLAG = 1;
				opcodeIndex = i;
				break;
			}
		}
	}

	if(opcodeIndex == 8){
		//.fill condition
		printf(".fill value :: %li\n",*(fillType(line, labelArray, length)));	
	}else if (opcodeIndex >= 0 && opcodeIndex <= 1){
		//r-type condition
		printf("r type value :: %li\n", *(rType(line, labelArray, length)));
	}else if (opcodeIndex >= 2 && opcodeIndex <= 4){
		//i-type condition
	}else if (opcodeIndex == 5){
		//j-type condition
	}else if (opcodeIndex >= 6 && opcodeIndex <= 7){
		//o-type condition
	}else{
		//someting is wrong
		printf("Line :: %s, exiting, something went wrong\n", line);
		exit(1);
	}
	
	*outputNumber = 1;

	return outputNumber;
}

long int* fillType(char *line, struct Labels *labelArray, int length){
	long int *outputNumber = (long int*)malloc(sizeof(long int));

	char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
    strcpy(strCopy, line);		
	
	char *firstWord = (char*)malloc(7*sizeof(char));
    strcpy(firstWord,strtok(strCopy, " \t\n\v\f\r"));

	//.fill must use a label, pull out value from the label array
	int i;
	for(i=0; i<length; i++){
		if(strcmp(firstWord, labelArray[i].labelName) == 0){
			//matched label
			*outputNumber = labelArray[i].value;
		}
	}

	return outputNumber;
}

long int* rType(char *line, struct Labels *labelArray, int length){
	long int *outputNumber = (long int*)malloc(sizeof(long int));

    char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
    strcpy(strCopy, line);

    char *firstWord = (char*)malloc(7*sizeof(char));
    strcpy(firstWord,strtok(strCopy, " \t\n\v\f\r"));

	long int opcodeDigit;

	if(strcmp(firstWord, "add") == 0 || strcmp(firstWord, "nand") == 0){
		//the first word is a opcode
		
		if(strcmp(firstWord, "add") == 0){
            opcodeDigit = 0;
        }else if(strcmp(firstWord, "nand") == 0){
            opcodeDigit = 1;
        }	

		long int regDest = atoi(strtok(NULL, " \t\n\v\f\r"));
        long int regA = atoi(strtok(NULL," \t\n\v\f\r"));
        long int regB = atoi(strtok(NULL," \t\n\v\f\r"));
	
		opcodeDigit = opcodeDigit << 22;
        regA = regA << 19;
        regB = regB << 16;

		*outputNumber = *outputNumber | opcodeDigit | regDest | regA | regB;
	}else{
		//the first word is a label
		//the second word is a opcode

		char *secondWord = (char*)malloc(7*sizeof(char));
		strcpy(secondWord, strtok(NULL, " \t\n\v\f\r"));

		if(strcmp(secondWord, "add") == 0){
			opcodeDigit = 0;
		}else if(strcmp(secondWord, "nand") == 0){
			opcodeDigit = 1;
		}

		long int regDest = atoi(strtok(NULL, " \t\n\v\f\r"));
		long int regA = atoi(strtok(NULL," \t\n\v\f\r"));
		long int regB = atoi(strtok(NULL," \t\n\v\f\r"));	

		opcodeDigit = opcodeDigit << 22;
		regA = regA << 19;
		regB = regB << 16;

		*outputNumber = *outputNumber | opcodeDigit | regDest | regA | regB;

	}


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


