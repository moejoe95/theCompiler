.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $124, %esp
        flds    tmp_0
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    f_1
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_2
        flds    f_1
        faddp
        fstps   -4(%ebp)
        movl    -4(%ebp), %eax
        movl    %eax, -8(%ebp)
        flds    -8(%ebp)
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_3
        flds    -8(%ebp)
        fsubp
        fstps   -12(%ebp)
        movl    -12(%ebp), %eax
        movl    %eax, -16(%ebp)
        flds    -16(%ebp)
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_4
        flds    -16(%ebp)
        fmulp
        fstps   -20(%ebp)
        movl    -20(%ebp), %eax
        movl    %eax, -24(%ebp)
        flds    -24(%ebp)
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_5
        flds    -24(%ebp)
        fdivp
        fstps   -28(%ebp)
        movl    -28(%ebp), %eax
        movl    %eax, -32(%ebp)
        flds    -32(%ebp)
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        movl    $0, %eax
        leave
        ret

.data

tmp_0:
        .float 0.000000

f_1:
        .float 1.000000

tmp_2:
        .float 3.000000

tmp_3:
        .float 2.000000

tmp_4:
        .float 5.000000

tmp_5:
        .float 2.000000
        