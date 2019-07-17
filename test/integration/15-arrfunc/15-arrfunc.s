.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $36, %esp
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
        calll   func
        pushl   $0
        calll   func2
        addl    $4, %esp
        movl    %eax, -4(%ebp)
        movl    -4(%ebp), %eax
        movl    %eax, -8(%ebp)
        pushl   -8(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    $4, %edi
        movl    arr(%edi), %eax
        addl    $1, %eax
        movl    %eax, -12(%ebp)
        pushl   -12(%ebp)
        calll   func
        addl    $4, %esp
        pushl   $0
        calll   func4
        addl    $4, %esp
        movl    $0, %edi
        pushl   arr(%edi)
        calll   print_int
        calll   print_nl
        movl    $0, %eax
        leave
        ret

        .globl func

func:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $8, %esp
        movl    8(%ebp), %eax
        movl    %eax, -4(%ebp)
        pushl   -4(%ebp)
        calll   print_int
        addl    $4, %esp
        calll   print_nl
        movl    -4(%ebp), %eax
        addl    $8, %esp
        popl    %ebp
        ret

        .globl func2

func2:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $8, %esp
        movl    8(%ebp), %eax
        movl    %eax, -4(%ebp)
        movl    $4, %edi
        movl    arr(%edi), %eax
        addl    $8, %esp
        popl    %ebp
        ret

        .globl func4

func4:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $8, %esp
        movl    8(%ebp), %eax
        movl    %eax, -4(%ebp)
        movl    $0, %edi
        movl    $3, %eax
        movl    %eax, arr(%edi)
        movl    arr(%edi), %eax
        movl    -4(%ebp), %eax
        addl    $8, %esp
        popl    %ebp
        ret

.data

arr:
        .long 0, 0, 0
