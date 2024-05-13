#include "taro.h"
#include <stdio.h>
#if ALLOC_FUNC == malloc
#include <stdlib.h>
#endif

#include "opcodes.h"
#include <memory.h>

static void *allocs[TARO_STACK_SIZE] = {0};

static void *taro_alloc(size_t size) {
  void **next_slot = &allocs[0];
  while (*next_slot != NULL && next_slot < (&allocs[TARO_STACK_SIZE])) {
    next_slot++;
  }
  if (next_slot >= &allocs[TARO_STACK_SIZE]) {
    return NULL;
  }
  void *mem = ALLOC_FUNC(size);
  *next_slot = mem;
  return mem;
}

static void taro_dealloc() {
  void **next_slot = &allocs[0];
  while (next_slot != NULL && next_slot < (&allocs[TARO_STACK_SIZE])) {
    DEALLOC_FUNC(*next_slot);
    *next_slot = NULL;
    next_slot++;
  }
}

typedef struct TaroMemReturn {
  TaroReturnCode rc;
  TaroMem mem;
} TaroMemReturn;

TaroMemReturn taro_mem_new(uint32_t const size) {
  TaroMemReturn r;
  r.rc = TARO_OK;
  r.mem.mem_size = size;
  r.mem.mem = (uint8_t *)taro_alloc(size);
  if (r.mem.mem == NULL) {
    r.rc = TARO_ERROR_ALLOC;
    return r;
  }
  memset(r.mem.mem, 0, size);
  return r;
}

TaroThread taro_thread_new() {
  TaroThread t;
  t.fp = 0;
  for (size_t i = 0; i < TARO_STACK_SIZE; i++) {
    t.frames[i].pc = TARO_EXEC_START;
    memset(&t.frames[i].regs[0], 0, TARO_STACK_SIZE * sizeof(uint32_t));
  }
  return t;
}

TaroReturn taro_new(uint32_t const mem_size) {
  TaroReturn r;
  r.rc = TARO_OK;

  r.taro.threads = taro_alloc(sizeof(TaroThread));
  if (r.taro.threads == NULL) {
    r.rc = TARO_ERROR_ALLOC;
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
    return TARO_ERROR_MEM;
  }

  memcpy(taro->mem.mem, mem, mem_size);

  return TARO_OK;
}

void taro_reset(Taro *const taro) {
  taro->threads[0].fp = 0;
  taro->threads[0].frames[0].pc = TARO_EXEC_START;
}

#define CHECK_REGS(rd, r1, r2)                                                 \
  do {                                                                         \
    if (rd > TARO_MAX_REGISTERS || r1 > TARO_MAX_REGISTERS ||                  \
        r2 > TARO_MAX_REGISTERS) {                                             \
      return TARO_ERROR_REG;                                                   \
    }                                                                          \
  } while (0)

#define CHECK_REGS_NO_ZERO(rd, r1, rs)                                         \
  do {                                                                         \
    CHECK_REGS(rd, r1, rs);                                                    \
    if (rd == TARO_ZERO_REG) {                                                 \
      return TARO_ERROR_ZERO_REG_WRITE;                                        \
    }                                                                          \
  } while (0)

#define NEXT_WORD(taro, frame) (*(uint32_t *)(&(taro->mem.mem[frame->pc])))
#define NEXT_OP(taro, frame) ((taro->mem.mem[frame->pc]))
#define OPCODE_RD(taro, frame) ((taro->mem.mem[frame->pc + 1]))
#define OPCODE_R1(taro, frame) ((taro->mem.mem[frame->pc + 2]))
#define OPCODE_R2(taro, frame) ((taro->mem.mem[frame->pc + 3]))
#define CURR_FRAME(thread_num, taro)                                           \
  &taro->threads[thread_num].frames[taro->threads[thread_num].fp]

