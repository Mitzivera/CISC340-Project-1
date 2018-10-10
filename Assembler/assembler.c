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

void test_label(char* line);
void print_usage();
int containLabel(char* line);
void createLabels(int addr, char* line, struct Labels *labelArray, int index);
void print_labelArray(struct Labels *labelArray, int length);
long int* translateLine(char *line, struct Labels *labelArray, int length, int currAddr);
long int* rType(char *line);
long int* iType(char *line, struct Labels *labelArray, int length, int currAddr);
long int* jType(char *line);
long int* oType(char *line);
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
	FILE *wf;
	if(strlen(outputFilename) != 0){
		wf = fopen(outputFilename, "w");
	}
	address = 0;
	
	while((fgets(line, 100, rf)) != NULL){
		long int *outputNumber = (long int*)malloc(sizeof(long int));	
		
		outputNumber = translateLine(line, labelArray, labelArrayIndex, address);
		
	
		if(strlen(outputFilename) == 0){
			if(address != numberOfLines-1){
				printf("%li\n", *(outputNumber));
			}else{
				printf("%li", *(outputNumber));
			}
		}else{
			if(address != numberOfLines-1){
				fprintf(wf, "%li\n", *outputNumber);
			}else{
				fprintf(wf, "%li", *outputNumber);
				fclose(wf);
			}
		}
		
		++address;	
	}

	fclose(rf);
	

///////////////////////////////////////////////////////////////
}

void print_usage(){
    printf("Usage: ./assembler -i program.as > program.mc | ./assembler -i program.as -o program.mc | ./assembler -i program.as\n");
    exit(2);
}

