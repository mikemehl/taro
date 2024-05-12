#ifndef __OPCODES_H__
#define __OPCODES_H__

typedef enum TaroOpcode {
  // nop
  NOP,
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
  /* ADDF, */
  /* SUBF, */
  /* MULF, */
  /* DIVF, */
  // memory & registers
  LD,
  LDI,
  ST,
  MOV,
  PUSH, // push to next frame to be called
  PULL, // pull from next frame to be called
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
  // MAX
  MAX_OPCODES,
  INVALID = MAX_OPCODES
} TaroOpcode;

#endif
