#include "../include/dif.h"

//static-----------------------------------------------------------------------

static void ErrorMessage(const char* msg);
static void ElemDtor(Elem* data);
static bool CopyElem(TreeNode* dest, TreeNode* src);

//public-----------------------------------------------------------------------

DifError Diffirentiator::Ctor(const int argc, const char** argv) {
  ASSERT(argv != nullptr);

  StringError str_error = StringError::kSuccess;
  TreeError tree_error = TreeError::kSuccess;

  if (argc < 3) { return DifError::kNotEnoughFiles; }
  if (argv[1] == nullptr) { return DifError::kBadArgsPassedToDif; }
  if (argv[2] == nullptr) { return DifError::kBadArgsPassedToDif; }

  FILE* expression_file = fopen(argv[1], "r");
  if (expression_file == nullptr) { return DifError::kCantOpenSourceFile; }

  BinData expression_data = {};
  expression_data.GetData(expression_file);
  fclose(expression_file);

  String expression_str = {};
  str_error = expression_str.Ctor(expression_data.buf_sz_, (const char*)expression_data.buf_);
  if (str_error != StringError::kSuccess) { return DifError::kCtorBadString; }
  expression_data.FreeData();

  expression_tree_.Ctor();

  tree_error = expression_tree_.LoadFromStr(&expression_str);
  expression_str.Dtor();
  if (tree_error != TreeError::kSuccess) { return DifError::kInvalidExpression; }

  return DifError::kSuccess;
}

void Diffirentiator::Dtor() {
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
    default:
      ASSERT(0 && "UNKNOWN ERROR CODE");
      break;
  }
}

DifError Diffirentiator::Diffirentiate() {
  return DifNode(expression_tree_.root_);
}

DifError Diffirentiator::DifNode(TreeNode* node) {
  ASSERT(node != nullptr);

  switch (node->data.type) {
    case TypeOfElem::kConstant:
      node->data.value.num = 0;

      return DifError::kSuccess;
      break;
    case TypeOfElem::kVariable:
      node->data.value.var.Dtor();
      node->data.type = TypeOfElem::kConstant;
      node->data.value.num = 1;

      return DifError::kSuccess;
      break;
    case TypeOfElem::kOperator:

      return DifNodeOperator(node);
      break;
    case TypeOfElem::kUninit:

      return DifError::kInvalidDif;
      break;
    default:
      ASSERT(0 && "UNKNOWN TYPE OF NODE");

      return DifError::kInvalidDif;
      break;
  }
}

DifError Diffirentiator::DifNodeOperator(TreeNode* node) {
  ASSERT(node != nullptr);
  ASSERT(node->data.type == TypeOfElem::kOperator);

  DifError error = DifError::kSuccess;

  switch (node->data.value.op) {
    case Operator::kAddition:
    case Operator::kSubtraction:
      error = DifNode(node->l_child);
      if (error != DifError::kSuccess) { return error; }

      error = DifNode(node->r_child);
      if (error != DifError::kSuccess) { return error; }

      return DifError::kSuccess;
      break;
    case Operator::kMultiplication: { //FIXME copypase

      return DifNodeMult(node);
      break;
    }
    case Operator::kDivision:

      return DifNodeDiv(node);
      break;
    case Operator::kPowerFunction:
      //FIXME

      return DifError::kSuccess;
      break;
    case Operator::kUninitOperator:

      return DifError::kInvalidDif;
      break;
    default:
      ASSERT(0 && "UNKNOWN OPERATOR");

      return DifError::kInvalidDif;
      break;
  }
}

DifError Diffirentiator::DifNodeMult(TreeNode* node) {
  Elem data = {};
  DifError error = DifError::kSuccess;

  data.type = TypeOfElem::kOperator;
  data.value.op = Operator::kAddition;
  TreeNode* plus_node = expression_tree_.CtorNode(node->parent, &data);
  if (plus_node == nullptr) { return DifError::kInvalidDif; }
  //---
  data.type = TypeOfElem::kOperator;
  data.value.op = Operator::kMultiplication;
  //---
  TreeNode* l_mult_node = expression_tree_.CtorNode(plus_node, &data);
  if (l_mult_node == nullptr) { return DifError::kInvalidDif; }
  plus_node->l_child = l_mult_node;

  TreeNode* dL_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (dL_node == nullptr) { return DifError::kInvalidDif; }
  error = DifNode(dL_node);
  if (error != DifError::kSuccess) { return error; }
  dL_node->parent = l_mult_node;
  l_mult_node->l_child = dL_node;

  TreeNode* R_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_node == nullptr) { return DifError::kInvalidDif; }
  R_node->parent = l_mult_node;
  l_mult_node->r_child = R_node;
  //---
  TreeNode* r_mult_node = expression_tree_.CtorNode(plus_node, &data);
  if (r_mult_node == nullptr) { return DifError::kInvalidDif; }
  plus_node->r_child = r_mult_node;

  TreeNode* L_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (L_node == nullptr) { return DifError::kInvalidDif; }
  L_node->parent = r_mult_node;
  r_mult_node->l_child = L_node;

  TreeNode* dR_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (dR_node == nullptr) { return DifError::kInvalidDif; }
  error = DifNode(dR_node);
  if (error != DifError::kSuccess) { return DifError::kInvalidDif; }
  dR_node->parent = r_mult_node;
  r_mult_node->r_child = dR_node;

  if (node->parent == nullptr) {
    return DifError::kSuccess;
  }

  if (node->parent->l_child == node) {
    node->parent->l_child = plus_node;
  } else if (node->parent->r_child == node) {
    node->parent->r_child = plus_node;
  } else {
    return DifError::kInvalidDif;
  }

  expression_tree_.DtorNode(node, ElemDtor);

  return DifError::kSuccess;
}

