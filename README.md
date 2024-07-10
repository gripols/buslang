bussed in rust 😩😩😩
============

Massive W for femboys and arch users.
A shitty no good very bad interpreter for 102 lang in rust.

## Build + Usage

Interpreter source: [src/main.rs](src/main.rs)

Download [Rust](https://www.rust-lang.org/tools/install).
Compile with `rustc main.rs` and execute with a valid buslang program.

I wrote two in [src/examples](src/examples), there will be more I swear.

## Keywords 

| Keyword      | Usage                                                                                                 | 
|----------------------------------------------------------------------------------------------------------------------|
| `ROUTE`      | Increment data pointer by 1.                                                                          |
| `102`        | Decrement data pointer by 1.                                                                          |
| `MARKHAM`    | Increment pointed value by 1. Wraps around to 255 if 0.                                               |
| `ROAD`       | Decrement pointed value by 1. Wraps around to 0 if 255.                                               | 
| `SOUTHBOUND` | Write the value at the data pointer as a character to the output file.                                |
| `TOWARDS`    | Read a character from the input file into the value at the data pointer.                              |
| `WARDEN`     | If the value at the data pointer is zero, continue execution after the matching `WARDEN` keyword.     |
| `STATION`    | If the value at the data pointer is not zero, continue execution after the matching `STATION` keyword.|
| `PRESTO`     | Trigger the debugging event handler.                                                                  |


## Future Additions

I'm getting around to building a version that's actually good,
with some additional features on top. Here's what version 2 will look like:

- A JIT compiler written in pure C
- Bus code conversion to C + Rust and vice versa
- YT video explaining how it works and steps I took to creating it
- Idk send a pr and if its cool I'll prob add it
