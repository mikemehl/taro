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

UTEST(taro, ld_add) {
  uint8_t mem[] = {
      LDI,  0x00, 0x01, 0x00, 0x00, 0x00, LDI,  0x01, 0x01,
      0x00, 0x00, 0x00, ADD,  0x02, 0x00, 0x01, BRK,
  };
  TaroReturn tr = taro_new(sizeof(mem));
  Taro t = tr.taro;
  TaroReturnCode rc = taro_load(&t, mem, sizeof(mem));
  ASSERT_EQ(rc, TARO_OK);
  ASSERT_EQ(taro_run(&t), TARO_BRK);
  ASSERT_EQ(t.threads[0].frames[0].stack[0], 1);
  ASSERT_EQ(t.threads[0].frames[0].stack[1], 1);
  ASSERT_EQ(t.threads[0].frames[0].stack[2], 2);
}
