.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $180, %esp
        flds    k_1
        flds    j_0
        fmulp
        fstps   -4(%ebp)
        flds    -4(%ebp)
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_4
        flds    tmp_5
        fsubp
        fstps   -16(%ebp)
        flds    tmp_6
        flds    j_2
        fdivp
        fstps   -20(%ebp)
        flds    -20(%ebp)
        flds    -16(%ebp)
        fmulp
        fstps   -24(%ebp)
        flds    -24(%ebp)
        flds    tmp_3
        faddp
        fstps   -28(%ebp)
        movl    -28(%ebp), %eax
        movl    %eax, -32(%ebp)
        flds    -32(%ebp)
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_8
        flds    tmp_9
        fsubp
        fstps   -40(%ebp)
        flds    tmp_10
        flds    j_7
        fdivp
        fstps   -44(%ebp)
        flds    -44(%ebp)
        flds    -40(%ebp)
        fmulp
        fstps   -48(%ebp)
        flds    -48(%ebp)
        flds    tmp_11
        fsubp
        fstps   -52(%ebp)
        flds    tmp_12
        flds    -52(%ebp)
        fmulp
        fstps   -56(%ebp)
        movl    -56(%ebp), %eax
        movl    %eax, -60(%ebp)
        flds    -60(%ebp)
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        movl    $0, %eax
        leave
        ret

.data

j_0:
        .float 2.000000

k_1:
        .float 6.000000

j_2:
        .float 100.000000

tmp_3:
        .float -2.000000

tmp_4:
        .float -2.000000

tmp_5:
        .float 1.000000

tmp_6:
        .float 80.000000

j_7:
        .float 100.000000

tmp_8:
        .float -1.000000

tmp_9:
        .float 1.000000

tmp_10:
        .float 50.000000

tmp_11:
        .float 1.000000

tmp_12:
        .float 2.000000
