#ifndef __OPCODES_H__
#define __OPCODES_H__

#define OPCODE_OP(a) (a & (0xFF << 24))
#define OPCODE_RD(a) (a & (0xFF << 16))
#define OPCODE_R1(a) (a & (0xFF << 8))
#define OPCODE_R2(a) (a & (0xFF))

typedef enum TaroOpcode {
  // math
  ADD,
  SUB,
  MUL,
  DIV,
  SHL,
  SHR,
  AND,
  OR,
  NOT,
  XOR,
  MOD,
  ADDF,
  SUBF,
  MULF,
  DIVF,
  // memory & registers
  LD,
  LDI,
  ST,
  MOV,
  // control flow
  JMP,
  JCN,
  CALL,
  RET,
  INT,
  BRK,
  // threading
  THRD,
  SEND,
  JOIN,
  STOP,
  // i/o
  IN,
  OUT,
  // debug
  DBG,
} TaroOpcode;

#endif
