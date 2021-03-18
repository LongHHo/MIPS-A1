main:  addi $t0, $zero, 4
       add  $t0, $t0, $t0
       and  $s0, $t0, $s0  # s0 is 0
       addi $s1, $t1, 8
       and  $s2, $s1, $s2  # s2 is and of 0 and 8 
       addi $t1, $t1, 36
       addi $t3, $t3, 5555   
       jr   $t1 
       addi $t1, $t1, 60000 # skips this instruction
       nor  $t2, $t2, $t1   # all higher bits should be one except at 5th position
       slt  $t4, $t2, $t3   # should be 0
       sltu $t5, $t2, $t3   # should be 1
       addi $t6, $t6, 4
       sll  $t7, $t6, 4     # should be 4*16
       srl  $t8, $t7, 4     # should be 4
       sub  $t9, $t7, $t6   # should be 60
       subu $s1, $t1, $t2   
       .word 0xfeedfeed

    