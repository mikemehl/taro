#ifndef __TARO_H__
#include <stdint.h>

#define TARO_STACK_SIZE 256
#define ALLOC_FUNC malloc
#define DEALLOC_FUNC free

typedef struct TaroFrame {
  uint16_t sp;
  uint32_t pc;
  uint32_t stack[TARO_STACK_SIZE];
} TaroFrame;

typedef struct TaroMem {
  uint8_t *mem;
  uint32_t mem_size;
} TaroMem;

typedef struct TaroThread {
  TaroFrame frames[TARO_STACK_SIZE];
  uint32_t fp;
} TaroThread;

typedef struct Taro {
  TaroMem mem;
  TaroThread *threads;
  uint32_t num_threads;
} Taro;

typedef enum TaroReturnCode {
  TARO_OK = 0,
  TARO_ERROR = 1,
} TaroReturnCode;

typedef struct TaroReturn {
  TaroReturnCode rc;
  Taro taro;
} TaroReturn;

TaroReturn taro_new(uint32_t const mem_size);
TaroReturnCode taro_load(Taro *const taro, uint8_t *mem, uint32_t mem_size);
#endif /* ifndef __TARO_H__ */
