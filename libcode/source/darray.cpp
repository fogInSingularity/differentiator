#include "../include/darray.h"

//public-----------------------------------------------------------------------

DArrayError DArray::Ctor(size_t elem_size, size_t init_cap) {
  elem_size_ = elem_size;
  size_ = 0;
  cap_ = (DArrayStandartAllocSize > init_cap) ?
         DArrayStandartAllocSize : init_cap;

  array_ = calloc(cap_, elem_size);
  if (array_ == nullptr) { return DArrayError::kCtorCantAlloc; }

  return DArrayError::kSuccess;
}

DArrayError DArray::Dtor() {
  free(array_);
  array_ = nullptr;

  cap_ = 0;
  size_ = 0;
  elem_size_ = 0;

  return DArrayError::kSuccess;
}

DArrayError DArray::PushBack(void* elem) {
  ASSERT(elem != nullptr);

  DArrayError error = ResizeUp();
  if (error != DArrayError::kSuccess) { return error; }

  memcpy((char*)array_ + size_ * elem_size_, elem, elem_size_);
  size_++;

  return error;
}

DArrayError DArray::PopBack(void* elem) {
  ASSERT(elem != nullptr);

  if (size_ <= 0) { return DArrayError::kEmptyDArrPop; }

  DArrayError error = ResizeDown();
  if (error != DArrayError::kSuccess) { return error; }

  size_--;
  memcpy(elem, (char*)array_ + size_ * elem_size_, elem_size_);

  return error;
}

DArrayError DArray::SetAt(void* elem, Index index) {
  ASSERT(elem != nullptr);

  if (index > size_ - 1) { return DArrayError::kSetAtoutOfBounds; }

  memcpy((char*)array_ + index * elem_size_, elem, elem_size_);

  return DArrayError::kSuccess;
}

DArrayError DArray::GetAt(void* elem, Index index) {
  ASSERT(elem != nullptr);

  if (index > size_ - 1) { return DArrayError::kGetAtOutOfBounds; }

  memcpy(elem, (char*)array_ + index * elem_size_, elem_size_);

  return DArrayError::kSuccess;
}

void* DArray::At(Index index) {
  if (index > size_ - 1) { return nullptr; }

  return (char*)array_ + index * elem_size_;
}

void DArray::Dump(Dump_t* DumpElemFunc, const char* file, size_t line, const char* func) {
  ASSERT(DumpElemFunc != nullptr);
  ASSERT(file != nullptr);
  ASSERT(func != nullptr);

  fprintf(stderr, "#####################################################\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "#  dynamic array called from file %s(%lu) from function %s\n", file, line, func);
  fprintf(stderr, "#  {\n");
  fprintf(stderr, "#    array\n");
  fprintf(stderr, "#    {\n");
  // pirnt array
  for (Index i = 0; i < size_; i++) {
  fprintf(stderr, "#      *[%lu] ", i);
    DumpElemFunc((char*)array_ + i * elem_size_);
    fprintf(stderr, "\n");
  }

  for (Index i = size_; i < cap_; i++) {
  fprintf(stderr, "#       [%lu] ", i);
    DumpElemFunc((char*)array_ + i * elem_size_);
    fprintf(stderr, "\n");
  }
  //
  fprintf(stderr, "#    }\n");
  fprintf(stderr, "#    size %lu\n", size_);
  fprintf(stderr, "#    cap %lu\n", cap_);
  fprintf(stderr, "#    elemSize %lu\n", elem_size_);
  fprintf(stderr, "#  }\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "#####################################################\n");
}

//private----------------------------------------------------------------------

DArrayError DArray::ResizeUp() {
  if (cap_ <= size_) {
    cap_ *= DArrayMultiplyConst;

    return Recalloc();
  }

  return DArrayError::kSuccess;
}

DArrayError DArray::ResizeDown() {
  if (size_ * DArrayMultiplyConst * DArrayMultiplyConst <= cap_) {
    //NOTE - might have bug
    cap_ = (cap_ > DArrayStandartAllocSize)
          ? (cap_ / DArrayMultiplyConst)
          : DArrayStandartAllocSize;

    return Recalloc();
  }

  return DArrayError::kSuccess;
}

DArrayError DArray::Recalloc() {
  void* hold = array_;
  array_ = realloc(array_, cap_ * elem_size_);
  if (array_ == nullptr) {
    array_ = hold;
    return DArrayError::kReclcCantAlloc;
  }

  memset((char*)array_ + size_ * elem_size_, 0, (cap_ - size_) * elem_size_);

  return DArrayError::kSuccess;
}
