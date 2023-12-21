#include "../include/loader.h"

//static-----------------------------------------------------------------------

static TreeError GetToken(Token* token, const char** move_str, String* source);
static TreeError GetLongToken(Token* token, const char** move_str, String* source);
static double ParseNum(const char* left_str, const char* right_str);
static const char* strchrs(const char* str, const char* keys);
static const char* SkipSpaces(const char* move);
static TreeError LoadFromDArray(BTree* tree, DArray* token_arr);
static TreeNode* LoadNodeFromDArray(BTree* tree, DArray* token_arr, TreeNode* parent, Index* shift);

//public-----------------------------------------------------------------------

TreeError BTree::LoadFromStr(String* source) {
  ASSERT(source != nullptr);

  TreeError error = TreeError::kSuccess;
  DArrayError darr_error = DArrayError::kSuccess;

  if (!IsValid(source)) { return TreeError::kBadLoad; }

  DArray token_arr = {};
  darr_error = token_arr.Ctor(sizeof(Token));
  if (darr_error != DArrayError::kSuccess) { return TreeError::kBadLoad; }

  Token token = {};
  const char* move_str = source->Data();
  while (move_str < source->Data() + source->Size()) {
    token = {};

    error = GetToken(&token, &move_str, source);
    if (error != TreeError::kSuccess) { return TreeError::kBadLoad; }

    darr_error = token_arr.PushBack(&token);
    if (darr_error != DArrayError::kSuccess) { return TreeError::kBadLoad; }

    move_str = SkipSpaces(move_str);
  }

  error = LoadFromDArray(this, &token_arr);
  if (error != TreeError::kSuccess) {$$( return error; )}

  token_arr.Dtor();

  return TreeError::kSuccess;
}

//static-----------------------------------------------------------------------

static TreeError GetToken(Token* token, const char** move_str, String* source) {
  ASSERT(token != nullptr);
  ASSERT(move_str != nullptr);
  ASSERT(source != nullptr);

  switch (**move_str) {
    case '(':
      token->type = TokenType::kBracketType;
      token->value.bracket = Bracket::kLeftBracket;

      (*move_str)++;

      return TreeError::kSuccess;
    case ')':
      token->type = TokenType::kBracketType;
      token->value.bracket = Bracket::kRightBracket;

      (*move_str)++;

      return TreeError::kSuccess;
    case '+':
      token->type = TokenType::kOperatorType;
      token->value.op = Operator::kAddition;

      (*move_str)++;

      return TreeError::kSuccess;
    case '-':
      token->type = TokenType::kOperatorType;
      token->value.op = Operator::kSubtraction;

      (*move_str)++;

      return TreeError::kSuccess;
    case '*':
      token->type = TokenType::kOperatorType;
      token->value.op = Operator::kMultiplication;

      (*move_str)++;

      return TreeError::kSuccess;
    case '/':
      token->type = TokenType::kOperatorType;
      token->value.op = Operator::kDivision;

      (*move_str)++;

      return TreeError::kSuccess;
    case '^':
      token->type = TokenType::kOperatorType;
      token->value.op = Operator::kPowerFunction;

      (*move_str)++;

      return TreeError::kSuccess;
    default:
      return GetLongToken(token, move_str, source);
  }
}

static TreeError GetLongToken(Token* token, const char** move_str, String* source) {
  ASSERT(token != nullptr);
  ASSERT(move_str != nullptr);
  ASSERT(source != nullptr);

  const char* left_str = *move_str;
  const char* right_str = strchrs(*move_str, "()");

  double num = ParseNum(left_str, right_str);
  if (!isnan(num)) {
    token->type = TokenType::kOperandType;
    token->value.num = num;

    *move_str = right_str;

    return TreeError::kSuccess;
  } else if (strncmp(left_str, "log", (size_t)(right_str - left_str)) == 0) {
    token->type = TokenType::kOperatorType;
    token->value.op = Operator::kLogFunction;

    *move_str = right_str;

    return TreeError::kSuccess;
  } else {
    StringError str_error = StringError::kSuccess;

    token->type = TokenType::kVariableType;
    str_error = token->value.var.Ctor((size_t)(right_str - left_str), left_str);
    if (str_error != StringError::kSuccess) { return TreeError::kBadLoad; }

    *move_str = right_str;

    return TreeError::kSuccess;
  }
}

