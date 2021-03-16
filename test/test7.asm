addi $t0, $t0, -1
addi $t2, $t2, -1
add $t1, $t1, 24
sh  $t0, 0($t1)
.word 0xfeedfeed
.word 0xeeeeeeee
.word 0xeeeeeeee
.word 0xeeeeeeee
.word 0xeeeeeeee