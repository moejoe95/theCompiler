## Milestone 3 - IR Generation and Control Flow Graph

__Main goals__

* Valid inputs are converted to IR.
* The IR can be printed (see mc_ir).
* The CFG is generated and can be printed in the DOT format (see mc_cfg_to_dot).

__Theory__

Our IR was inspired by the concept of Triples presented in Lecture 6.

![IR generation](./images/triples.png "IR Example")

__Testing the objective__

To verify the correct behaviour, please follow the [Build Instructions](../README.md#build-instructions).
Afterwards the `mc_ir` binary can produce the intermediate representation and the `mc_cfg_to_dot` will produce an output in the DOT format to visualize the CFG.

```
./mc_ir ../test/integration/euclid/euclid.mc
```

```
./mc_cfg_to_dot ../test/integration/euclid/euclid.mc
```

For the CFG a graphical output is available through graphviz:
(make sure that the log level is set to 0 to make this work)

```
./mc_cfg_to_dot ../test/integration/euclid/euclid.mc | dot -Tpng > euclid_cfg.png
```

__Example output__

```
generate IR code...

-------------------------------------------------------------------------------------------------------------
| index | operation     | argument 1                              | argument 2                              |
-------------------------------------------------------------------------------------------------------------
| 1     | label         | main                                    | -                                       |
| 2     | print         | "Please enter the first number: "       | -                                       |
| 3     | print_nl      | -                                       | -                                       |
| 4     | read_int      | -                                       | -                                       |
| 5     | assign        | n                                       | (4)                                     |
| 6     | print         | "Please enter the second number: "      | -                                       |
| 7     | print_nl      | -                                       | -                                       |
| 8     | read_int      | -                                       | -                                       |
| 9     | assign        | k                                       | (8)                                     |
| 10    | print_nl      | -                                       | -                                       |
| 11    | push          | k                                       | -                                       |
| 12    | push          | n                                       | -                                       |
| 13    | label         | euclid                                  | -                                       |
| 14    | pop           | (5)                                     | -                                       |
| 15    | pop           | (9)                                     | -                                       |
| 16    | ==            | (9)                                     | 0                                       |
| 17    | jumpfalse     | (16)                                    | (22)                                    |
| 18    | push          | n                                       | -                                       |
| 19    | jump          | (20)                                    | -                                       |
| 20    | pop           | (19)                                    | -                                       |
| 21    | jump          | (22)                                    | -                                       |
| 22    | ==            | (5)                                     | 0                                       |
| 23    | jumpfalse     | (22)                                    | (28)                                    |
| 24    | push          | k                                       | -                                       |
| 25    | jump          | (26)                                    | -                                       |
| 26    | pop           | (25)                                    | -                                       |
| 27    | jump          | (28)                                    | -                                       |
| 28    | >             | (5)                                     | (9)                                     |
| 29    | jumpfalse     | (28)                                    | (37)                                    |
| 30    | push          | k                                       | -                                       |
| 31    | -             | (5)                                     | (9)                                     |
| 32    | jump          | (13)                                    | -                                       |
| 33    | push          | (31)                                    | -                                       |
| 34    | jump          | (35)                                    | -                                       |
| 35    | pop           | (34)                                    | -                                       |
| 36    | jump          | (44)                                    | -                                       |
| 37    | -             | (9)                                     | (5)                                     |
| 38    | push          | n                                       | -                                       |
| 39    | jump          | (13)                                    | -                                       |
| 40    | push          | (38)                                    | -                                       |
| 41    | jump          | (42)                                    | -                                       |
| 42    | pop           | (41)                                    | -                                       |
| 43    | jump          | (44)                                    | -                                       |
| 44    | assign        | result                                  | (43)                                    |
| 45    | print         | "euclid("                               | -                                       |
| 46    | print_int     | -                                       | -                                       |
| 47    | print         | ", "                                    | -                                       |
| 48    | print_int     | -                                       | -                                       |
| 49    | print         | ") = "                                  | -                                       |
| 50    | print_int     | -                                       | -                                       |
| 51    | print_nl      | -                                       | -                                       |
| 52    | push          | 0                                       | -                                       |
| 53    | jump          | (54)                                    | -                                       |
| 54    | pop           | (53)                                    | -                                       |
-------------------------------------------------------------------------------------------------------------
```

![CFG of euclid.mc](./images/euclid_cfg.png "CFG of euclid.mc")