#ifndef DARRAY_H
#define DARRAY_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <memory.h>
#include <stdio.h>

#include "debug.h"
#include "my_assert.h"
#include "my_typedefs.h"

typedef void Dump_t(void* elem);

static const size_t DArrayStandartAllocSize = 8;
static const size_t DArrayMultiplyConst = 2;

enum DArrayError {
  kSuccess            = 0,
  kCtorCantAlloc      = 1,
  kReclcCantAlloc     = 2,
  kEmptyDArrPop       = 3,
  kSetAtoutOfBounds   = 4,
  kGetAtOutOfBounds   = 5,
  kAtOutOfBounds      = 6,
};

struct DArray {
 public:
  void* array_;
  size_t size_;
  size_t cap_;
  size_t elem_size_;

  DArrayError Ctor(size_t elem_size,
                   size_t init_cap = DArrayStandartAllocSize);
  DArrayError Dtor();

  DArrayError PushBack(void* elem);
  DArrayError PopBack(void* elem);
  DArrayError SetAt(void* elem, Index index);
  DArrayError GetAt(void* elem, Index index);
  void* At(Index index);
  void Dump(Dump_t* DumpElemFunc,
            const char* file, size_t line,
            const char* func);
 private:
  DArrayError ResizeUp();
  DArrayError ResizeDown();
  DArrayError Recalloc();
};

#define DARRAY_DUMP(d_arr, FuncRef)                                            \
  d_arr->Dump(FuncRef, __FILE__, __LINE__, __func__);

#endif
