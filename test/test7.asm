add $t0, $t0, 65535
add $t1, $t1, 20
lbu $t0, 0($t1)
.word 0xfeedfeed
.word 0x01ffffff
.word 0x01ffffff
.word 0x01ffffff
.word 0x01ffffff