long int* translateLine(char *line, struct Labels *labelArray, int length, int currAddr){
	char opcodes[9][6] = {"add", "nand", "lw", "sw", "beq", "jalr", "halt", "noop", ".fill"};		
	char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
	strcpy(strCopy, line);
	long int *outputNumber = (long int*)malloc(sizeof(long int));
	//this FLAG indicates the program found the opcode
	int opcodeFLAG = 0;
	//this index indicate which opcode is using
	int opcodeIndex;
	int labelFLAG = 0;

	labelFLAG = containLabel(line);

	char *firstWord = (char*)malloc(7*sizeof(char));
	strcpy(firstWord,strtok(strCopy, " \t\n\v\f\r"));

	if(labelFLAG == 0){
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
	}else{
		char *secondWord = (char*)malloc(7*sizeof(char));
		strcpy(secondWord, strtok(NULL, " \t\n\v\f\r"));

		int i;
		for(i=0; i<9; i++){
			if(strcmp(secondWord, opcodes[i]) == 0){
				opcodeFLAG = 1;
				opcodeIndex = i;
				break;
			}
		}
	}

	if(opcodeFLAG == 0){
		fprintf(stderr, "%s : %s\n", "Error: opcode is not recognized", line);
		exit(0); 
	}

	if(opcodeIndex == 8){
		//.fill condition
		outputNumber = fillType(line, labelArray, length); 
	}else if (opcodeIndex >= 0 && opcodeIndex <= 1){
		//r-type condition
		outputNumber = rType(line);	
	}else if (opcodeIndex >= 2 && opcodeIndex <= 4){
		//i-type condition
		outputNumber = iType(line, labelArray, length, currAddr);		
	}else if (opcodeIndex == 5){
		//j-type condition
		outputNumber = jType(line);	
	}else if (opcodeIndex >= 6 && opcodeIndex <= 7){
		//o-type condition
		outputNumber = oType(line);	
	}else{
		//someting is wrong
		printf("Line :: %s, exiting, something went wrong\n", line);
		exit(1);
	}
	
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

long int* rType(char *line){
	long int *outputNumber = (long int*)malloc(sizeof(long int));

    char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
    strcpy(strCopy, line);

    char *firstWord = (char*)malloc(7*sizeof(char));
    strcpy(firstWord,strtok(strCopy, " \t\n\v\f\r"));

	char *opcodeChar = (char*)malloc(7*sizeof(char));
	long int opcodeDigit;

	if(strcmp(firstWord, "add") == 0 || strcmp(firstWord, "nand") == 0){
		//the first word is a opcode
		
		strcpy(opcodeChar, firstWord);		
	}else{
		//the first word is a label
		//the second word is a opcode
		char *secondWord = (char*)malloc(7*sizeof(char));
		strcpy(secondWord, strtok(NULL, " \t\n\v\f\r"));
		
		strcpy(opcodeChar, secondWord);
	}
 
	if(strcmp(opcodeChar, "add") == 0){
    	opcodeDigit = 0;
    }else if(strcmp(opcodeChar, "nand") == 0){
        opcodeDigit = 1;
    }

    long int regDest = atoi(strtok(NULL, " \t\n\v\f\r"));
    long int regA = atoi(strtok(NULL," \t\n\v\f\r"));
   	long int regB = atoi(strtok(NULL," \t\n\v\f\r"));

    opcodeDigit = opcodeDigit << 22;
    regA = regA << 19;
    regB = regB << 16;

    *outputNumber = *outputNumber | opcodeDigit | regDest | regA | regB;

	return outputNumber;	
}

long int* jType(char *line){
    long int *outputNumber = (long int*)malloc(sizeof(long int));

    char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
    strcpy(strCopy, line);

    char *firstWord = (char*)malloc(7*sizeof(char));
    strcpy(firstWord,strtok(strCopy, " \t\n\v\f\r"));
	
	char *opcodeChar = (char*)malloc(7*sizeof(char));
    long int opcodeDigit;

	if(strcmp(firstWord, "jalr") == 0){
		//the first word is an opcode
		strcpy(opcodeChar, firstWord);
	}else{
		//the first word is a label
		//the second word is an opcode
		char *secondWord = (char*)malloc(7*sizeof(char));
		strcpy(secondWord, strtok(NULL," \t\n\v\f\r")); 
		strcpy(opcodeChar, secondWord);
	}
	
	if(strcmp(opcodeChar, "jalr") == 0){
		opcodeDigit = 5;
	}else{
		printf("something went wrong :: J-type translation\n");
		exit(1);
	}

	long int regA = atoi(strtok(NULL, " \t\n\v\f\r"));
	long int regB = atoi(strtok(NULL, " \t\n\v\f\r"));

	opcodeDigit = opcodeDigit << 22;
	regA = regA << 19;
	regB = regB << 16;

	*outputNumber = *outputNumber | opcodeDigit | regA | regB ;

	return outputNumber;

}

long int* oType(char *line){
    long int *outputNumber = (long int*)malloc(sizeof(long int));

    char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
    strcpy(strCopy, line);

    char *firstWord = (char*)malloc(7*sizeof(char));
    strcpy(firstWord,strtok(strCopy, " \t\n\v\f\r"));

    char *opcodeChar = (char*)malloc(7*sizeof(char));
    long int opcodeDigit;
	
	if(strcmp(firstWord, "halt") == 0 || strcmp(firstWord, "noop") == 0){
		//the first word is an opcode
		strcpy(opcodeChar, firstWord);
	}else{
		//the first word is a label
		//the second word is an opcode
		char *secondWord = (char*)malloc(7*sizeof(char));
        strcpy(secondWord, strtok(NULL," \t\n\v\f\r"));
        strcpy(opcodeChar, secondWord);	
	}

	if(strcmp(opcodeChar, "halt") == 0){
		opcodeDigit = 6;
	}else if(strcmp(opcodeChar, "noop") == 0){
		opcodeDigit = 7;
	}else{
		printf("something went wrong :: o-type translation");
		exit(1);
	}

	opcodeDigit = opcodeDigit << 22;

	*outputNumber = *outputNumber | opcodeDigit;

	return outputNumber;
}

long int* iType(char *line, struct Labels *labelArray, int length, int currAddr){
	
	long int *outputNumber = (long int*)malloc(sizeof(long int));

    char *strCopy = (char*)malloc(sizeof(char)*strlen(line));
    strcpy(strCopy, line);

    char *firstWord = (char*)malloc(7*sizeof(char));
    strcpy(firstWord,strtok(strCopy, " \t\n\v\f\r"));

    char *opcodeChar = (char*)malloc(7*sizeof(char));
    long int opcodeDigit;	

	long int offsetDigit;

	if(strcmp(firstWord, "lw") == 0 || strcmp(firstWord, "sw") == 0 || strcmp(firstWord, "beq") == 0){
		//first word is an opcode
		strcpy(opcodeChar, firstWord);
	}else{
		//first word is a label
		//second word is an opcode
		char *secondWord = (char*)malloc(7*sizeof(char));
        strcpy(secondWord, strtok(NULL," \t\n\v\f\r"));
        strcpy(opcodeChar, secondWord);	
	}
	
	if(strcmp(opcodeChar, "lw") == 0){
		opcodeDigit = 2;
	}else if(strcmp(opcodeChar, "sw") == 0){
		opcodeDigit = 3;
	}else if(strcmp(opcodeChar, "beq") == 0){
		opcodeDigit = 4;
	}else{
		printf("something went wrong :: i-type translation");
        exit(1);	
	}

	long int regA = atoi(strtok(NULL," \t\n\v\f\r"));
    long int regB = atoi(strtok(NULL," \t\n\v\f\r"));
	
	char *offsetChar = (char*)malloc(7*sizeof(char));
	strcpy(offsetChar, strtok(NULL, " \t\n\v\f\r"));

	offsetDigit = atoi(offsetChar);
		 
	if(offsetDigit != 0){
    	//the offset field contains non-zero nunmber    
	}else{
    	if(strcmp(offsetChar, "0")==0){
        	//the offset field contains a zero number	 
		}else{
         	//this means the offset field contains a label
            //then we find the label in the labelArray
			//if opcode is beq, we need to calculate the number of lines we need to go
			//offsetDigit will be the number of lines
			//if opcode is lw or sw, we need to take the value out from the .fill type
			//offsetDigit will be the value

            int i;
			int matchedFLAG = 0;
            for(i=0; i<length; i++){
            	if(strcmp(strtok(offsetChar, " \t\n\v\f\r"), labelArray[i].labelName) == 0){
    	        	//the label matched
                	matchedFLAG = 1;
					if(opcodeDigit == 2 || opcodeDigit == 3){
						//opcode is either lw or sw
						if(labelArray[i].isDotFill == 1){
							offsetDigit = labelArray[i].address;
						}else{
							printf("Line %d :: should not put a .fill label for opcode lw or sw", currAddr);
							exit(1);
						}
					}else if(opcodeDigit == 4){
						//opcode is beq
						if(labelArray[i].isDotFill == 0){
							//destination address = PC + 1 + offsetField
							//offsetField = destination address - (PC + 1)
							offsetDigit = (long int)(labelArray[i].address - (currAddr + 1));
						}else{
							printf("Line %d :: should not put a address label for opcode beq", currAddr);
							exit(1);
						}
					}else {
						printf("opcode not identify, translating in i type");
					}	

					break;
  	         	}
            }
			if(matchedFLAG == 0){
				fprintf(stderr,"%s : %s\n", "Error: label in the offset field does not exists", offsetChar);
				exit(0);
			}
			
       	}
    }

	opcodeDigit = opcodeDigit << 22;
	regA = regA << 19;
    regB = regB << 16;

	printf("%s : %li\n", "offset number is :: ", offsetDigit);
	if(offsetDigit > 32767 || offsetDigit < -32768){
		fprintf(stderr, "%s: %li\n", "Error: offset does not fit in 16-bit", offsetDigit);
	}

	//0000 FFFF
	long int mask = 65535;
	offsetDigit = offsetDigit & mask;

	*outputNumber = *outputNumber | opcodeDigit | regA | regB | offsetDigit;

	return outputNumber;
}

void test_label(char* line){
	//check that label doesn't start with number
	if(isalpha(line[0])==0){
		fprintf(stderr,"%s : %s\n","Error: a label cannot start with a number", line);
		exit(0);
	}

	//check that label is alphanumeric
	for(int i=0; i < strlen(line); i++){
		if(isalnum(line[i])== 0){
			fprintf(stderr, "%s : %s\n", "Error: a label has to be alphanumeric", line);
			exit(0);
		}
	}

	//check length of label
	if(strlen(line) > 6){
		fprintf(stderr, "%s : %s\n", "Error: length of label cannot exceed 6 characters", line);
		exit(0);
	}

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
	test_label(label);
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

	int i;
	for(i=0; i<index; i++){
		if(strcmp(newLabel.labelName, labelArray[i].labelName) == 0){
			fprintf(stderr, "%s : %s\n", "Error: This label is duplicated", newLabel.labelName);
			exit(0);
		}
	}

	if(newLabel.value > 32767 || newLabel.value < -32768){
		fprintf(stderr, "%s : %s : %d\n", "Error: This .fill label contains a number which does not fit in 16-bit", newLabel.labelName, newLabel.value);
            exit(0);
	}

	labelArray[index] = newLabel;	
}

void print_labelArray(struct Labels *labelArray, int length){
	int i;
	for(i=0; i<length; i++){
		printf("Label :: %s, Address :: %d, isDotFill? :: %d, Immediate Value :: %d \n", labelArray[i].labelName, labelArray[i].address, labelArray[i].isDotFill, labelArray[i].value);
	}
}


