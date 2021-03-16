main:   add $t0, $t0, 4
        blez $t0, label2
label:  add $t1, $t1, 4
label2: add $t1, $t1, 8
        .word 0xfeedfeed
