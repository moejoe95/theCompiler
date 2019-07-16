.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $92, %esp
        flds    tmp_1
        flds    f_0
        fcomip  %st(1), %st
        fstp    %st(0)
        sete    %al
        movzbl  %al, %eax
        cmpl    $0, %eax
        je      L_main_7
        pushl   $tmp_2
        calll   print
        addl    $4, %esp
        jmp     L_main_11
L_main_7:
        pushl   $tmp_3
        calll   print
        addl    $4, %esp
L_main_11:
        calll   print_nl
        flds    f_0
        flds    tmp_4
        fcomip  %st(1), %st
        fstp    %st(0)
        seta    %al
        movzbl  %al, %eax
        cmpl    $0, %eax
        je      L_main_17
        pushl   $tmp_5
        calll   print
        addl    $4, %esp
        jmp     L_main_21
L_main_17:
        pushl   $tmp_6
        calll   print
        addl    $4, %esp
L_main_21:
        calll   print_nl
        flds    tmp_7
        flds    f_0
        fcomip  %st(1), %st
        fstp    %st(0)
        setnb   %al
        movzbl  %al, %eax
        cmpl    $0, %eax
        je      L_main_27
        pushl   $tmp_8
        calll   print
        addl    $4, %esp
        jmp     L_main_31
L_main_27:
        pushl   $tmp_9
        calll   print
        addl    $4, %esp
L_main_31:
        calll   print_nl
        flds    tmp_10
        flds    f_0
        fcomip  %st(1), %st
        fstp    %st(0)
        setne   %al
        movzbl  %al, %eax
        cmpl    $0, %eax
        je      L_main_37
        pushl   $tmp_11
        calll   print
        addl    $4, %esp
        jmp     L_main_41
L_main_37:
        pushl   $tmp_12
        calll   print
        addl    $4, %esp
L_main_41:
        calll   print_nl
        flds    tmp_13
        flds    f_0
        fcomip  %st(1), %st
        fstp    %st(0)
        sete    %al
        movzbl  %al, %eax
        cmpl    $0, %eax
        je      L_main_47
        pushl   $tmp_14
        calll   print
        addl    $4, %esp
        jmp     L_main_51
L_main_47:
        pushl   $tmp_15
        calll   print
        addl    $4, %esp
L_main_51:
        calll   print_nl
        movl    $0, %eax
        leave
        ret

.data

f_0:
        .float 1.000000

tmp_1:
        .float 1.000000

tmp_2:
        .string "1 true"

tmp_3:
        .string "1 false"

tmp_4:
        .float 2.000000

tmp_5:
        .string "2 true"

tmp_6:
        .string "2 false"

tmp_7:
        .float 1.000000

tmp_8:
        .string "3 true"

tmp_9:
        .string "3 false"

tmp_10:
        .float 1.000000

tmp_11:
        .string "4 true"

tmp_12:
        .string "4 false"

tmp_13:
        .float 1.000000

tmp_14:
        .string "5 true"

tmp_15:
        .string "5 false"
        