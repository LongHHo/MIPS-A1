main:   addi $t0, $t0, 8     #t0 should be 8
        addiu $t1, $t0, -16  # t1 should -8 twos compl
        andi $t2, $t0, 65535 #t2 should be 8
        beq  $t0, $t2, label1
        add  $t0, $t2, $t0   # should not happen
label1: lbu  $t3, 76($zero)  # 12
        lbu  $a1, 77($zero)  # 34
        lhu  $t3, 79($zero)  # cdcd ??
        lhu  $a0, 80($zero)  # cdef
        lw   $t4, 88($zero)    # 
        lui  $t5, 65535         
        ori  $t6, $zero, 65535 # first 16 bits should be 1
        addi $t7, $t7, -1
        slt  $t8, $t7, 65535    # t8 = 1
        sltu $t9, $t7, 65535    # t9 = 0
        .word 0xfeedfeed
        .word 0x1234abcd   #76
        .word 0xcdef4567   # 80
        .word 0xefefefef   # 84
        .word 0xadcdefdf   # 88
        .word 0x1234abcd