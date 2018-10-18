## Updates
10/10/2018: Part 1 has been implemented 

## How-to run
In the following examples, the assembly-language program to be
assembled is "program.as" and the machine-code file generated would be "program.mc":

  #One input example
  
  $./assembler -i <filename.as> > <filename.mc>
  
  #Two input example
  
  $./assembler -i <filename.as> -o <filename.mc>


The program should store only the list of decimal numbers in the machine-code file, one instruction
per line.

## Files Used
**assembler.c** : translates the assembly-language into machine code (decimal value) and stores that value in an output file
**makefile** : compiles and cleans our assembler.c
### Test Suite
	duplicateLabels.as : tests that when there are duplicate labels, the program throws an error message and terminates
	labelDoesNotExist.as : tests that when a label does not exist when referenced, the program throws an error message and terminates
	labelSameNameAsOpcode.as : tests that when a label is the same name as an opcode, the program interprets it as a label
	labelStartsWithNumber.as : tests that when a label starts with a number, the proram throws an error message and terminates
	labelTooLong.as : tests that when a label is longer than 6 characters, the program throws an error message and terminates
	labelWithWeirdCharacter.as : tests that when there is a non-alphanumeric character in a label, the program throws an error message and terminates
	noLabelsAndOnlySpacesWithComments.as : tests an assembly-language program with no labels, spaces as white space, and comments
	offsetFieldGreaterThan16Bits.as : tests that when an offsetfield (numeric address) cannot fit into 16 bits, the program throws an error message and terminates
	offsetFieldGreaterThan16Bits2.as : tests that when an offsetfield (symbolic address / using label) cannot fit into 16 bits, the program throws an error message and terminates
	offsetField LessThan16Bits.as : tests that when an offsetfield cannot fit into 16 bits (negative), the program throws an error message and terminates
	otherOpcodes.as : tests nand, jalr, sw
	registerOutOfRange.as : tests that when a register is not between 0 and 7, the program throws an error message and terminates
	testExample.as : tests the example included in the project description document
	unrecognizedOpcode.as : tests an opcode that doesn't exist on the UST-3400 computer, the program throws an error message and terminates
	
