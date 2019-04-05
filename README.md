# Yet another mc Compiler

This repository holds the code-base from UIBK [compiler construction course](https://git.uibk.ac.at/c7031162/703602-Compiler-Construction) group 5.

## Authors
- Andreas Peintner (1515339)
- Josef Gugglberger (1518418)
- Tobias Kupek (11828471)

## Prerequisites

- [Meson](http://mesonbuild.com/) in a recent version (`0.44.0`)
  (you may want to install it via `pip3 install --user meson`)
- [Ninja](https://ninja-build.org/)
- `time`, typically located at `/usr/bin/time`, do not confuse this with the Bash built-in
- `flex` for generating the lexer
- `bison` for generating the parser
- A compiler supporting C11 — typically GCC or Clang

## Build Instructions

First, generate the build directory.

    $ meson builddir
    $ cd builddir

Meson creates Ninja build files.
Let's build.

    $ ninja

Unit tests can be run directly with Ninja (or Meson).

    $ ninja test

For integration testing we try to compile .mc programs and compare their output for a given input.

    $ ../scripts/run_integration_tests

Taken from the [Meson Documentation](https://mesonbuild.com/Unit-tests.html#coverage):

> If you enable coverage measurements by giving Meson the command line flag `-Db_coverage=true`, you can generate coverage reports.
> Meson will autodetect what coverage generator tools you have installed and will generate the corresponding targets.
> These targets are `coverage-xml` and `coverage-text` which are both provided by Gcovr and `coverage-html`, which requires Lcov and GenHTML or Gcovr with html support.

## Milestone 1 Parser

__Main goals__

- Inputs are accepted / rejected correctly (syntax only).
- Syntactically invalid inputs result in a meaningful error message containing the corresponding source location.
- An AST is constructed for valid inputs.
- The obtained AST can be printed in the DOT format (see mc_ast_to_dot).

__Testing the objective__

To verify the correct behavious, please follow the [Build Instructions](#build-instructions) from above.
Afterwards the `mc_ast_to_dot` binary can produce the DOT output:

```
cat ../examples/euclid/euclid.mc | ./mc_ast_to_dot
```

A graphical output is available through graphviz:

```
cat ../examples/euclid/euclid.mc | ./mc_ast_to_dot | dot -Tpng > euclid_ast.png
```

__Example output__

![AST of euclid.mc](./docs/images/euclid_ast.png "AST of euclid.mc")

__Examples for rejected code__

Code with incorrect syntax is rejected with an error that includes the line, column and a message.

Examples are an invalid type, an incorrect comment and a missing semicolon.

```
echo 'int main() {int i; ind e;}' | ./mc_ast_to_dot
echo 'int main() {/ this is an incorrect comment}' | ./mc_ast_to_dot
echo 'int main() {int i; i = 1 + 1}' | ./mc_ast_to_dot
```


## Milestone 2

TODO

## Known Issues
* Units Tests not passing
    * parser only accepts inputs that are valid mC code (only functions as top level)
* memory leak in delete_func_list
* strdup function in scanner.l leaks memory
* parser leaks memory on invalid inputs
