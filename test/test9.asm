add $t1, $t1, 30
add $t2, $t2, 30
lw $t0, -4($t1)
sw $t2, -8($t1)
.word 0xfeedfeed
.word 0xaaaaaaaa
.word 0xbbbbbbbb
.word 0xcccccccc
.word 0xdddddddd
.word 0xeeeeeeee
.word 0xffffffff

