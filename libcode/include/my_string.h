#ifndef LIB_MYSTRING_H_
#define LIB_MYSTRING_H_

#include <ctype.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>
#include <string>

#include "lib_config.h"

#include "my_assert.h"
#include "my_typedefs.h"
#include "recalloc.h"
#include "utils.h"
#include "debug.h"

//lib_config-----------------------------------------------------------------------

static const size_t kStrMinAlloc = 16ul;
static const size_t kStrMultiplier = 2;
// static const size_t kStrMemRequest = 3;
// static const size_t kStrMaxSize    = 4096;

//-----------------------------------------------------------------------------

enum class StringError {
  kSuccess   = 1,
  kCantAlloc = 2,
};

struct StringView {
 public:
  void Ctor(const char* origin);
  void Ctor(const char* origin, size_t len);
  void Dtor();

  bool IsEmpty();
  size_t Size();
  size_t Length();

  const char* At(Index ind);
  const char* Data();
 private:
  const char* data_;
  size_t size_;
};

struct String {
 public:
  StringError Ctor(const char* origin = "",
                   const size_t min_alloc = kStrMinAlloc);
  StringError Ctor(const size_t len,
                   const char* origin = "",
                   const size_t min_alloc = kStrMinAlloc);
  void Dtor();
  void Dump() const;

  bool IsEmpty() const;
  size_t Size() const;
  size_t Length() const;
  size_t Capacity() const;

  StringError Reserve(size_t new_cap);
  StringError Resize(size_t new_cap);
  StringError ShrinkToFit();
  StringError Clear();

  char* At(Index ind);
  const char* CStr() const;
  const char* Data() const;

  StringError Append(String* add_str);
  StringError Append(const char* add_str);
  StringError Append(StringView* add_str);
  //TODO add other Append overloads
  StringError PushBack(char ch);
  char PopBack();
  StringError Assign(String* str);
  StringError Assign(const char* str);
  StringError Assign(StringView* str);
  //TODO add other Assign overloads
  //TODO Insert
  //TODO Erase
  //TODO Swap
 private:
  char* data_;
  size_t size_;
  size_t cap_;

  StringError Append(size_t add_size, const char* add_str);
};

#endif // LIB_MYSTRING_H_
