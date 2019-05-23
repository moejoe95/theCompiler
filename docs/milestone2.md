## Milestone 2 Semantic Checks

__Main goals__

- The compiler rejects semantically wrong inputs.
- Invalid inputs trigger a meaningful error message including source location information.
- Type checking can be traced (see mc_type_check_trace).
- Symbol tables can be viewed (see mc_symbol_table).

__Testing the objective__

To verify the correct behavious, please follow the [Build Instructions](../README.md#build-instructions).
Afterwards the `mc_symbol_table` binary can produce the symbol table output and the `mc_type_check_trace` will perform semantic type checking.

```
./mc_symbol_table ../test/integration/euclid/euclid.mc
```

```
./mc_type_check_trace ../test/integration/euclid/euclid.mc
```

__Example output__

symbol table
```
[symbol_table global]
name            |       type
-----------------------------
print           |       VOID
print_nl                |       VOID
print_int               |       VOID
print_float             |       VOID
read_int                |       INT
read_float              |       FLOAT
euclid          |       INT
main            |       INT

[symbol_table euclid]
name            |       type
-----------------------------
n               |       INT
k               |       INT

[symbol_table main]
name            |       type
-----------------------------
n               |       INT
k               |       INT
result          |       INT

[symbol_table anonymous]
name            |       type
-----------------------------

[symbol_table anonymous]
name            |       type
-----------------------------

[symbol_table anonymous]
name            |       type
-----------------------------

[symbol_table anonymous]
name            |       type
-----------------------------
```

type-checking trace

```
Location        |       Expression      |       Type-LHS - Type-RHS     |       Status
--------------------------------------------------------------------------------------
3:7             |       bin op          |       INT ~ INT               |       VALID
4:9             |       return          |       INT ~ INT               |       VALID
7:7             |       bin op          |       INT ~ INT               |       VALID
8:9             |       return          |       INT ~ INT               |       VALID
11:7            |       bin op          |       INT ~ INT               |       VALID
12:18           |       bin op          |       INT ~ INT               |       VALID
12:9            |       return          |       INT ~ INT               |       VALID
14:21           |       bin op          |       INT ~ INT               |       VALID
14:9            |       return          |       INT ~ INT               |       VALID
1:15            |       declare n       |       INT                     |       VALID
1:22            |       declare k       |       INT                     |       VALID
23:5            |       declare n       |       INT                     |       VALID
24:3            |       assign n        |       INT <- INT              |       VALID
29:5            |       declare k       |       INT                     |       VALID
30:3            |       assign k        |       INT <- INT              |       VALID
33:5            |       declare result  |       INT                     |       VALID
34:8            |       assign result   |       INT <- INT              |       VALID
44:8            |       return          |       INT ~ INT               |       VALID
```

__Examples for rejected code__

Code with semantic errors is rejected with an error that includes the line, column and a message.

Examples are the usage of an undeclared variable, missing return statement or a wrong type in assignment.

```
int main() { i = 1; return 0; } -> ./test.mc:1:15: error: undefined identifier (id 'i')
int main() { } -> ./test.mc:1:4: error: no return value in non void function (func 'main')
int main() { int i; i = "foobar"; return 0; } -> ./test.mc:1:22: error: invalid assignment (expected type 'INT', but got type 'STRING')
```
