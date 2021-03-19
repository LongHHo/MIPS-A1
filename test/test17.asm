main:   addi $zero, $zero, 9
        addi $t0, $t0, -5
        addi $t9, $t9, 12  
        blez $t0, label2
        addi $t1, $t1, 9
label2: addi $t2, $t2, -9
        sw $t0, 44($zero)      
        sb $t2, 48($zero)
        sh $t0, 52($zero)
        .word 0xfeedfeed
        .word 0xffffffff    # 36
        .word 0xffffffff    # 40
        .word 0xffffffff    # 44
        .word 0xffffffff



