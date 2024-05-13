# taro

taro is a personal virtual machine. I am writing it for fun. It is inspired by projects like [uxn](https://100r.co/site/uxn.html).

## Specs

- 32 bit arithmetic
- variable length opcodes
- built in "threading" model (not yet sure wether to implement actual threads or something more like coroutines)
- virtual register machine (i.e. register windows)
- floating point arithmetic (maybe)
- vegetables

See `src/opcodes.h` for opcode listing and `test/opcodes_test.c` for example usage.

## Make Targets

- `make`: build library and test executable
- `make clean`: clean build directory
- `make bear`: clean and generate `compile_commands.json`
- `make test`: run test executable
- `make lint`: run `clang-tidy`
- `make check`: run `clang-tidy` and test executable with `valgrind`
