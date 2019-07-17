.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $140, %esp
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
        calll   print_nl
        movl    $4, %edi
        flds    tmp_3
        fstps   arr(%edi)
        movl    $4, %edi
        pushl   arr(%edi)
        calll   print_float
        calll   print_nl
        movl    $8, %edi
        flds    tmp_4
        fstps   arr(%edi)
        movl    $4, %edi
        movl    $8, %edi
        movl    $4, %edi
        flds    arr(%edi)
        movl    $8, %edi
        flds    arr(%edi)
        fmulp
        fstps   -24(%ebp)
        flds    -24(%ebp)
        fstps   -28(%ebp)
        pushl   -28(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        movl    $4, %edi
        flds    arr(%edi)
        fstps   -32(%ebp)
        flds    -32(%ebp)
        fstps   -36(%ebp)
        pushl   -36(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
        movl    $0, %edi
        movl    $8, %edi
        movl    $0, %edi
        flds    arr(%edi)
        movl    $8, %edi
        flds    arr(%edi)
        faddp
        fstps   -40(%ebp)
        movl    -40(%ebp), %eax
        movl    %eax, -44(%ebp)
        flds    -44(%ebp)
        fstps   -48(%ebp)
        pushl   -48(%ebp)
        calll   print_float
        addl    $4, %esp
        calll   print_nl
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

tmp_3:
        .float 9.000000

tmp_4:
        .float 2.000000
