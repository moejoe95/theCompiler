.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $132, %esp
        movl    $7, %eax
        movl    %eax, -4(%ebp)
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    $-7, %eax
        movl    %eax, -8(%ebp)
        movl    -8(%ebp), %eax
        movl    %eax, -4(%ebp)
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    $-7, %eax
        movl    %eax, -12(%ebp)
        movl    -12(%ebp), %eax
        movl    %eax, -4(%ebp)
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        flds    f_0
        fstps   -16(%ebp)
        flds    -16(%ebp)
        fstps   -20(%ebp)
        pushl   -20(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_1
        fstps   -28(%ebp)
        pushl   -28(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_2
        fstps   -36(%ebp)
        pushl   -36(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    f_3
        fstps   -40(%ebp)
        flds    -40(%ebp)
        fchs
        fstps   -44(%ebp)
        movl    -44(%ebp), %eax
        movl    %eax, -16(%ebp)
        flds    -44(%ebp)
        fstps   -48(%ebp)
        pushl   -48(%ebp)
        calll   print_float
        addl    $4, %esp
        movl    $0, %eax
        leave
        ret

.data

f_0:
        .float 7.000000

tmp_1:
        .float -7.000000

tmp_2:
        .float -7.000000

f_3:
        .float 7.000000
