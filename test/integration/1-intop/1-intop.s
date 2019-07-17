.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $72, %esp
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
        calll   print_nl
        movl    $2, %eax
        addl    $3, %eax
        movl    %eax, -24(%ebp)
        movl    -24(%ebp), %eax
        movl    %eax, -28(%ebp)
        movl    -28(%ebp), %eax
        negl    %eax
        movl    %eax, -32(%ebp)
        movl    -32(%ebp), %eax
        movl    %eax, -28(%ebp)
        pushl   -28(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    -28(%ebp), %eax
        movl    %eax, -36(%ebp)
        movl    -28(%ebp), %eax
        imul    -28(%ebp), %eax
        movl    %eax, -40(%ebp)
        movl    -40(%ebp), %eax
        movl    %eax, -28(%ebp)
        movl    -28(%ebp), %eax
        movl    $0, %edx
        movl    -36(%ebp), %ecx
        idiv    %ecx
        movl    %eax, -44(%ebp)
        movl    -36(%ebp), %eax
        cmpl    -44(%ebp), %eax
        sete    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -48(%ebp)
        cmpl    $0, %eax
        je      L_main_25
        pushl   -28(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
L_main_25:
        movl    $0, %eax
        leave
        ret