static double ParseNum(const char* left_str, const char* right_str) {
  ASSERT(left_str != nullptr);
  ASSERT(right_str != nullptr);

  Counter n_donts = 0;
  bool flag = true;

  const char* move_str = left_str;

  if (*move_str == '+' || *move_str == '-') {
    move_str++;
  }

  while (move_str < right_str) {
    if (isdigit(*move_str)) {
      ;
    } else if (*move_str == '.') {
      if (n_donts >= 1) {
        flag = false;
      } else {
        n_donts++;
      }
    } else {
      flag = false;
    }

    move_str++;
  }

  if (flag == false) {
    return NAN;
  } else {
    return atof(left_str);
  }
}

static const char* strchrs(const char* str, const char* keys) {
  ASSERT(str != nullptr);
  ASSERT(keys != nullptr);

  size_t keys_len = strlen(keys);

  while (*str != '\0') {
    for (Index i = 0; i < keys_len; i++) {
      if (*str == keys[i]) { return str; }
    }

    str++;
  }

  return nullptr;
}

static const char* SkipSpaces(const char* move) {
  ASSERT(move != nullptr);

  while (isspace(*move)) { move++; }

  return move;
}

static TreeError LoadFromDArray(BTree* tree, DArray* token_arr) {
  ASSERT(tree != nullptr);
  ASSERT(token_arr != nullptr);

  Index cur_pos = 0;

  Elem data = {.type = TypeOfElem::kUninit};
  tree->root_ = tree->CtorNode(nullptr, &data);
  if (tree->root_ == nullptr) { return TreeError::kBadLoad; }

  tree->root_->l_child = LoadNodeFromDArray(tree, token_arr, tree->root_, &cur_pos);
  if (tree->root_ == nullptr) { return TreeError::kBadLoad; }

  return TreeError::kSuccess;
}

static TreeNode* LoadNodeFromDArray(BTree* tree, DArray* token_arr, TreeNode* parent, Index* shift) {
  ASSERT(tree != nullptr);
  ASSERT(token_arr != nullptr);
  ASSERT(parent != nullptr);
  ASSERT(shift != nullptr);

  Token* darr_elem = (Token*)token_arr->At(*shift);
  if (darr_elem == nullptr) {$$( return nullptr; )}

  if (darr_elem->type != TokenType::kBracketType
      || darr_elem->value.bracket != Bracket::kLeftBracket) {
    $$( return nullptr; )
  }
  (*shift)++;

  Elem data = {};
  TreeNode* new_node = tree->CtorNode(parent, &data);
  if (new_node == nullptr) {$$( return nullptr; )}

  darr_elem = (Token*)token_arr->At(*shift);
  if (darr_elem == nullptr) {$$( return nullptr; )}

  if (darr_elem->type == TokenType::kBracketType
      && darr_elem->value.bracket == Bracket::kLeftBracket) {
    new_node->l_child = LoadNodeFromDArray(tree, token_arr, new_node, shift);
    if (new_node->l_child == nullptr) {$$( return nullptr; )}
  }

  darr_elem = (Token*)token_arr->At(*shift);
  if (darr_elem == nullptr) {$$( return nullptr; )}

  switch (darr_elem->type) {
    case TokenType::kOperatorType:
      new_node->data.type = TypeOfElem::kOperator;
      new_node->data.value.op = darr_elem->value.op;
      break;
    case TokenType::kOperandType:
      new_node->data.type = TypeOfElem::kConstant;
      new_node->data.value.num = darr_elem->value.num;
      break;
    case TokenType::kVariableType:
      new_node->data.type = TypeOfElem::kVariable;
      new_node->data.value.var = darr_elem->value.var;
      break;
    case TokenType::kBracketType:
      break;
    case TokenType::kUninitType:
    default:
      $$( return nullptr; )
  }

  (*shift)++;

  darr_elem = (Token*)token_arr->At(*shift);
  if (darr_elem == nullptr) {$$( return nullptr; )}

  if (darr_elem->type == TokenType::kBracketType
      && darr_elem->value.bracket == Bracket::kLeftBracket) {
    new_node->r_child = LoadNodeFromDArray(tree, token_arr, new_node, shift);
    if (new_node->r_child == nullptr) {$$( return nullptr; )}
  }

  (*shift)++;

  return new_node;
}
