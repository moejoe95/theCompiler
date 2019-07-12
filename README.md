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

## Known Issues

* type checking ignores type of array
* string size of blocks in CFG is fixed, on really large blocks (around 1700 chars), stack is smashed
* general problems with the generated ASM code, some special features (e.g. arrays) are not working correctly
* only a single input files is supported


## Milestones

All milestones are explained in more detail here:

* [Milestone 1 - AST](./docs/milestone1.md)
* [Milestone 2 - Semantic Checks](./docs/milestone2.md)
* [Milestone 3 - IR & CFG](./docs/milestone3.md)
* [Milestone 4 - Backend](./docs/milestone4.md)