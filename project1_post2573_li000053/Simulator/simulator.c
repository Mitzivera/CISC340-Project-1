#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define NUMMEMORY 65536
#define NUMREGS 8

typedef struct state_struct {
	int pc;
	long int mem[NUMMEMORY];
	int reg[NUMREGS];
	int num_memory;
}statetype;

void print_state(statetype *stateptr);
void print_usage();

int main (int argc, char **argv){
	char *inputFilename = (char*)malloc(sizeof(char)*256);

	int opt;

	char *line = (char*)malloc(sizeof(char)*100);
	int address;
	
/////////////////     taking input argument ///////////////////////

	while((opt = getopt(argc, argv, "i:")) != -1){
		switch(opt){
			case 'i':
				inputFilename = optarg;
				break;				
			
			default:
				print_usage();
		}
	}

/////////////// initialize registers and program counter //////////////
	statetype *machine_state;
	machine_state->pc = 0;
	for(int i = 0; i < NUMREGS; i++)
	{
		machine_state->reg[i] = 0;
	} 
	
////////////start reading the file line by line //////////
        FILE *rf = fopen(inputFilename, "r");

        while((fgets(line, 100, rf)) != NULL){
		char ** pEnd;
                long int temp =  strtol(strtok(line, " \t\n\v\f\r"), pEnd, 10);
		
        }
	fclose(rf);
}

void print_usage(){
    fprintf(stderr, "Usage: ./assembler -i program.mc\n");
    exit(0);
}

void print_state(statetype *stateptr){
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", stateptr->pc);
	printf("\tmemory:\n");

	for(i = 0; i < stateptr->num_memory; i++){
		printf("\t\tmem[%d]=%li\n", i, stateptr->mem[i]);
	}

	printf("\tregisters:\n");
	
	for(i = 0; i < NUMREGS; i++){
		printf("\t\treg[%d]=%d\n", i, stateptr->reg[i]);
	}
		printf("end state\n");
}
