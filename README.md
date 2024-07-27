buslang 😩😩😩
============

The 102 Markham Road bus made turing complete.

## Interpreter Build + Usage

Interpreter source: [interpreter.c](interpreter.c)

Compile and run executable with a valid `.bus` file.

## JIT Compiler Build + Usage

JIT only works on machines with x86-64 architecture.

If you are using ARM, use the interpreter (for now).

Run `make` to build the executable and execute with a valid `.bus`
program.

## Keywords

Both the JIT and Interpreter work the same way as follows:

| Keyword      | Usage                                                                                                 |
|--------------|-------------------------------------------------------------------------------------------------------|
| `ROUTE`      | Increment data pointer by 1.                                                                          |
| `102`        | Decrement data pointer by 1.                                                                          |
| `MARKHAM`    | Increment pointed value by 1. Wraps around to 255 if 0.                                               |
| `ROAD`       | Decrement pointed value by 1. Wraps around to 0 if 255.                                               |
| `SOUTHBOUND` | Write the value at the data pointer as a character to the output file.                                |
| `TOWARDS`    | Read a character from the input file into the value at the data pointer.                              |
| `WARDEN`     | If the value at the data pointer is zero, continue execution after the matching `WARDEN` keyword.     |
| `STATION`    | If the value at the data pointer is not zero, continue execution after the matching `STATION` keyword.|
| `PRESTO`     | Trigger the debugging event handler.                                                                  |

## Examples

I wrote a few basic example programs
found in [examples](examples). Not great, but
it's what I can do.

## Future Additions

I'm getting around to building a version that's actually good,
    with some additional features on top. Here's what version 2 will look like:

    - ARM architecture support for JIT
    - Buslang to C/C to Buslang
    - YT video explaining how it works and steps I took to creating it
    - Lambda calculus
    - Idk send a pr and if I like it I'll probably add it
