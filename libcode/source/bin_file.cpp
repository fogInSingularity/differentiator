#include "../include/bin_file.h"

//static-----------------------------------------------------------------------

static size_t FileSize(FILE* file);

//public-----------------------------------------------------------------------

void BinData::GetData(FILE* file) {
  ASSERT(file != nullptr);

  buf_sz_ = FileSize(file);

  buf_ = (Byte*)calloc(buf_sz_, sizeof(Byte));

  FileRead(file);
}

void BinData::PutData(FILE* file) {
  ASSERT(file != nullptr);

  fwrite(buf_, sizeof(Byte), buf_sz_, file);
}

void BinData::FreeData() {
  free(buf_);
  buf_ = nullptr;

  buf_sz_ = 0;
}

//private----------------------------------------------------------------------

void BinData::FileRead(FILE* file) {
  ASSERT(file != nullptr);

  fread(buf_, sizeof(Byte), buf_sz_, file);
}

//static-----------------------------------------------------------------------

static size_t FileSize(FILE* file) {
  ASSERT(file != nullptr);

  fseek(file, 0, SEEK_END);

  size_t size = (size_t)ftell(file);

  fseek(file, 0, SEEK_SET);

  return size;
}