static TaroReturnCode taro_frame_step(TaroFrame *const frame,
                                      Taro *const taro) {
  TaroOpcode op = NEXT_OP(taro, frame);
  uint32_t rd, r1, r2, offset = 0;
  uint32_t fp = taro->threads[0].fp;
  uint32_t imm = 0;
  TaroFrame *const next_frame = &taro->threads[0].frames[fp + 1];
  switch (op) {
  case ADD:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] + frame->regs[r2];
    frame->pc += 4;
    break;
  case SUB:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] - frame->regs[r2];
    frame->pc += 4;
    break;
  case MUL:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] * frame->regs[r2];
    frame->pc += 4;
    break;
  case DIV:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] / frame->regs[r2];
    frame->pc += 4;
    break;
  case SHL:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] << frame->regs[r2];
    frame->pc += 4;
    break;
  case SHR:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] >> frame->regs[r2];
    frame->pc += 4;
    break;
  case AND:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] & frame->regs[r2];
    frame->pc += 4;
    break;
  case OR:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] | frame->regs[r2];
    frame->pc += 4;
    break;
  case NOT:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, 0);
    frame->regs[rd] = ~frame->regs[r1];
    frame->pc += 3;
    break;
  case XOR:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] ^ frame->regs[r2];
    frame->pc += 4;
    break;
  case MOD:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    frame->regs[rd] = frame->regs[r1] % frame->regs[r2];
    frame->pc += 4;
    break;
  case LD:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R1(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    offset = frame->regs[r1] + frame->regs[r2];
    if (offset >= taro->mem.mem_size) {
      return TARO_ERROR_MEM;
    }
    frame->regs[rd] = taro->mem.mem[offset];
    frame->pc += 4;
    break;
  case LDI:
    rd = OPCODE_RD(taro, frame);
    CHECK_REGS_NO_ZERO(rd, 0, 0);
    frame->pc += 2;
    imm = NEXT_WORD(taro, frame);
    frame->regs[rd] = imm;
    frame->pc += 4;
    break;
  case ST:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, r2);
    offset = frame->regs[r1] + frame->regs[r2];
    if (offset >= taro->mem.mem_size) {
      return TARO_ERROR_MEM;
    }
    taro->mem.mem[offset] = frame->regs[rd];
    frame->pc += 4;
    break;
  case MOV:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, 0);
    frame->regs[rd] = frame->regs[r1];
    frame->pc += 3;
    break;
  case PUSH:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, 0);
    if (fp >= TARO_STACK_SIZE) {
      return TARO_ERROR_FRAME_OVERFLOW;
    }
    next_frame->regs[rd] = frame->regs[r1];
    frame->pc += 3;
    break;
  case PULL:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    CHECK_REGS_NO_ZERO(rd, r1, 0);
    if (fp >= TARO_STACK_SIZE) {
      return TARO_ERROR_FRAME_OVERFLOW;
    }
    frame->regs[rd] = next_frame->regs[r1];
    frame->pc += 3;
    break;
  case JMP:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    CHECK_REGS(rd, r1, 0);
    frame->pc = frame->regs[rd] + frame->regs[r1];
    break;
  case JMPI:
    frame->pc += 1;
    imm = NEXT_WORD(taro, frame);
    frame->pc = imm;
    break;
  case JCN:
    rd = OPCODE_RD(taro, frame);
    r1 = OPCODE_R1(taro, frame);
    r2 = OPCODE_R2(taro, frame);
    CHECK_REGS(rd, r1, r2);
    if (frame->regs[r2] != 0) {
      frame->pc = frame->regs[rd] + frame->regs[r1];
    } else {
      frame->pc += 4;
    }
    break;
  case JCNI:
    rd = OPCODE_RD(taro, frame);
    frame->pc += 2;
    imm = NEXT_WORD(taro, frame);
    CHECK_REGS(rd, 0, 0);
    if (frame->regs[rd] != 0) {
      frame->pc = imm;
    } else {
      frame->pc += 4;
    }
    break;
  case CALL:
    rd = OPCODE_RD(taro, frame);
    CHECK_REGS(rd, 0, 0);
    taro->threads[0].fp += 1;
    if (taro->threads[0].fp >= TARO_STACK_SIZE) {
      return TARO_ERROR_FRAME_OVERFLOW;
    }
    taro->threads[0].frames[taro->threads[0].fp].pc = frame->regs[rd];
    frame->pc += 2;
    break;
  case RET:
    if (taro->threads[0].fp <= 0) {
      return TARO_ERROR_FRAME_UNDERFLOW;
    }
    taro->threads[0].fp -= 1;
    break;
  case BRK:
    return TARO_BRK;
  case INVALID:
  default:
    return TARO_ERROR_BAD_OP;
  }
  return TARO_OK;
}

TaroReturnCode taro_run(Taro *const taro) {
  if (taro == NULL) {
    return TARO_ERROR_NULL;
  }

  while (1) {
    TaroReturnCode rc = taro_frame_step(CURR_FRAME(0, taro), taro);
    if (rc != TARO_OK) {
      return rc;
    }
  }

  return TARO_OK;
}

void taro_free() { taro_dealloc(); }
