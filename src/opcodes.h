#ifndef __OPCODES_H__
#define __OPCODES_H__

typedef enum TaroOpcode {
  // nop
  NOP = 0x00,

  // math
  ADD = 0x10,
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
  LD = 0x20,
  LDI,
  ST,
  MOV,
  PUSH, // push to next frame to be called
  PULL, // pull from next frame to be called

  // control flow
  JMP = 0x30,
  JMPI,
  JCN,
  JCNI,
  CALL,
  CALLI,
  RET,
  INT,
  BRK,

  // threading
  THRD = 0x40,
  SEND,
  JOIN,
  STOP,
  // i/o
  IN = 0x50,
  OUT,
  // debug
  DBG = 0x60,
  // MAX
  MAX_OPCODES,
  INVALID = MAX_OPCODES
} TaroOpcode;

#endif
