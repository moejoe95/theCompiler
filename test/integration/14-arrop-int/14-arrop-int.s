.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $44, %esp
        movl    $0, %edi
        movl    $0, %eax
        movl    %eax, arr(%edi)
        movl    arr(%edi), %eax
        movl    $4, %edi
        movl    $1, %eax
        movl    %eax, arr(%edi)
        movl    arr(%edi), %eax
        movl    $8, %edi
        movl    $2, %eax
        movl    %eax, arr(%edi)
        movl    arr(%edi), %eax
        movl    $0, %edi
        pushl   arr(%edi)
        calll   print_int
        calll   print_nl
        movl    $4, %edi
        pushl   arr(%edi)
        calll   print_int
        calll   print_nl
        movl    $8, %edi
        pushl   arr(%edi)
        calll   print_int
        calll   print_nl
        movl    $4, %edi
        movl    arr(%edi), %eax
        movl    %eax, -4(%ebp)
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    $4, %edi
        movl    arr(%edi), %eax
        movl    $8, %edi
        addl    arr(%edi), %eax
        movl    %eax, -8(%ebp)
        movl    -8(%ebp), %eax
        movl    %eax, -12(%ebp)
        pushl   -12(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    $4, %edi
        movl    $9, %eax
        movl    %eax, arr(%edi)
        movl    arr(%edi), %eax
        movl    $4, %edi
        pushl   arr(%edi)
        calll   print_int
        calll   print_nl
        movl    $8, %edi
        movl    $2, %eax
        movl    %eax, arr(%edi)
        movl    arr(%edi), %eax
        movl    $4, %edi
        movl    arr(%edi), %eax
        movl    $8, %edi
        imul    arr(%edi), %eax
        movl    %eax, -16(%ebp)
        pushl   -16(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    $0, %eax
        leave
        ret

.data

arr:
        .long 0, 0, 0
