.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $48, %esp
        calll   read_int
        movl    %eax, -4(%ebp)
        movl    -4(%ebp), %eax
        movl    %eax, -8(%ebp)
        pushl   -8(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        calll   read_float
        fstps   -16(%ebp)
        flds    -16(%ebp)
        movl    -16(%ebp), %eax
        movl    %eax, -20(%ebp)
        flds    -20(%ebp)
        fstps   -4(%ebp)
        pushl   -4(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        movl    $c_0, %eax
        movl    %eax, -24(%ebp)
        pushl   -24(%ebp)
        calll   print
        addl    $4, %esp
        movl    $0, %eax
        leave
        ret

.data

c_0:
        .string "hello world"
