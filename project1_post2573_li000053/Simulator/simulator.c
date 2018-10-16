#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#define NUMMEMORY 65536
#define NUMREGS 8
#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6

typedef struct state_struct {
	int pc;
	long int mem[NUMMEMORY];
	long int reg[NUMREGS];
	int num_memory;
}statetype;

int convert_num(int num);
void print_state(statetype *stateptr);
void print_usage();
void print_stats(int n_instrs);

int main (int argc, char **argv){
	char *inputFilename = (char*)malloc(sizeof(char)*256);
	int num_instructions = 0;
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
	statetype type;
	type.pc = 0;
	for(int i = 0; i < NUMREGS; i++)
	{
		type.reg[i] = 0;
	} 

	type.num_memory = 0;	
////////////start reading the file line by line and put each instruction into memory //////////
        FILE *rf = fopen(inputFilename, "r");

        while((fgets(line, 100, rf)) != NULL){
		char ** pEnd;
                long int temp =  strtol(strtok(line, " \t\n\v\f\r"), pEnd, 10);
		type.mem[type.num_memory] = temp;	
		type.num_memory = type.num_memory + 1; //increment num_memory by 1
        }
	fclose(rf);

///////////start going through memory and completing instructions///////////////
	int destReg;
	int regA;
	int regB;
	int instr;
	int offset;
	int haltFlag = 0;
	while(type.pc < type.num_memory) {
		print_state(&type);
		num_instructions += 1;
		instr = type.mem[type.pc];
		//get the opcode (bits 24-22)
		int opcode = instr >> 22;
		//execute based on opcode type
		if(opcode == ADD){
			destReg = instr & 7;
			regA = (instr & (7 << 19)) >> 19;
			regB = (instr & (7 << 16)) >> 16;
			type.reg[destReg] = type.reg[regA] + type.reg[regB];	
		}
		else if(opcode == NAND){
			destReg = instr & 7;
			regA = (instr & (7 << 19)) >> 19;
			regB = (instr & (7 << 16)) >> 16;
			type.reg[destReg] = ~(type.reg[regA] & type.reg[regB]);
		}
		else if(opcode == LW){
			regA = (instr & (7 << 19)) >> 19 ;
			regB = (instr & (7 << 16)) >> 16;
			offset = instr & 0xFFFF;
			offset = convert_num(offset);
			type.reg[regA] = type.mem[type.reg[regB] + offset];
		}
		else if(opcode == SW){
			regA = (instr & (7 << 19)) >> 19;
                        regB = (instr & (7 << 16)) >> 16;
                        offset = instr & 0xFFFF;
			offset = convert_num(offset);
			type.mem[type.reg[regB] + offset] = type.reg[regA]; 
		}
		else if(opcode == BEQ){
			regA = (instr & (7 << 19)) >> 19;
                        regB = (instr & (7 << 16)) >> 16;
                        offset = instr & 0xFFFF;
			offset = convert_num(offset);
			if(type.reg[regA] == type.reg[regB]){
				type.pc = type.pc +  offset; 
			}
		}
		else if(opcode == JALR){
			regA = (instr & (7 << 19)) >> 19;
                        regB = (instr & (7 << 16)) >> 16;
			type.reg[regA] = type.pc + 1;
			type.pc = type.reg[regB];
		}
		else if(opcode == HALT){
			printf("machine halted\n");
			break;
		}
		type.pc = type.pc + 1;
	}
	print_stats(num_instructions);
}

void print_stats(int n_instrs){
	printf("INSTRUCTIONS: %d\n",n_instrs);
}

int convert_num(int num){
	if(num & (1 << 15)){
		num -= (1<<16);
	}
	return num;
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
		printf("\t\treg[%d]=%li\n", i, stateptr->reg[i]);
	}
		printf("end state\n");
}
