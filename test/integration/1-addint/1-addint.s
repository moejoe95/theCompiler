
.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $28, %esp
        movl    $1, %eax
        movl    %eax, -4(%ebp)
        movl    $2, %eax
        movl    %eax, -8(%ebp)
        movl    -4(%ebp), %eax
        addl    -8(%ebp), %eax
        movl    %eax, -12(%ebp)
        movl    -12(%ebp), %eax
        addl    -4(%ebp), %eax
        movl    %eax, -16(%ebp)
        movl    -16(%ebp), %eax
        movl    %eax, -20(%ebp)
        pushl   -20(%ebp)
        calll   print_int
        addl    $4, %esp
        movl    $0, %eax
        leave
        ret