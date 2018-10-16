 add 7 0 0 				# result = 0 -> reg7
 lw 2 0 mplier	 		# multiplier -> reg2
 lw 3 0 mcand			# multiplicand -> reg3
 add 5 0 0				# i = 0 -> reg5
 lw 6 0 one				# mask = 1 -> reg6
start lw 4 0 numB 		# reg4 = 32
 beq 5 4 done			# terminate the program if (i==32)
 nand 1 2 6				# reg1 = nand(multiplier, mask)
 add 1 1 6				# reg1 += mask
 lw 4 0 negO			# reg4 = -1
 beq 1 4 addi			# only do addition if the value in reg1 == -1
 beq 0 0 rest 			# thing below is only for addition
addi add 7 7 3			# result += multiplicand
rest add 3 3 3			# multiplicand <<= 1
 add 6 6 6				# mask <<= 1
 lw 1 0 one				# reg1 = 1
 add 5 5 1 				# i += 1
 beq 0 0 start			# jump to the start of the loop
done halt
one .fill 1
negO .fill -1
mplier .fill 29562
mcand .fill 11834
numB .fill 32
