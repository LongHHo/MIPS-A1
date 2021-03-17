main:   add $t0, $t0, 6
        add $t1, $t1, 6
        addi $t2, $t3, 8
        beq $t0, $t1, label2
label2: .word 0xfeedfeed
        .word 0xffffffff
