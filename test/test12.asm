main:   addi $t0, $t0, -1
        addi $t1, $t1, 8
        blez $t0, label2
        addi $t6, $t6, 6
label2: .word 0xfeedfeed
        .word 0xeeeeeeee
