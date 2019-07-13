.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $52, %esp
        movl    $42, %eax
        movl    %eax, -4(%ebp)
        movl    $k_1, %eax
        movl    %eax, -8(%ebp)
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        flds    j_0
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        pushl   -8(%ebp)
        calll   print
        addl    $4, %esp
        calll   print_nl
        pushl   $13
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        flds    tmp_2
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        pushl   $tmp_3
        calll   print
        addl    $4, %esp
        calll   print_nl
        movl    $0, %eax
        leave
        ret

.data

j_0:
        .float 3.100000

k_1:
        .string "lorem ipsum"

tmp_2:
        .float 1.110000

tmp_3:
        .string "hello world"