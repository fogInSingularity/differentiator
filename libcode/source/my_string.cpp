#include "../include/my_string.h"

//public-----------------------------------------------------------------------

StringError String::Ctor(const char* origin, const size_t min_alloc) {
  ASSERT(origin != nullptr);

  size_ = strlen(origin) + 1; // \0 included
  cap_ = MAX(size_ * kStrMultiplier, min_alloc);

  data_ = (char*)calloc(cap_, sizeof(char));
  if (data_ == nullptr) { return StringError::kCantAlloc; }

  memcpy(data_, origin, (size_ - 1) * sizeof(char));

  return StringError::kSuccess;
}

StringError String::Ctor(const size_t len,
                         const char* origin,
                         const size_t min_alloc) {
  ASSERT(origin != nullptr);

  size_ = len + 1;
  cap_ = MAX(size_ * kStrMultiplier, min_alloc);


  data_ = (char*)calloc(cap_, sizeof(char));
  if (data_ == nullptr) { return StringError::kCantAlloc; }

  memcpy(data_, origin, (size_ - 1) * sizeof(char));

  return StringError::kSuccess;
}

void String::Dtor() {
  free(data_);

  data_ = nullptr;
  size_ = 0;
  cap_  = 0;
}

void String::Dump() const {
  fprintf(stderr, "###string dump start##################\n");
  fprintf(stderr, "# data { %p: %s }\n", data_, data_);
  fprintf(stderr, "# size { %lu }\n", size_);
  fprintf(stderr, "# cap { %lu }\n", cap_);
  fprintf(stderr, "###string dump end####################\n");
}

bool String::IsEmpty() const {
  return !((bool)Size());
}

size_t String::Size() const {
  return size_ - 1;
}

size_t String::Length() const {
  return Size();
}

size_t String::Capacity() const {
  return cap_ - 1;
}

StringError String::Resize(size_t new_cap) {
  size_t new_size = (new_cap <= size_ - 1) ? (new_cap + 1) : size_;

  char* hold = data_;
  data_ = (char*)Recalloc(data_,
                          (new_cap + 1) * sizeof(char),
                          new_size * sizeof(char));
  if (data_ == nullptr) {
    data_ = hold;

    return StringError::kCantAlloc;
  }

  data_[new_cap] = '\0';
  size_ = new_size;
  cap_ = new_cap + 1;

  return StringError::kSuccess;
}

StringError String::Reserve(size_t new_cap) {
  if (new_cap <= cap_ - 1) {
    return StringError::kSuccess;
  }

  return Resize(new_cap);
}

StringError String::Clear() {
  memset(data_, 0, cap_ * sizeof(char));
  size_ = 1;
  return Reserve(kStrMinAlloc - 1);
}

StringError String::ShrinkToFit() {
  return Reserve(size_ - 1);
}

char* String::At(Index ind) {
  if (ind >= size_ - 1) { return nullptr; }
  return data_ + ind;
}

const char* String::CStr() const {
  return Data();
}

const char* String::Data() const {
  return data_;
}

StringError String::Append(String* add_str) {
  ASSERT(add_str != nullptr);
  ASSERT(add_str->Data() != nullptr);

  return Append(add_str->Size(), add_str->Data());
}

StringError String::Append(const char* add_str) {
  ASSERT(add_str != nullptr);

  return Append(strlen(add_str), add_str);
}

StringError String::Append(StringView* add_str) {
  ASSERT(add_str != nullptr);

  return Append(add_str->Size(), add_str->Data());
}

StringError String::PushBack(char ch) {
  StringError error = Reserve(size_ + 1);
  if (error != StringError::kSuccess) { return error; }

  data_[size_ - 1] = ch;
  data_[size_] = '\0';

  size_++;

  return StringError::kSuccess;
}

char String::PopBack() {
  size_--;

  char ret = data_[size_ - 1];
  data_[size_ - 1] = '\0';
  return ret;
}

StringError String::Assign(String* str) {
  ASSERT(str != nullptr);
  ASSERT(str->Data() != nullptr);

  Dtor();
  return Ctor(str->Data());
}

StringError String::Assign(const char* str) {
  ASSERT(str != nullptr);

  Dtor();
  return Ctor(str);
}

StringError String::Assign(StringView* str) {
  ASSERT(str != nullptr);

  Dtor();
  return Ctor(str->Data(), str->Length());
}

void StringView::Ctor(const char* origin = "") {
  data_ = origin;
  size_ = strlen(origin) + 1; //with \0
}

void StringView::Ctor(const char* origin, size_t len) {
  data_ = origin;
  size_ = len;
}

void StringView::Dtor() {
  data_ = "";
  size_ = 1;
}

bool StringView::IsEmpty() {
  return !((bool)Size());
}

size_t StringView::Size() {
  return size_ - 1;
}

size_t StringView::Length() {
  return Size();
}

const char* StringView::At(Index ind) {
  if (ind >= size_ - 1) { return nullptr; }
  return data_ + ind;
}

const char* StringView::Data() {
  return data_;
}

// view += size;

//private----------------------------------------------------------------------

StringError String::Append(size_t add_size, const char* add_str) {
  StringError error = Reserve(size_ + add_size);
  if (error != StringError::kSuccess) { return error; }

  memcpy(data_ + size_ - 1, add_str, add_size * sizeof(char));
  data_[size_ + add_size - 1] = '\0';

  size_ += add_size;

  return StringError::kSuccess;
}
