# Load 2147483647 into $s1
LUI $s0, 32767
ORI $s1, $s0, 65535
addi $s2, $s2, 1

# Add 1 to $s1 and store in $s2. This should produce an overflow exception
ADD $s3, $s1, $s2
.word 0xfeedfeed