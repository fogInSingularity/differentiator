#include "../include/dif.h"
#include <sys/cdefs.h>

//static-----------------------------------------------------------------------

static void ErrorMessage(const char* msg);
static bool CopyElem(TreeNode* dest, TreeNode* src);
static void DumpNodeTrav(TreeNode* node);

//global-----------------------------------------------------------------------

void ElemDtor(Elem* data) {
  ASSERT(data != nullptr);

  if (data->type == TypeOfElem::kVariable) {
    data->value.var.Dtor();
  }

  data->type = TypeOfElem::kUninit;
}

bool RehangParent(TreeNode* old_child, TreeNode* new_child) {$$$
  ASSERT(old_child != nullptr);
  ASSERT(new_child != nullptr);

  if (old_child->parent->l_child == old_child) {
    old_child->parent->l_child = new_child;
  } else if (old_child->parent->r_child == old_child) {
    old_child->parent->r_child = new_child;
  } else {
    $$( return true; )
  }

  // false is ok
  $$( return false; )
}

//public-----------------------------------------------------------------------

#define INVALID_DIF_RET $$( return DifError::kInvalidDif; )

DifError Diffirentiator::Ctor(const int argc, const char** argv) {$$$
  ASSERT(argv != nullptr);

  StringError str_error = StringError::kSuccess;
  TreeError tree_error = TreeError::kSuccess;

  if (argc < 3) {$$( return DifError::kNotEnoughFiles; )}
  if (argv[1] == nullptr) {$$( return DifError::kBadArgsPassedToDif; )}
  if (argv[2] == nullptr) {$$( return DifError::kBadArgsPassedToDif; )}

  FILE* expression_file = fopen(argv[1], "r");
  if (expression_file == nullptr) {$$( return DifError::kCantOpenSourceFile; )}
  out_file_name_ = argv[2];

  BinData expression_data = {};
  expression_data.GetData(expression_file);
  fclose(expression_file);

  String expression_str = {};
  str_error = expression_str.Ctor(expression_data.buf_sz_, (const char*)expression_data.buf_);
  expression_data.FreeData();
  if (str_error != StringError::kSuccess) {$$( return DifError::kCtorBadString; )}

  expression_tree_.Ctor();

  tree_error = expression_tree_.LoadFromStr(&expression_str);
  expression_str.Dtor();
  if (tree_error != TreeError::kSuccess) { INVALID_DIF_RET }

  $$( return DifError::kSuccess; )
}

void Diffirentiator::Dtor() {
  TreeError tree_error = TreeError::kSuccess;

  String store_str = {};
  store_str.Ctor();

  tree_error = expression_tree_.LoadToStr(&store_str);
  if (tree_error != TreeError::kSuccess) { $ }

  FILE* out_file = fopen(out_file_name_, "w");
  if (out_file == nullptr) { $ }

  fwrite(store_str.Data(), sizeof(char), store_str.Size(), out_file);
  store_str.Dtor();

  out_file_name_ = nullptr;
  expression_tree_.Dtor(ElemDtor);
}

void Diffirentiator::ThrowError(DifError error) {
  switch (error) {
    case DifError::kSuccess:
      //---//
      break;
    case DifError::kNotEnoughFiles:
      ErrorMessage("not enough files");
      break;
    case DifError::kCantOpenSourceFile:
      ErrorMessage("cant open source file");
      break;
    case DifError::kCantOpenDifFile:
      ErrorMessage("cant open dif file");
      break;
    case DifError::kBadArgsPassedToDif:
      ErrorMessage("invalid args passed to programm");
      break;
    case DifError::kCtorBadString:
      ErrorMessage("bad alloc for string");
      break;
    case DifError::kInvalidExpression:
      ErrorMessage("passed invalid expression");
      break;
    case DifError::kInvalidDif:
      ErrorMessage("invalid diffirentiation");
      break;
    case DifError::kInvalidSimpl:
      ErrorMessage("invalid simplification");
      break;
    default:
      ASSERT(0 && "UNKNOWN ERROR CODE");
      break;
  }
}

DifError Diffirentiator::Diffirentiate() {$$$
  $$( return DifNode(expression_tree_.root_->l_child); )
}

