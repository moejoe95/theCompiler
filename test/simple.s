.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $44, %esp
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
        je      L_main_11
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_2
        flds    -4(%ebp)
        faddp
        fstps   -12(%ebp)
        movl    -12(%ebp), %eax
        movl    %eax, -4(%ebp)
        jmp     L_main_3
L_main_11:
        movl    $0, %eax
        leave
        ret

.data

x_0:
        .float 0.000000

tmp_1:
        .float 5.000000

tmp_2:
        .float 1.000000