DifError Diffirentiator::DifNodeDiv(TreeNode* node) {
  Elem data = {};
  DifError error = DifError::kSuccess;
  TreeNode* ptr_error = nullptr;

  data.type = TypeOfElem::kOperator;

  data.value.op = Operator::kDivision;
  TreeNode* div_node = expression_tree_.CtorNode(node->parent, &data);
  if (div_node == nullptr) { return DifError::kInvalidDif; }

  data.value.op = Operator::kPowerFunction;
  TreeNode* power_node = expression_tree_.CtorNode(div_node, &data);
  if (power_node == nullptr) { return DifError::kInvalidDif; }
  div_node->r_child = power_node;

  data.type = TypeOfElem::kConstant;
  data.value.num = 2;
  TreeNode* two_node = expression_tree_.CtorNode(power_node, &data);
  if (two_node == nullptr) { return DifError::kInvalidDif; }
  power_node->r_child = two_node;

  TreeNode* R_power_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_power_node == nullptr) { return DifError::kInvalidDif; }
  R_power_node->parent = power_node;
  power_node->l_child = R_power_node;

  data.type = TypeOfElem::kOperator;

  data.value.op = Operator::kSubtraction;
  TreeNode* minus_node = expression_tree_.CtorNode(div_node, &data);
  if (minus_node == nullptr) { return DifError::kInvalidDif; }
  div_node->l_child = minus_node;

  data.value.op = Operator::kMultiplication;
  TreeNode* mult_l_minus_node = expression_tree_.CtorNode(minus_node, &data);
  if (mult_l_minus_node == nullptr) { return DifError::kInvalidDif; }
  minus_node->l_child = mult_l_minus_node;

  ptr_error = CopyAndDifL(node, mult_l_minus_node);
  if (ptr_error == nullptr) { return DifError::kInvalidDif; }

  TreeNode* R_mult_node = expression_tree_.CopySubTree(node->r_child, CopyElem);
  if (R_mult_node == nullptr) { return DifError::kInvalidDif; }
  R_mult_node->parent = mult_l_minus_node;
  mult_l_minus_node->r_child = R_mult_node;

  TreeNode* mult_r_minus_node = expression_tree_.CtorNode(minus_node, &data);
  if (mult_r_minus_node == nullptr) { return DifError::kInvalidDif; }
  minus_node->r_child = mult_r_minus_node;

  ptr_error = CopyAndDifR(node, mult_r_minus_node);
  if (ptr_error == nullptr) { return DifError::kInvalidDif; }

  TreeNode* L_mult_node = expression_tree_.CopySubTree(node->l_child, CopyElem);
  if (L_mult_node == nullptr) { return DifError::kInvalidDif; }
  L_mult_node->parent = mult_r_minus_node;
  mult_r_minus_node->r_child = R_mult_node;

  return DifError::kSuccess;
}

TreeNode* Diffirentiator::CopyAndDifR(TreeNode* node, TreeNode* new_parent) {
  ASSERT(node != nullptr);
  ASSERT(new_parent != nullptr);

  TreeNode* dR_node = CopyAndDif(node->r_child);
  if (dR_node == nullptr) { return nullptr; }

  dR_node->parent = new_parent;
  new_parent->r_child = dR_node;

  return dR_node;
}

TreeNode* Diffirentiator::CopyAndDifL(TreeNode* node, TreeNode* new_parent) {
  ASSERT(node != nullptr);
  ASSERT(new_parent != nullptr);

  TreeNode* dL_node = CopyAndDif(node->l_child);
  if (dL_node == nullptr) { return nullptr; }

  dL_node->parent = new_parent;
  new_parent->l_child = dL_node;

  return dL_node;
}

TreeNode* Diffirentiator::CopyAndDif(TreeNode* node) {
  ASSERT(node != nullptr);

  TreeNode* d_node = expression_tree_.CopySubTree(node, CopyElem);
  if (d_node == nullptr) { return nullptr; }
  DifError error = DifNode(d_node);
  if (error != DifError::kSuccess) { return nullptr; }

  return d_node;
}

//static-----------------------------------------------------------------------

static void ErrorMessage(const char* msg) {
  ASSERT(msg != nullptr);

  fprintf(stderr, RED BOLD "error: " RESET "%s\n", msg);
}

static void ElemDtor(Elem* data) {
  ASSERT(data != nullptr);

  if (data->type == TypeOfElem::kVariable) {
    data->value.var.Dtor();
  }

  data->type = TypeOfElem::kUninit;
}

static bool CopyElem(TreeNode* dest, TreeNode* src) {
  ASSERT(dest != nullptr);
  ASSERT(src != nullptr);

  if (src->data.type != TypeOfElem::kVariable) {
    dest->data = src->data;
  } else {
    StringError str_error = StringError::kSuccess;

    dest->data.type = src->data.type;
    str_error = dest->data.value.var.Ctor(src->data.value.var.Data());
    if (str_error != StringError::kSuccess) { return true; }
  }

  return false;
}