DifError Diffirentiator::DifNode(TreeNode* node) {$$$
  ASSERT(node != nullptr);

  switch (node->data.type) {
    case TypeOfElem::kConstant:
      node->data.value.num = 0;

      $$( return DifError::kSuccess; )
    case TypeOfElem::kVariable:
      node->data.value.var.Dtor();
      node->data.type = TypeOfElem::kConstant;
      node->data.value.num = 1;

      $$( return DifError::kSuccess; )
    case TypeOfElem::kOperator:

      $$( return DifNodeOperator(node); )
    case TypeOfElem::kUninit:

      INVALID_DIF_RET
    default:
      ASSERT(0 && "UNKNOWN TYPE OF NODE");

      INVALID_DIF_RET
  }
}

DifError Diffirentiator::DifNodeOperator(TreeNode* node) {$$$
  ASSERT(node != nullptr);
  ASSERT(node->data.type == TypeOfElem::kOperator);

  DifError error = DifError::kSuccess;

  switch (node->data.value.op) {
    case Operator::kAddition:
    case Operator::kSubtraction:
      error = DifNode(node->l_child);
      if (error != DifError::kSuccess) {$$( return error; )}

      error = DifNode(node->r_child);
      if (error != DifError::kSuccess) {$$( return error; )}

      $$( return DifError::kSuccess; )
    case Operator::kMultiplication:

      $$( return DifNodeMult(node); )
    case Operator::kDivision:

      $$( return DifNodeDiv(node); )
    case Operator::kPowerFunction:

      $$( return DifNodePower(node); )
    case Operator::kLogFunction:

      $$( return DifNodeLog(node); )
    case Operator::kLnFunction:

      $$( return DifNodeLn(node); )
    case Operator::kSinFuntion:

      $$( return DifNodeSin(node); )
    case Operator::kCosFunction:

      $$( return DifNodeCos(node); )
    case Operator::kUninitOperator:

      INVALID_DIF_RET
    default:
      ASSERT(0 && "UNKNOWN OPERATOR");

      INVALID_DIF_RET
  }
}

