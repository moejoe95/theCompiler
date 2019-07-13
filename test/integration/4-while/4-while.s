.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $152, %esp
        movl    $0, %eax
        movl    %eax, -4(%ebp)
L_main_3:
        movl    -4(%ebp), %eax
        cmpl    $10, %eax
        setl    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -8(%ebp)
        cmpl    $0, %eax
        je      L_main_11
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    -4(%ebp), %eax
        addl    $1, %eax
        movl    %eax, -12(%ebp)
        movl    -12(%ebp), %eax
        movl    %eax, -4(%ebp)
        jmp     L_main_3
L_main_11:
        calll   print_nl
L_main_14:
        movl    -4(%ebp), %eax
        cmpl    $0, %eax
        setg    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -16(%ebp)
        cmpl    $0, %eax
        je      L_main_22
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    -4(%ebp), %eax
        subl    $1, %eax
        movl    %eax, -20(%ebp)
        movl    -20(%ebp), %eax
        movl    %eax, -4(%ebp)
        jmp     L_main_14
L_main_22:
        calll   print_nl
        movl    $10, %eax
        movl    %eax, -24(%ebp)
        movl    $0, %eax
        movl    %eax, -28(%ebp)
L_main_27:
        movl    -24(%ebp), %eax
        cmpl    $0, %eax
        setge   %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -32(%ebp)
        movl    -28(%ebp), %eax
        cmpl    $10, %eax
        setl    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -36(%ebp)
        movl    -32(%ebp), %eax
        and     -36(%ebp), %eax
        movl    %eax, -40(%ebp)
        cmpl    $0, %eax
        je      L_main_43
        pushl   -24(%ebp)
        calll   print_int
        addl    $4, %esp
        pushl   $tmp_0
        calll   print
        addl    $4, %esp
        pushl   -28(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    -24(%ebp), %eax
        subl    $2, %eax
        movl    %eax, -44(%ebp)
        movl    -44(%ebp), %eax
        movl    %eax, -24(%ebp)
        movl    -28(%ebp), %eax
        addl    $1, %eax
        movl    %eax, -48(%ebp)
        movl    -48(%ebp), %eax
        movl    %eax, -28(%ebp)
        jmp     L_main_27
L_main_43:
        calll   print_nl
        movl    $0, %eax
        movl    %eax, -52(%ebp)
L_main_47:
        movl    -52(%ebp), %eax
        addl    $1, %eax
        movl    %eax, -56(%ebp)
        movl    -56(%ebp), %eax
        cmpl    $5, %eax
        setl    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -60(%ebp)
        cmpl    $0, %eax
        je      L_main_56
        pushl   -52(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    -52(%ebp), %eax
        addl    $1, %eax
        movl    %eax, -64(%ebp)
        movl    -64(%ebp), %eax
        movl    %eax, -52(%ebp)
        jmp     L_main_47
L_main_56:
        calll   print_nl
        movl    $0, %eax
        movl    %eax, -24(%ebp)
L_main_60:
        movl    -24(%ebp), %eax
        cmpl    $3, %eax
        setl    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -68(%ebp)
        cmpl    $0, %eax
        je      L_main_79
        movl    $0, %eax
        movl    %eax, -28(%ebp)
        pushl   -24(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
L_main_67:
        movl    -28(%ebp), %eax
        cmpl    $3, %eax
        setl    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -72(%ebp)
        cmpl    $0, %eax
        je      L_main_74
        pushl   -28(%ebp)
        calll   print_int
        addl    $4, %esp
        movl    -28(%ebp), %eax
        addl    $1, %eax
        movl    %eax, -76(%ebp)
        movl    -76(%ebp), %eax
        movl    %eax, -28(%ebp)
        jmp     L_main_67
L_main_74:
        calll   print_nl
        movl    -24(%ebp), %eax
        addl    $1, %eax
        movl    %eax, -80(%ebp)
        movl    -80(%ebp), %eax
        movl    %eax, -24(%ebp)
        jmp     L_main_60
L_main_79:
        movl    $0, %eax
        leave
        ret

.data

tmp_0:
        .string " "
