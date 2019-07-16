.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $132, %esp
        flds    y_0
        fstps   -4(%ebp)
L_main_3:
        flds    -4(%ebp)
        flds    tmp_1
        fcomip  %st(1), %st
        fstp    %st(0)
        seta    %al
        movzbl  %al, %eax
        cmpl    $0, %eax
        je      L_main_11
        flds    -4(%ebp)
        fstps   -12(%ebp)
        pushl   -12(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_2
        flds    -4(%ebp)
        faddp
        fstps   -16(%ebp)
        movl    -16(%ebp), %eax
        movl    %eax, -4(%ebp)
        jmp     L_main_3
L_main_11:
        calll   print_nl
        flds    z_3
        fstps   -20(%ebp)
L_main_15:
        flds    -20(%ebp)
        flds    tmp_4
        fcomip  %st(1), %st
        fstp    %st(0)
        seta    %al
        movzbl  %al, %eax
        cmpl    $0, %eax
        je      L_main_39
        pushl   $tmp_5
        calll   print
        addl    $4, %esp
        flds    -20(%ebp)
        fstps   -28(%ebp)
        pushl   -28(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    x_6
        fstps   -32(%ebp)
L_main_24:
        flds    -32(%ebp)
        flds    tmp_7
        fcomip  %st(1), %st
        fstp    %st(0)
        seta    %al
        movzbl  %al, %eax
        cmpl    $0, %eax
        je      L_main_34
        pushl   $tmp_8
        calll   print
        addl    $4, %esp
        flds    -32(%ebp)
        fstps   -40(%ebp)
        pushl   -40(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        flds    tmp_9
        flds    -32(%ebp)
        faddp
        fstps   -44(%ebp)
        movl    -44(%ebp), %eax
        movl    %eax, -32(%ebp)
        jmp     L_main_24
L_main_34:
        calll   print_nl
        flds    tmp_10
        flds    -20(%ebp)
        faddp
        fstps   -48(%ebp)
        movl    -48(%ebp), %eax
        movl    %eax, -20(%ebp)
        jmp     L_main_15
L_main_39:
        movl    $0, %eax
        leave
        ret

.data

y_0:
        .float 0.000000

tmp_1:
        .float 5.000000

tmp_2:
        .float 1.000000

z_3:
        .float 0.000000

tmp_4:
        .float 3.000000

tmp_5:
        .string "z "

x_6:
        .float 0.000000

tmp_7:
        .float 3.000000

tmp_8:
        .string "x "

tmp_9:
        .float 1.000000

tmp_10:
        .float 1.000000
