## Milestone 1 Parser

__Main goals__

- Inputs are accepted / rejected correctly (syntax only).
- Syntactically invalid inputs result in a meaningful error message containing the corresponding source location.
- An AST is constructed for valid inputs.
- The obtained AST can be printed in the DOT format (see mc_ast_to_dot).

__Testing the objective__

To verify the correct behavious, please follow the [Build Instructions](../README.md#build-instructions).
Afterwards the `mc_ast_to_dot` binary can produce the DOT output:

```
cat ../examples/euclid/euclid.mc | ./mc_ast_to_dot
```

A graphical output is available through graphviz:

```
cat ../examples/euclid/euclid.mc | ./mc_ast_to_dot | dot -Tpng > euclid_ast.png
```

__Example output__

![AST of euclid.mc](./images/euclid_ast.png "AST of euclid.mc")

__Examples for rejected code__

Code with incorrect syntax is rejected with an error that includes the line, column and a message.

Examples are an invalid type, an incorrect comment and a missing semicolon.

```
echo 'int main() {int i; ind e;}' | ./mc_ast_to_dot
echo 'int main() {/ this is an incorrect comment}' | ./mc_ast_to_dot
echo 'int main() {int i; i = 1 + 1}' | ./mc_ast_to_dot
```