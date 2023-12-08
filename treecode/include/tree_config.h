#ifndef TREE_CONFIG_H_
#define TREE_CONFIG_H_

#include "../../libcode/include/my_string.h"

enum class Operator {
  kUninitOperator = 0,
  kAddition       = 1,
  kSubtraction    = 2,
  kMultiplication = 3,
  kDivision       = 4,
  kPowerFunction  = 5,
  // logarithm?
  // trig funtions?
};

typedef int Operand;

typedef String Variable; //REVIEW

union Value {
  Operator op;
  Operand num;
  Variable var;
};

enum class TypeOfElem {
  kUninit   = 0,
  kOperator = 1,
  kConstant = 2,
  kVariable = 3,
};

struct Elem {
  TypeOfElem type;
  Value value;
};

#endif // TREE_CONFIG_H_
