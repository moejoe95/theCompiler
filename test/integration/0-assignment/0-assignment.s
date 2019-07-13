.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $16, %esp
        movl    $0, %eax
        movl    %eax, -4(%ebp)
        movl    $1, %eax
        movl    %eax, -4(%ebp)
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        movl    $0, %eax
        leave
        ret