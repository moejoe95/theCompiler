.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $44, %esp
        calll   print1
        pushl   $tmp_0
        calll   print2
        addl    $4, %esp
        pushl   $12
        pushl   $30
        calll   add
        addl    $8, %esp
        pushl   $59
        pushl   $5
        calll   add2
        addl    $8, %esp
        movl    %eax, -4(%ebp)
        movl    -4(%ebp), %eax
        movl    %eax, -8(%ebp)
        pushl   -8(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        calll   print3
        movl    %eax, -12(%ebp)
        pushl   -12(%ebp)
        calll   print
        addl    $4, %esp
        calll   print_nl
        movl    $0, %eax
        leave
        ret

        .globl print1

print1:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $4, %esp
        pushl   $tmp_1
        calll   print
        addl    $4, %esp
        calll   print_nl
        movl    %ebp, %eax
        addl    $4, %esp
        popl    %ebp
        ret

        .globl print2

print2:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $8, %esp
        movl    8(%ebp), %eax
        movl    %eax, -4(%ebp)
        pushl   -4(%ebp)
        calll   print
        addl    $4, %esp
        calll   print_nl
        movl    -4(%ebp), %eax
        addl    $8, %esp
        popl    %ebp
        ret

        .globl add

add:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $20, %esp
        movl    12(%ebp), %eax
        movl    %eax, -4(%ebp)
        movl    8(%ebp), %eax
        movl    %eax, -8(%ebp)
        movl    -4(%ebp), %eax
        addl    -8(%ebp), %eax
        movl    %eax, -12(%ebp)
        movl    -12(%ebp), %eax
        movl    %eax, -16(%ebp)
        pushl   -16(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    -16(%ebp), %eax
        addl    $20, %esp
        popl    %ebp
        ret

        .globl add2

add2:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $16, %esp
        movl    12(%ebp), %eax
        movl    %eax, -4(%ebp)
        movl    8(%ebp), %eax
        movl    %eax, -8(%ebp)
        movl    -4(%ebp), %eax
        addl    -8(%ebp), %eax
        movl    %eax, -12(%ebp)
        movl    -12(%ebp), %eax
        addl    $16, %esp
        popl    %ebp
        ret

        .globl print3

print3:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $4, %esp
        movl    $tmp_2, %eax
        addl    $4, %esp
        popl    %ebp
        ret

.data

tmp_0:
        .string "world"

tmp_1:
        .string "hello"

tmp_2:
        .string "bye"
