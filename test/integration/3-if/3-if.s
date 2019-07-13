.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $128, %esp
        movl    $2, %eax
        movl    %eax, -4(%ebp)
        movl    $10, %eax
        movl    %eax, -8(%ebp)
        cmpl    $0, %eax
        movl    $1, %eax
        addl    $0, %eax
        jz      L_main_6
        pushl   $tmp_0
        calll   print
        addl    $4, %esp
L_main_6:
        calll   print_nl
        cmpl    $0, %eax
        movl    $0, %eax
        addl    $0, %eax
        jz      L_main_12
        pushl   $tmp_1
        calll   print
        addl    $4, %esp
        jmp     L_main_16
L_main_12:
        pushl   $tmp_2
        calll   print
        addl    $4, %esp
L_main_16:
        calll   print_nl
        movl    $1, %eax
        cmpl    $1, %eax
        sete    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -12(%ebp)
        movl    -12(%ebp), %eax
        and     $1, %eax
        movl    %eax, -16(%ebp)
        cmpl    $0, %eax
        je      L_main_22
        pushl   $tmp_3
        calll   print
        addl    $4, %esp
L_main_22:
        calll   print_nl
        movl    -4(%ebp), %eax
        imul    $5, %eax
        movl    %eax, -20(%ebp)
        movl    -20(%ebp), %eax
        cmpl    -8(%ebp), %eax
        sete    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -24(%ebp)
        cmpl    $0, %eax
        je      L_main_29
        pushl   $tmp_4
        calll   print
        addl    $4, %esp
L_main_29:
        calll   print_nl
        movl    $2, %eax
        imul    $1, %eax
        movl    %eax, -28(%ebp)
        movl    -28(%ebp), %eax
        imul    $5, %eax
        movl    %eax, -32(%ebp)
        movl    -8(%ebp), %eax
        imul    $10, %eax
        movl    %eax, -36(%ebp)
        movl    -36(%ebp), %eax
        movl    $0, %edx
        movl    $10, %ecx
        idiv    %ecx
        movl    %eax, -40(%ebp)
        movl    -32(%ebp), %eax
        cmpl    -40(%ebp), %eax
        sete    %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -44(%ebp)
        cmpl    $0, %eax
        je      L_main_40
        pushl   $tmp_5
        calll   print
        addl    $4, %esp
        jmp     L_main_44
L_main_40:
        pushl   $tmp_6
        calll   print
        addl    $4, %esp
L_main_44:
        calll   print_nl
        movl    $1, %eax
        movl    %eax, -48(%ebp)
        movl    $0, %eax
        movl    %eax, -52(%ebp)
        movl    -48(%ebp), %eax
        cmpl    $0, %eax
        cmpl    $0, %eax
        je      L_main_52
        pushl   $tmp_7
        calll   print
        addl    $4, %esp
        jmp     L_main_56
L_main_52:
        pushl   $tmp_8
        calll   print
        addl    $4, %esp
L_main_56:
        calll   print_nl
        movl    -52(%ebp), %eax
        xorl    $1, %eax
        movl    %eax, -60(%ebp)
        movl    -60(%ebp), %eax
        cmpl    $0, %eax
        cmpl    $0, %eax
        je      L_main_62
        pushl   $tmp_9
        calll   print
        addl    $4, %esp
        jmp     L_main_66
L_main_62:
        pushl   $tmp_10
        calll   print
        addl    $4, %esp
L_main_66:
        calll   print_nl
        movl    -52(%ebp), %eax
        xorl    $1, %eax
        movl    %eax, -64(%ebp)
        movl    -64(%ebp), %eax
        cmpl    $0, %eax
        movl    -48(%ebp), %eax
        and     -64(%ebp), %eax
        movl    %eax, -68(%ebp)
        movl    -68(%ebp), %eax
        and     $1, %eax
        movl    %eax, -72(%ebp)
        cmpl    $0, %eax
        je      L_main_74
        pushl   $tmp_11
        calll   print
        addl    $4, %esp
        jmp     L_main_78
L_main_74:
        pushl   $tmp_12
        calll   print
        addl    $4, %esp
L_main_78:
        calll   print_nl
        movl    $0, %eax
        leave
        ret

.data

tmp_0:
        .string "1 true"

tmp_1:
        .string "2 false"

tmp_2:
        .string "2 true"

tmp_3:
        .string "3 true"

tmp_4:
        .string "4 true"

tmp_5:
        .string "5 true"

tmp_6:
        .string "5 false"

tmp_7:
        .string "6 true"

tmp_8:
        .string "6 false"

tmp_9:
        .string "7 true"

tmp_10:
        .string "7 false"

tmp_11:
        .string "8 true"

tmp_12:
        .string "8 false"
