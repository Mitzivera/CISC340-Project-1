## Updates
10/16/2018: Part 2 & part 3 has been implemented 

## How-to run
In the following examples, the assembly-language program to be
assembled is "program.as" and the machine-code file generated would be "program.mc":

  # input example
  $./assembler -i program.as -o program.mc
  $./simulator -i program.mc

The program should print the state of UST-3400 before each instruction is executed.


## Files Used
**simulator.c** : takes machine code and simulates the behaviors of the instructions on the UST-3400 machine.
**makefile** : compiles and cleans our simulator.c

### Test Suite
    testExample : tests the example included in the project description document
    test2: tests the jalr instruction
    test3: tests the nand and sw instructions
