## Milestone 4 - ASM Generation and GCC Invocation

__Main goals__

* Valid inputs are converted to IR and then to assembly code.
* The assembly code can be printed (see mc_asm).
* GCC is invoked to create the final executable.

__ASM Code__

Our MCC Compiler generates ASM code for x86 processors using the AT&T / GNU syntax.

__GCC Invocation__

The main executable `mcc` will print the ASM code to the temporary file `./asm_tmp.s` and invoke the GCC compiler like this:
```
gcc -m32 "../resources/mc_builtins.c" "./asm_tmp.s" -o "a.out"
```

The name of the resulting executable can be overwritten with the `-o` parameter of `mcc`.

__Testing the objective__

To verify the correct behaviour, please follow the [Build Instructions](../README.md#build-instructions).
Afterwards the `mc_asm` binary can produce the ASM code and `mcc` will put everything together to generate the executable.

```
./mc_asm ../test/integration/mandelbrot/mandelbrot.mc
```
(to output all previous steps before the ASM code, set the log level to 1)


```
./mcc ../test/integration/mandelbrot/mandelbrot.mc
```
(default log level 0 will just output errors, temporary ASM file and the executable, log level 1 can be used for debugging)


__Example output__

Example input
```
int main() {
    int x;
    int y;

    x = 0;
    y = 1;

    if(x != y) {
        print("hello ASM");
        print_nl();
    }

    return 0;
}

```

Resulting ASM code
```
.text

        .globl main

main:
        pushl   %ebp
        movl    %esp, %ebp
        subl    $20, %esp
        movl    $0, %eax
        movl    %eax, -4(%ebp)
        movl    $1, %eax
        movl    %eax, -8(%ebp)
        movl    -4(%ebp), %eax
        cmpl    -8(%ebp), %eax
        setne   %cl
        and     $1, %cl
        movzbl  %cl, %eax
        movl    %eax, -12(%ebp)
        cmpl    $0, %eax
        je      L_main_8
        pushl   $tmp_0
        calll   print
        addl    $4, %esp
        calll   print_nl
L_main_8:
        movl    $0, %eax
        leave
        ret

.data

tmp_0:
        .string "hello ASM"
```
