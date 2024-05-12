#include "taro.h"
#if ALLOC_FUNC == malloc
#include <stdlib.h>
#endif

#include <memory.h>

TaroFrame taro_frame_new() {
  TaroFrame f;
  f.pc = 0;
  f.sp = 0;
  return f;
}

typedef struct TaroMemReturn {
  TaroReturnCode rc;
  TaroMem mem;
} TaroMemReturn;

TaroMemReturn taro_mem_new(uint32_t const size) {
  TaroMemReturn r;
  r.mem.mem_size = size;
  r.mem.mem = (uint8_t *)ALLOC_FUNC(size);
  if (r.mem.mem == NULL) {
    r.rc = TARO_ERROR;
  }
  return r;
}

TaroThread taro_thread_new() {
  TaroThread t;
  t.fp = 0;
  return t;
}

TaroReturn taro_new(uint32_t const mem_size) {
  TaroReturn r;
  r.rc = TARO_OK;

  r.taro.threads = ALLOC_FUNC(sizeof(TaroThread));
  if (r.taro.threads == NULL) {
    r.rc = TARO_ERROR;
    return r;
  }
  r.taro.num_threads = 1;
  *(r.taro.threads) = taro_thread_new();

  TaroMemReturn memrc = taro_mem_new(mem_size);
  if (memrc.rc != TARO_OK) {
    r.rc = memrc.rc;
    return r;
  }

  r.taro.mem = memrc.mem;

  return r;
}

TaroReturnCode taro_load(Taro *const taro, uint8_t *mem, uint32_t mem_size) {
  if (mem == NULL || mem_size > taro->mem.mem_size) {
    return TARO_ERROR;
  }

  memcpy(taro->mem.mem, mem, mem_size);

  return TARO_OK;
}