DifError Diffirentiator::DifNodeMult(TreeNode* node) {$$$
  ASSERT(node != nullptr);

  Elem data = {};
  DifError error = DifError::kSuccess;

  data.type = TypeOfElem::kOperator;
  data.value.op = Operator::kAddition;
  TreeNode* plus_node = expression_tree_.CtorNode(node->parent, &data);
  if (plus_node == nullptr) { INVALID_DIF_RET }
  //---
  data.type = TypeOfElem::kOperator;
  data.value.op = Operator::kMultiplication;
  //---
  TreeNode* l_mult_node = expression_tree_.CtorNode(plus_node, &data);
  if (l_mult_node == nullptr) { INVALID_DIF_RET }
  plus_node->l_child = l_mult_node;

  TreeNode* dL_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (dL_node == nullptr) { INVALID_DIF_RET }
  dL_node->parent = l_mult_node;
  l_mult_node->l_child = dL_node;
  error = DifNode(dL_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  TreeNode* R_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_node == nullptr) { INVALID_DIF_RET }
  R_node->parent = l_mult_node;
  l_mult_node->r_child = R_node;
  //---
  TreeNode* r_mult_node = expression_tree_.CtorNode(plus_node, &data);
  if (r_mult_node == nullptr) { INVALID_DIF_RET }
  plus_node->r_child = r_mult_node;

  TreeNode* L_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (L_node == nullptr) { INVALID_DIF_RET }
  L_node->parent = r_mult_node;
  r_mult_node->l_child = L_node;

  TreeNode* dR_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (dR_node == nullptr) { INVALID_DIF_RET }
  dR_node->parent = r_mult_node;
  r_mult_node->r_child = dR_node;
  error = DifNode(dR_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  RehangParent(node, plus_node);

  expression_tree_.DtorNode(node, ElemDtor);

  $$( return DifError::kSuccess; )
}

DifError Diffirentiator::DifNodeDiv(TreeNode* node) {$$$
  ASSERT(node != nullptr);

  Elem data = {};
  DifError error = DifError::kSuccess;

  data.type = TypeOfElem::kOperator;

  data.value.op = Operator::kDivision;
  TreeNode* div_node = expression_tree_.CtorNode(node->parent, &data);
  if (div_node == nullptr) { INVALID_DIF_RET }

  data.value.op = Operator::kPowerFunction;
  TreeNode* power_node = expression_tree_.CtorNode(div_node, &data);
  if (power_node == nullptr) { INVALID_DIF_RET }
  div_node->r_child = power_node;

  data.type = TypeOfElem::kConstant;
  data.value.num = 2;
  TreeNode* two_node = expression_tree_.CtorNode(power_node, &data);
  if (two_node == nullptr) { INVALID_DIF_RET }
  power_node->r_child = two_node;

  TreeNode* R_power_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_power_node == nullptr) { INVALID_DIF_RET }
  R_power_node->parent = power_node;
  power_node->l_child = R_power_node;

  data.type = TypeOfElem::kOperator;

  data.value.op = Operator::kSubtraction;
  TreeNode* minus_node = expression_tree_.CtorNode(div_node, &data);
  if (minus_node == nullptr) { INVALID_DIF_RET }
  div_node->l_child = minus_node;

  data.value.op = Operator::kMultiplication;
  TreeNode* mult_l_minus_node = expression_tree_.CtorNode(minus_node, &data);
  if (mult_l_minus_node == nullptr) { INVALID_DIF_RET }
  minus_node->l_child = mult_l_minus_node;

  TreeNode* dL_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (dL_node == nullptr) { INVALID_DIF_RET }
  dL_node->parent = mult_l_minus_node;
  mult_l_minus_node->l_child = dL_node;
  error = DifNode(dL_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  TreeNode* R_mult_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_mult_node == nullptr) { INVALID_DIF_RET }
  R_mult_node->parent = mult_l_minus_node;
  mult_l_minus_node->r_child = R_mult_node;

  TreeNode* mult_r_minus_node = expression_tree_.CtorNode(minus_node, &data);
  if (mult_r_minus_node == nullptr) { INVALID_DIF_RET }
  minus_node->r_child = mult_r_minus_node;

  TreeNode* dR_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (dR_node == nullptr) { INVALID_DIF_RET }
  dR_node->parent =   mult_r_minus_node;
  mult_r_minus_node->r_child = dR_node;
  error = DifNode(dR_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  TreeNode* L_mult_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (L_mult_node == nullptr) { INVALID_DIF_RET }
  L_mult_node->parent = mult_r_minus_node;
  mult_r_minus_node->l_child = L_mult_node;

  RehangParent(node, div_node);

  expression_tree_.DtorNode(node, ElemDtor);

  $$( return DifError::kSuccess; )
}

DifError Diffirentiator::DifNodePower(TreeNode* node) {$$$
  ASSERT(node != nullptr);

  DifError error = DifError::kSuccess;
  Elem data = {};
  data.type = TypeOfElem::kOperator;

  data.value.op = Operator::kMultiplication;
  TreeNode* main_mult_node = expression_tree_.CtorNode(node->parent, &data);
  if (main_mult_node == nullptr) { INVALID_DIF_RET }

  data.value.op = Operator::kPowerFunction;
  TreeNode* power_node = expression_tree_.CtorNode(main_mult_node, &data);
  if (power_node == nullptr) { INVALID_DIF_RET }
  main_mult_node->l_child = power_node;

  TreeNode* L_power_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (L_power_node == nullptr) { INVALID_DIF_RET }
  L_power_node->parent = power_node;
  power_node->l_child = L_power_node;

  TreeNode* R_power_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_power_node == nullptr) { INVALID_DIF_RET }
  R_power_node->parent = power_node;
  power_node->r_child = R_power_node;

  data.value.op = Operator::kAddition;
  TreeNode* plus_node = expression_tree_.CtorNode(main_mult_node, &data);
  if (plus_node == nullptr) { INVALID_DIF_RET }
  main_mult_node->r_child = plus_node;

  data.value.op = Operator::kMultiplication;
  TreeNode* mult_l_node = expression_tree_.CtorNode(plus_node, &data);
  if (mult_l_node == nullptr) { INVALID_DIF_RET }
  plus_node->l_child = mult_l_node;

  data.value.op = Operator::kDivision;
  TreeNode* div_node = expression_tree_.CtorNode(mult_l_node, &data);
  if (div_node == nullptr) { INVALID_DIF_RET }
  mult_l_node->l_child = div_node;

  //left child should be R subtree!!!
  TreeNode* R_div_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_div_node == nullptr) { INVALID_DIF_RET }
  R_div_node->parent = div_node;
  div_node->l_child = R_div_node;

  //right child should be L subtree!!!
  TreeNode* L_div_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (L_div_node == nullptr) { INVALID_DIF_RET }
  L_div_node->parent = div_node;
  div_node->r_child = L_div_node;

  TreeNode* dL_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (dL_node == nullptr) { INVALID_DIF_RET }
  dL_node->parent = mult_l_node;
  mult_l_node->r_child = dL_node;
  error = DifNode(dL_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  data.value.op = Operator::kMultiplication;
  TreeNode* mult_r_node = expression_tree_.CtorNode(plus_node, &data);
  if (mult_r_node == nullptr) { INVALID_DIF_RET }
  plus_node->r_child = mult_r_node;

  data.value.op = Operator::kLnFunction;
  TreeNode* ln_node = expression_tree_.CtorNode(mult_r_node, &data);
  if (ln_node == nullptr) { INVALID_DIF_RET }
  mult_r_node->l_child = ln_node;

  TreeNode* L_log_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (L_log_node == nullptr) { INVALID_DIF_RET }
  L_log_node->parent = ln_node;
  ln_node->r_child = L_log_node;

  TreeNode* dR_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (dR_node == nullptr) { INVALID_DIF_RET }
  dR_node->parent = mult_r_node;
  mult_r_node->r_child = dR_node;
  error = DifNode(dR_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  RehangParent(node, main_mult_node);

  expression_tree_.DtorNode(node, ElemDtor);

  $$( return DifError::kSuccess; )
}

DifError Diffirentiator::DifNodeLog(TreeNode* node) {$$$
  ASSERT(node != nullptr);

  DifError error = DifError::kSuccess;
  Elem data = {};

  data.type = TypeOfElem::kOperator;
  data.value.op = Operator::kDivision;
  TreeNode* div_node = expression_tree_.CtorNode(node->parent, &data);
  if (div_node == nullptr) { INVALID_DIF_RET }

  // R on the left !!!
  data.value.op = Operator::kLnFunction;
  TreeNode* ln_R_node = expression_tree_.CtorNode(div_node, &data);
  if (ln_R_node == nullptr) { INVALID_DIF_RET }
  div_node->l_child = ln_R_node;

  TreeNode* R_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_node == nullptr) { INVALID_DIF_RET }
  R_node->parent = ln_R_node;
  ln_R_node->r_child = R_node;

  // L on the right !!!
  TreeNode* ln_L_node = expression_tree_.CtorNode(div_node, &data);
  if (ln_L_node == nullptr) { INVALID_DIF_RET }
  div_node->r_child = ln_L_node;

  TreeNode* L_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (L_node == nullptr) { INVALID_DIF_RET }
  L_node->parent = ln_L_node;
  ln_L_node->r_child = L_node;

  RehangParent(node, div_node);

  expression_tree_.DtorNode(node, ElemDtor);

  error = DifNode(div_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  $$( return DifError::kSuccess; )
}

DifError Diffirentiator::DifNodeLn(TreeNode* node) {$$$
  ASSERT(node != nullptr);

  Elem data = {};
  DifError error = DifError::kSuccess;

  data.type = TypeOfElem::kOperator;
  data.value.op = Operator::kDivision;
  TreeNode* div_node = expression_tree_.CtorNode(node->parent, &data);
  if (div_node == nullptr) { INVALID_DIF_RET }

  TreeNode* dR_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (dR_node == nullptr) { INVALID_DIF_RET }
  dR_node->parent = div_node;
  div_node->l_child = dR_node;
  error = DifNode(dR_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  TreeNode* R_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_node == nullptr) { INVALID_DIF_RET }
  R_node->parent = div_node;
  div_node->r_child = R_node;

  RehangParent(node, div_node);

  expression_tree_.DtorNode(node, ElemDtor);

  $$( return DifError::kSuccess; )
}

DifError Diffirentiator::DifNodeSin(TreeNode* node) {$$$
  ASSERT(node != nullptr);

  Elem data = {};
  DifError error = DifError::kSuccess;

  data.type = TypeOfElem::kOperator;
  data.value.op = Operator::kMultiplication;
  TreeNode* mult_node = expression_tree_.CtorNode(node->parent, &data);
  if (mult_node == nullptr) { INVALID_DIF_RET }

  data.value.op = Operator::kCosFunction;
  TreeNode* cos_node = expression_tree_.CtorNode(mult_node, &data);
  if (cos_node == nullptr) { INVALID_DIF_RET }
  mult_node->l_child = cos_node;

  TreeNode* R_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_node ==  nullptr) { INVALID_DIF_RET }
  R_node->parent = cos_node;
  cos_node->r_child = R_node;

  TreeNode* dR_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (dR_node == nullptr) { INVALID_DIF_RET }
  dR_node->parent = mult_node;
  mult_node->r_child = dR_node;
  error = DifNode(dR_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  RehangParent(node, mult_node);

  expression_tree_.DtorNode(node, ElemDtor);

  $$( return DifError::kSuccess; )
}

DifError Diffirentiator::DifNodeCos(TreeNode* node) {$$$
  ASSERT(node != nullptr);

  Elem data = {};
  DifError error = DifError::kSuccess;

  data.type = TypeOfElem::kOperator;
  data.value.op = Operator::kMultiplication;
  TreeNode* mult_node = expression_tree_.CtorNode(node->parent, &data);
  if (mult_node == nullptr) { INVALID_DIF_RET }

  data.value.op = Operator::kSinFuntion;
  TreeNode* sin_node = expression_tree_.CtorNode(mult_node, &data);
  if (sin_node == nullptr) { INVALID_DIF_RET }
  mult_node->l_child = sin_node;

  TreeNode* R_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_node ==  nullptr) { INVALID_DIF_RET }
  R_node->parent = sin_node;
  sin_node->r_child = R_node;

  data.value.op = Operator::kMultiplication;
  TreeNode* mult_r_node = expression_tree_.CtorNode(mult_node, &data);
  if (mult_r_node == nullptr) { INVALID_DIF_RET }
  mult_node->r_child = mult_r_node;

  data.type = TypeOfElem::kConstant;
  data.value.num = -1.0;
  TreeNode* minus_one_node = expression_tree_.CtorNode(mult_r_node, &data);
  if (minus_one_node == nullptr) { INVALID_DIF_RET }
  mult_r_node->l_child = minus_one_node;

  TreeNode* dR_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (dR_node == nullptr) { INVALID_DIF_RET }
  dR_node->parent = mult_r_node;
  mult_r_node->r_child = dR_node;
  error = DifNode(dR_node);
  if (error != DifError::kSuccess) { INVALID_DIF_RET }

  RehangParent(node, mult_node);

  expression_tree_.DtorNode(node, ElemDtor);

  $$( return DifError::kSuccess; )
}

