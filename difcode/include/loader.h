#ifndef LOADER_H_
#define LOADER_H_

#include "../../treecode/include/b_tree.h"
#include "../../treecode/include/tree_config.h"
#include "../../libcode/include/darray.h"

enum class Bracket {
  kUninitBracket = 0,
  kLeftBracket   = 1,
  kRightBracket  = 2,
};

enum class TokenType {
  kUninitType   = 0,
  kBracketType  = 1,
  kOperatorType = 2,
  kOperandType  = 3,
  kVariableType = 4,
};

union TokenValue {
  Bracket bracket;
  Operator op;
  Operand num;
  Variable var;
};

struct Token {
  TokenType type;
  TokenValue value;
};

#endif // LOADER_H_
