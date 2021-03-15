main:   addi $t0, $t0, 4
        addi $t1, $t1, 6
        bne  $t1, $t0, label1
label0: addi $t0, $t0, 4
        addi $t2, $t2, 4
label1: ori $t0, $t0, 65535
.word 0xfeedfeed
