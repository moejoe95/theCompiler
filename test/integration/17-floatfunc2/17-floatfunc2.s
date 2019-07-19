.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $68, %esp
        flds    x_0
        fstps   -4(%ebp)
L_main_3:
        flds    -4(%ebp)
        flds    tmp_1
        fcomip  %st(1), %st
        fstp    %st(0)
        seta    %al
        movzbl  %al, %eax
        movl    %eax, -8(%ebp)
        cmpl    $0, %eax
        je      L_main_14
        flds    -4(%ebp)
        fstps   -12(%ebp)
        pushl   -12(%ebp)
        calll   transform
        addl    $4, %esp
        movl    %eax, -20(%ebp)
        movl    -20(%ebp), %eax
        movl    %eax, -24(%ebp)
        flds    tmp_2
        flds    -4(%ebp)
        faddp
        fstps   -28(%ebp)
        movl    -28(%ebp), %eax
        movl    %eax, -4(%ebp)
        flds    -28(%ebp)
        fstps   -32(%ebp)
        pushl   -32(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        jmp     L_main_3
L_main_14:
        movl    $0, %eax
        leave
        ret

        .globl transform

transform:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $16, %esp
        movl    8(%ebp), %eax
        movl    %eax, -4(%ebp)
        movl    tmp_3, %eax
        addl    $16, %esp
        popl    %ebp
        ret

.data

x_0:
        .float 0.000000

tmp_1:
        .float 5.000000

tmp_2:
        .float 1.000000

tmp_3:
        .float 1.000000
        