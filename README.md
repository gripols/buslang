buslang 😩🚌🚌
============

The 102 Markham Road bus made turing complete.
It's a really crummy brainfuck derivative tl;dr.

## Build + Usage

Compile and run executable with a valid `.bus` file.

```
cc -o busl-jit busl-jit.c
./busl-jit [program.bus]
```

## Keywords

Same as brainfuck, but 102 themed.

* `ROUTE` increment the current cell's value.
* `102` decrement the current cell's value.
* `MARKHAM` move the cell pointer left.
* `ROAD` move the cell pointer right.
* `SOUTHBOUND` start a loop.
* `TOWARDS` end a loop.
* `WARDEN` read a byte.
* `STATION` print a byte.

## Examples

A few programs found in [examples](examples)
that I stole and rewrote to buslang. 

## Planned Future Additions

- Deprecating interpreter with a JIT compiler, supporting x86-64 and ARM.
- Additional logic and keywords.