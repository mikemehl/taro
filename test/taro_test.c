#include "opcodes.h"
#include "taro.h"
#include "utest.h"

UTEST(taro, init) {
  uint32_t const mem_size = 5;
  TaroReturn tr = taro_new(5);
  ASSERT_EQ(tr.rc, TARO_OK);
  ASSERT_EQ(tr.taro.mem.mem_size, mem_size);
}

UTEST(taro, load) {
  uint32_t const mem_size = 50;
  uint8_t mem_to_load[50];
  for (int i = 0; i < mem_size; i++) {
    mem_to_load[i] = i;
  }
  TaroReturn tr = taro_new(mem_size);
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem_to_load, mem_size);
  ASSERT_EQ(rc, TARO_OK);
  for (int i = 0; i < mem_size; i++) {
    ASSERT_EQ(mem_to_load[i], t.mem.mem[i]);
  }
}

UTEST(taro, ld) {
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

UTEST(taro, st) {
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

UTEST(taro, mov) {
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

UTEST(taro, push) {
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

UTEST(taro, pull) {
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

UTEST(taro, add) {
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

UTEST(taro, sub) {
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

UTEST(taro, mul) {
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

UTEST(taro, div) {
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

UTEST(taro, shl) {
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

UTEST(taro, shr) {
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

UTEST(taro, and) {
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

UTEST(taro, or) {
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

UTEST(taro, not ) {
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

UTEST(taro, xor) {
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

UTEST(taro, mod) {
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

UTEST(taro, jmp) {
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
