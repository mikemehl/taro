#ifndef __TARO_H__
#define __TARO_H__
#include <stdint.h>

#define TARO_STACK_SIZE 256
#define TARO_MAX_REGISTERS (TARO_STACK_SIZE)
#define TARO_ZERO_REG TARO_STACK_SIZE - 1
#define TARO_MAX_MEM_SIZE (1 << 24)
#define TARO_EXEC_START 0x00
#define ALLOC_FUNC malloc
#define DEALLOC_FUNC free

typedef struct TaroFrame {
  uint32_t pc;
  uint32_t regs[TARO_STACK_SIZE];
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
  TARO_OK,
  TARO_BRK,
  TARO_ERROR_ALLOC,
  TARO_ERROR_MEM,
  TARO_ERROR_REG,
  TARO_ERROR_ZERO_REG_WRITE,
  TARO_ERROR_BAD_OP,
  TARO_ERROR_NULL,
  TARO_ERROR_FRAME_OVERFLOW,
  TARO_ERROR_FRAME_UNDERFLOW,
  TARO_ERROR,
} TaroReturnCode;

typedef struct TaroReturn {
  TaroReturnCode rc;
  Taro taro;
} TaroReturn;

TaroReturn taro_new(uint32_t const mem_size);
TaroReturnCode taro_load(Taro *const taro, uint8_t *mem, uint32_t mem_size);
TaroReturnCode taro_run(Taro *const taro);
void taro_reset(Taro *const taro);
void taro_free();
#endif /* ifndef __TARO_H__ */
