.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $16, %esp
        pushl   $tmp_0
        calll   print
        addl    $4, %esp
        movl    $0, %eax
        leave
        ret
        calll   print_nl
        pushl   $tmp_1
        calll   print
        addl    $4, %esp
        movl    $0, %eax
        leave
        ret

.data

tmp_0:
        .string "before return"

tmp_1:
        .string "after return"
