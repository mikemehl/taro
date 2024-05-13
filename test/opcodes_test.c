#include "opcodes.h"
#include "taro.h"
#include "utest.h"

UTEST(opcodes, ld) {
  uint8_t mem[] = {
      LDI,  0x00, 0x01, 0x00, 0x00, 0x00, LDI,  0x01, 0x01,
      0x00, 0x00, 0x00, LD,   0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 1);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 1);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], (*(uint32_t *)(&mem[2])));
}

UTEST(opcodes, st) {
  uint8_t mem[] = {
      LDI, 0x00, 0x01, 0x00, 0x00, 0x00, LDI, 0x01, 0x47, 0x00, 0x00, 0x00,
      LDI, 0x02, 0xFF, 0x00, 0x00, 0x00, ST,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 1);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x47);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0x0FF);
  ASSERT_EQ(t.mem.mem[0x47 + 1], 0xFF);
}

UTEST(opcodes, mov) {
  uint8_t mem[] = {
      LDI,  0x00, 0x01, 0x00, 0x00, 0x00, LDI,  0x01, 0x47, 0x00, 0x00,
      0x00, LDI,  0x02, 0xFF, 0x00, 0x00, 0x00, MOV,  0x02, 0x00, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 1);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x47);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0x01);
}

UTEST(opcodes, push) {
  uint8_t mem[] = {
      LDI,  0x00, 0x78, 0x00, 0x00, 0x00, LDI,  0x01, 0x47, 0x00, 0x00,
      0x00, LDI,  0x02, 0xFF, 0x00, 0x00, 0x00, PUSH, 0x01, 0x00, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 0x78);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x47);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0xFF);
  ASSERT_EQ(t.threads[0].frames[1].regs[1], 0x78);
}

UTEST(opcodes, pull) {
  uint8_t mem[] = {
      LDI,  0x00, 0x78, 0x00, 0x00, 0x00, LDI,  0x01, 0x47, 0x00, 0x00,
      0x00, LDI,  0x02, 0xFF, 0x00, 0x00, 0x00, PULL, 0x01, 0x00, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  t.threads[0].frames[1].regs[0] = 0x69;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 0x78);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x69);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0xFF);
}

UTEST(opcodes, add) {
  uint8_t mem[] = {
      LDI,  0x00, 0x01, 0x00, 0x00, 0x00, LDI,  0x01, 0x01,
      0x00, 0x00, 0x00, ADD,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 1);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 1);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 2);
}

UTEST(opcodes, sub) {
  uint8_t mem[] = {
      LDI,  0x00, 0x03, 0x00, 0x00, 0x00, LDI,  0x01, 0x04,
      0x00, 0x00, 0x00, SUB,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 3);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 4);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], -1);
}

UTEST(opcodes, mul) {
  uint8_t mem[] = {
      LDI,  0x00, 0x03, 0x00, 0x00, 0x00, LDI,  0x01, 0x04,
      0x00, 0x00, 0x00, MUL,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 3);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 4);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 12);
}

UTEST(opcodes, div) {
  uint8_t mem[] = {
      LDI,  0x00, 0x0a, 0x00, 0x00, 0x00, LDI,  0x01, 0x05,
      0x00, 0x00, 0x00, DIV,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 10);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 5);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 2);
}

UTEST(opcodes, shl) {
  uint8_t mem[] = {
      LDI,  0x00, 0x01, 0x00, 0x00, 0x00, LDI,  0x01, 0x05,
      0x00, 0x00, 0x00, SHL,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 1);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 5);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], (1 << 5));
}

UTEST(opcodes, shr) {
  uint8_t mem[] = {
      LDI,  0x00, 0x10, 0x00, 0x00, 0x00, LDI,  0x01, 0x04,
      0x00, 0x00, 0x00, SHR,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 0x10);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 4);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], (0x10 >> 4));
}

UTEST(opcodes, and) {
  uint8_t mem[] = {
      LDI,  0x00, 0x12, 0x00, 0x00, 0x00, LDI,  0x01, 0x02,
      0x00, 0x00, 0x00, AND,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 0x12);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x02);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0x02);
}

UTEST(opcodes, or) {
  uint8_t mem[] = {
      LDI,  0x00, 0x10, 0x00, 0x00, 0x00, LDI,  0x01, 0x0F,
      0x00, 0x00, 0x00, OR,   0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 0x10);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x0F);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0x1F);
}

UTEST(opcodes, not ) {
  uint8_t mem[] = {
      LDI, 0x00, 0x10, 0x00, 0x00, 0x00, NOT, 0x02, 0x00, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 0x10);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], ~0x10);
}

UTEST(opcodes, xor) {
  uint8_t mem[] = {
      LDI,  0x00, 0x10, 0x00, 0x00, 0x00, LDI,  0x01, 0x0F,
      0x00, 0x00, 0x00, XOR,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 0x10);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x0F);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], (0x10 ^ 0x0F));
}

UTEST(opcodes, mod) {
  uint8_t mem[] = {
      LDI,  0x00, 0x10, 0x00, 0x00, 0x00, LDI,  0x01, 0x0F,
      0x00, 0x00, 0x00, MOD,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 0x10);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x0F);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], (0x10 % 0x0F));
}

UTEST(opcodes, jmp) {
  uint8_t mem[] = {LDI,  0x00, 0x06, 0x00, 0x00, 0x00, LDI,  0x01,
                   0x0a, 0x00, 0x00, 0x00, JMP,  0x00, 0x01, BRK,
                   LDI,  0x02, 0xFF, 0xFF, 0xFF, 0xFF, BRK};
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].pc, 22);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 6);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 10);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0xFFFFFFFF);
}

UTEST(opcodes, jmpi) {
  uint8_t mem[] = {LDI,  0x00, 0x06, 0x00, 0x00, 0x00, LDI,  0x01, 0x0a,
                   0x00, 0x00, 0x00, JMPI, 0x12, 0x00, 0x00, 0x00, BRK,
                   LDI,  0x02, 0xFF, 0xFF, 0xFF, 0xFF, BRK};
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].pc, 24);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 6);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 10);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0xFFFFFFFF);
}

UTEST(opcodes, jcn) {
  uint8_t mem[] = {LDI,  0x00, 0x07, 0x00, 0x00, 0x00, LDI,  0x01, 0x10, 0x00,
                   0x00, 0x00, LDI,  0x02, 0x0b, 0x00, 0x00, 0x00, JCN,  0x00,
                   0x01, 0x02, BRK,  LDI,  0x02, 0xFF, 0xFF, 0xFF, 0xFF, BRK};
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].pc, 29);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 7);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x10);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0xFFFFFFFF);

  taro_reset(&t);
  t.mem.mem[14] = 0x00;
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].pc, 22);
  ASSERT_EQ(t.threads[0].frames[0].regs[0], 7);
  ASSERT_EQ(t.threads[0].frames[0].regs[1], 0x10);
  ASSERT_EQ(t.threads[0].frames[0].regs[2], 0);
}
