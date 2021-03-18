# Load 2147483647 into $s1
LUI $s0, 32767
ORI $s1, $s0, 65535

# Add 1 to $s1 and store in $s2. This should produce an overflow exception
ADDI $s2, $s1, 1
.word 0xfeedfeed