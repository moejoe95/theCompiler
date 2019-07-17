.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $60, %esp
        movl    $0, %edi
        flds    tmp_0
        fstps   arr(%edi)
        movl    $4, %edi
        flds    tmp_1
        fstps   arr(%edi)
        movl    $8, %edi
        flds    tmp_2
        fstps   arr(%edi)
        movl    $0, %edi
        pushl   arr(%edi)
        calll   print_float
        calll   print_nl
        movl    $4, %edi
        pushl   arr(%edi)
        calll   print_float
        calll   print_nl
        movl    $8, %edi
        pushl   arr(%edi)
        calll   print_float
        movl    $0, %eax
        leave
        ret

.data

arr:
        .float 0.0, 0.0, 0.0

tmp_0:
        .float 0.000000

tmp_1:
        .float 1.000000

tmp_2:
        .float 2.000000
