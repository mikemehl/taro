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
