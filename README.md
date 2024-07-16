buslang 😩😩😩
============

The 102 Markham Road bus made turing complete.

## Interpreter Build + Usage

Interpreter source: [src/main.rs](src/main.rs)

Download [Rust](https://www.rust-lang.org/tools/install).
Compile with `rustc main.rs` and execute with a valid buslang program.

I wrote two in [src/examples](src/examples), and there will be more.

## JIT Compiler Build + Usage

I have only tested the JIT on a Linux machine. JIT only
works on machines with X86-64 architecture. 

Run `make` to build the executable and execute with a valid buslang
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

I wrote two in [src/examples](src/examples), and there will be more
if I decide to not be a lazy sack of shit and actually write them.

## Future Additions

I'm getting around to building a version that's actually good,
with some additional features on top. Here's what version 2 will look like:

- A JIT compiler written in pure C
- Bus code conversion to C and vice versa
- YT video explaining how it works and steps I took to creating it
- Lambda calculus 
- Idk send a pr and if its cool I'll prob add it
