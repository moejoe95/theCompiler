.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $100, %esp
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
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_1
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_2
        fstps   -4(%ebp)
        pushl   -4(%ebp)
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