//static-----------------------------------------------------------------------

static void ErrorMessage(const char* msg) {
  ASSERT(msg != nullptr);

  fprintf(stderr, RED BOLD "error: " RESET "%s\n", msg);
}

static bool CopyElem(TreeNode* dest, TreeNode* src) {$$$
  ASSERT(dest != nullptr);
  ASSERT(src != nullptr);

  if (src->data.type != TypeOfElem::kVariable) {
    dest->data = src->data;
  } else {
    StringError str_error = StringError::kSuccess;

    dest->data.type = src->data.type;
    str_error = dest->data.value.var.Ctor(src->data.value.var.Data());
    if (str_error != StringError::kSuccess) {$$( return true; )}
  }

  $$( return false; )
}

__attribute__((unused))
static void DumpNodeTrav(TreeNode* node) {
  if (node == nullptr) {return;}
  fprintf(stderr, RED BOLD);
  fprintf(stderr, "    [ node address: %p ][ left: %10p ][ right: %10p ][ parent: %10p ][ data.type: %d ]",
          node, node->l_child, node->r_child, node->parent, (int)node->data.type);

  switch (node->data.type) {
    case TypeOfElem::kConstant:
      fprintf(stderr, "[ data.value.num %lf ]\n", node->data.value.num);
      break;
    case TypeOfElem::kOperator:
      fprintf(stderr, "[ data.value.op %d ]\n", (int)node->data.value.op);
      break;
    case TypeOfElem::kVariable:
      fprintf(stderr, "[ data.value.var %s ]\n", node->data.value.var.Data());
      break;
    case TypeOfElem::kUninit:
      fprintf(stderr, "[ uninit data ]\n");
      break;
    default:
      ASSERT(0 && "UNKNOWN ELEM TYPE");
      break;
  }
  fprintf(stderr, RESET);
}
