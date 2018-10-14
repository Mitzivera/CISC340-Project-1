#include <stdio.h>

void print_state(statetype *stateptr);

const int NUMMEMORY = 65536;
const int NUMREGS = 8;

typedef struct state_struct {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int num_memory;
} statetype;

int main(){
	printf("hello, world\n");

}


void print_state(statetype *stateptr){
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", stateptr->pc);
	printf("\tmemory:\n");

	for(i = 0; i < stateptr->nummemory; i++){
		printf("\t\tmem[%d]=%d\n", i, stateptr->mem[i]);
	}

	printf("\tregisters:\n");
	
	for(i = 0; i < NUMREGS; i++){
		printf("\t\treg[%d]=%d\n", i, stateptr->reg[i]);
	}
		printf("end state\n");
}
