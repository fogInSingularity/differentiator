#ifndef LIB_BINFILE_H_
#define LIB_BINFILE_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "lib_config.h"

#include "my_typedefs.h"
#include "my_assert.h"

struct BinData {
 public:
  Byte* buf_;
  size_t buf_sz_;

  void GetData(FILE* file);
  void PutData(FILE* file);
  void FreeData();
 private:
  void FileRead(FILE* file);
};

#endif // LIB_BINFILE_H_
