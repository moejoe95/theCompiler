.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $100, %esp
        flds    tmp_0
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   func
        addl    $4, %esp
        flds    a_1
        fstps   -8(%ebp)
        pushl   -8(%ebp)
        calll   func
        addl    $4, %esp
        flds    tmp_2
        fstps   -12(%ebp)
        pushl   -12(%ebp)
        flds    tmp_3
        fstps   -16(%ebp)
        pushl   -16(%ebp)
        calll   func2
        addl    $8, %esp
        calll   func3
        movl    %eax, -24(%ebp)
        flds    -24(%ebp)
        movl    -24(%ebp), %eax
        movl    %eax, -28(%ebp)
        flds    -28(%ebp)
        fstps   -32(%ebp)
        pushl   -32(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_4
        fstps   -36(%ebp)
        pushl   -36(%ebp)
        calll   func4
        addl    $4, %esp
        movl    %eax, -44(%ebp)
        flds    -44(%ebp)
        movl    -44(%ebp), %eax
        movl    %eax, -48(%ebp)
        flds    -48(%ebp)
        fstps   -52(%ebp)
        pushl   -52(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        movl    $0, %eax
        leave
        ret

        .globl func

func:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $16, %esp
        movl    8(%ebp), %eax
        movl    %eax, -4(%ebp)
        flds    -4(%ebp)
        fstps   -8(%ebp)
        pushl   -8(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        movl    -8(%ebp), %eax
        addl    $16, %esp
        popl    %ebp
        ret

        .globl func2

func2:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $48, %esp
        movl    12(%ebp), %eax
        movl    %eax, -4(%ebp)
        movl    8(%ebp), %eax
        movl    %eax, -8(%ebp)
        flds    a_1
        fstps   -12(%ebp)
        pushl   -12(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    -4(%ebp)
        fstps   -16(%ebp)
        pushl   -16(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    -8(%ebp)
        fstps   -20(%ebp)
        pushl   -20(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        movl    -20(%ebp), %eax
        addl    $48, %esp
        popl    %ebp
        ret

        .globl func3

func3:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $8, %esp
        movl    tmp_6, %eax
        addl    $8, %esp
        popl    %ebp
        ret

        .globl func4

func4:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $16, %esp
        movl    8(%ebp), %eax
        movl    %eax, -4(%ebp)
        movl    -4(%ebp), %eax
        addl    $16, %esp
        popl    %ebp
        ret

.data

tmp_0:
        .float 0.000000

a_1:
        .float 1.000000

tmp_2:
        .float 2.000000

tmp_3:
        .float 3.000000

tmp_4:
        .float 5.000000

z_5:
        .float 1.000000

tmp_6:
        .float 4.000000
