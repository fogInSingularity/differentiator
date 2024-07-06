#include "../include/b_tree.h"

//static-----------------------------------------------------------------------

static const char* kDotName = "dotdump.dot";

//public-----------------------------------------------------------------------

void BTree::Ctor() {
  InsertCond_ = nullptr;
  FindCond_ = nullptr;
  Action_ = nullptr;

  root_ = nullptr;
}

void BTree::Dtor(ElemDtorFunc* ElemDtor) {
  ASSERT(ElemDtor != nullptr);

  DtorNode(root_, ElemDtor);
  root_ = nullptr;

  Action_ = nullptr;
  FindCond_ = nullptr;
  InsertCond_ = nullptr;
}

TreeError BTree::DotDump() { //FIXME dump func
  FILE* dot_file = fopen(kDotName, "w");
  if (dot_file == nullptr) { return TreeError::kBadFileAccess; }

  fprintf(dot_file, "digraph {\n");
  fprintf(dot_file, "rankdir=LR\n");
  NodeDotDump(dot_file, root_);
  fprintf(dot_file, "}\n");

  fclose(dot_file);
  return TreeError::kSuccess;
}

TreeNode* BTree::CtorNode(TreeNode* parent, Elem* elem) {
  ASSERT(elem != nullptr);
  // parent node can be nullptr

  TreeNode* new_node = (TreeNode*)calloc(1, sizeof(TreeNode));
  if (new_node == nullptr) { return nullptr; }

  new_node->data = *elem;
  new_node->l_child = nullptr;
  new_node->r_child = nullptr;
  new_node->parent = parent;

  return new_node;
}

void BTree::DtorNode(TreeNode* node, ElemDtorFunc* ElemDtor) {
  ASSERT(ElemDtor != nullptr);

  if (node == nullptr) { return; }

  DtorNode(node->l_child, ElemDtor);
  node->l_child = nullptr;
  DtorNode(node->r_child, ElemDtor);
  node->r_child = nullptr;

  ElemDtor(&node->data);
  free(node);
  node = nullptr;
}

TreeError BTree::Insert(Elem* elem, InsertCondFunc* Insrt) {
  ASSERT(elem != nullptr);
  ASSERT(Insrt != nullptr);

  InsertCond_ = Insrt;

  if (root_ == nullptr) {
    root_ = CtorNode(root_, elem);
    if (root_ == nullptr) { return TreeError::kNodeCtorBadAlloc; }
  } else {
    return InsertNode(root_, elem);
  }

  InsertCond_ = nullptr;

  return TreeError::kSuccess;
}

const TreeNode* BTree::Find(Elem* item, FindCondFunc* Fnd) {
  ASSERT(item != nullptr);
  ASSERT(Fnd != nullptr);

  FindCond_ = Fnd;

  const TreeNode* node = FindNode(root_, item);

  FindCond_ = nullptr;

  return node;
}

TreeError BTree::Traversal(ActionFunc* Act) {
  ASSERT(Act != nullptr);

  Action_ = Act;

  TreeError tree_error = TreeError::kSuccess;

  tree_error = NodeTraversal(root_);
  if (tree_error != TreeError::kSuccess) { return tree_error; }

  Action_ = nullptr;

  return TreeError::kSuccess;
}

TreeNode* BTree::CopySubTree(TreeNode* node, CopyElemFunc* CopyFunc) {
  ASSERT(node != nullptr);
  ASSERT(CopyFunc != nullptr);
  ASSERT(CopyFunc_ == nullptr);

  CopyFunc_ = CopyFunc;

  TreeNode* new_node = {};

  new_node = CopySubTreeNode(node);
  CopyFunc_ = nullptr;

  return new_node;
}

bool BTree::IsRoot(const TreeNode* node) {
  ASSERT(node != nullptr);

  return (node == root_);
}

TreeError BTree::LoadToStr(String* str) {
  ASSERT(str != nullptr);

  LoadNodeToStr(str, root_->l_child);

  return TreeError::kSuccess;
}

//private----------------------------------------------------------------------

void BTree::NodeDotDump(FILE* file, TreeNode* node) { //FIXME dump func
  ASSERT(file != nullptr);

  if (node == nullptr) { return; }

  if (node->parent != nullptr) {
    fprintf(file, "node%lu->node%lu\n",
            (size_t)node->parent, (size_t)node);
  }

  NodeDotDump(file, node->l_child);
  NodeDotDump(file, node->r_child);
}

TreeError BTree::InsertNode(TreeNode* node, Elem* elem) {
  ASSERT(node != nullptr);
  ASSERT(elem != nullptr);
  ASSERT(InsertCond_ != nullptr);

  InsertRes ins_res = InsertCond_(node, elem);

  switch (ins_res) {
    case (InsertRes::kError):
      return TreeError::kBadInsertion;
      break;
    case (InsertRes::kLeftNode):
      if (node->l_child != nullptr) {
        return InsertNode(node->l_child, elem);
      }

      node->l_child = CtorNode(node->l_child, elem);
      if (node->l_child == nullptr) { return TreeError::kNodeCtorBadAlloc; }
      break;
    case (InsertRes::kRightNode):
      if (node->r_child != nullptr) {
        return InsertNode(node->r_child, elem);
      }

      node->r_child = CtorNode(node->r_child, elem);
      if (node->r_child == nullptr) { return TreeError::kNodeCtorBadAlloc; }
      break;
    case (InsertRes::kStopInsertion):
      return TreeError::kSuccess;
      break;
    default:
      ASSERT(0 && "UNKNOWN INSERT RESULT");
      break;
  }

  return TreeError::kSuccess;
}

const TreeNode* BTree::FindNode(TreeNode* node, Elem* item) {
  ASSERT(item != nullptr);
  ASSERT(FindCond_ != nullptr);

  if (node == nullptr) { return nullptr; }

  if (FindCond_(&node->data, item) == 0) { return node; }

  const TreeNode* left_search = FindNode(node->l_child, item);
  if ( left_search != nullptr) { return left_search; }
  const TreeNode* right_search = FindNode(node->r_child, item);
  if ( right_search != nullptr) { return right_search; }

  return nullptr;
}

TreeError BTree::NodeTraversal(TreeNode* node) {
  ASSERT(Action_ != nullptr);

  Action_(node);
  if (node == nullptr) { return TreeError::kSuccess; }

  NodeTraversal(node->l_child);
  NodeTraversal(node->r_child);

  return TreeError::kSuccess;
}

TreeNode* BTree::CopySubTreeNode(TreeNode* src_node) {
  ASSERT(src_node != nullptr);

  Elem data = {};
  TreeNode* copy_node = CtorNode(nullptr, &data);
  if (copy_node == nullptr) { return nullptr; }

  bool have_error = CopyFunc_(copy_node, src_node);
  if (have_error) { return nullptr; }

  if (src_node->l_child != nullptr) {
    copy_node->l_child = CopySubTreeNode(src_node->l_child);
    if (copy_node->l_child == nullptr) { return nullptr; }
    copy_node->l_child->parent = copy_node;
  }
  if (src_node->r_child != nullptr) {
    copy_node->r_child = CopySubTreeNode(src_node->r_child);
    if (copy_node->r_child == nullptr) { return nullptr; }
    copy_node->r_child->parent = copy_node;
  }

  return copy_node;
}

TreeError BTree::LoadNodeToStr(String* str, TreeNode* node) {
  ASSERT(str != nullptr);
  // node can be nullptr

  if (node == nullptr) {
    return TreeError::kSuccess;
  }

  switch (node->data.type) {
    case TypeOfElem::kConstant:
      PushConst(str, node);
      break;
    case TypeOfElem::kVariable:
      str->PushBack(' ');
      str->Append(&node->data.value.var);
      str->PushBack(' ');
      break;
    case TypeOfElem::kOperator:
      PushOperator(str, node);
      break;
    case TypeOfElem::kUninit:
      break;
    default:
      ASSERT(0 && "UNKNOWN TYPE");
      break;
  }

  return TreeError::kSuccess;
}

TreeError BTree::PushOperator(String* str, TreeNode* node) {
  ASSERT(str != nullptr);
  ASSERT(node != nullptr);

  if (node->data.type != TypeOfElem::kOperator) {$$( return TreeError::kBadLoad; )}

  switch (node->data.value.op) {
    case Operator::kAddition:
      str->Append("{");
      LoadNodeToStr(str, node->l_child);
      str->Append("}+{");
      LoadNodeToStr(str, node->r_child);
      str->Append("}");
      break;
    case Operator::kSubtraction:
      str->Append("{");
      LoadNodeToStr(str, node->l_child);
      str->Append("}-{");
      LoadNodeToStr(str, node->r_child);
      str->Append("}");
      break;
    case Operator::kMultiplication:
      str->Append("{");
      LoadNodeToStr(str, node->l_child);
      str->Append("}\\cdot{");
      LoadNodeToStr(str, node->r_child);
      str->Append("}");
      break;
    case Operator::kDivision://FIXME
      str->Append("\\frac{");
      LoadNodeToStr(str, node->l_child);
      str->Append("}{");
      LoadNodeToStr(str, node->r_child);
      str->Append("}");
      break;
    case Operator::kPowerFunction:
      str->Append("{");
      LoadNodeToStr(str, node->l_child);
      str->Append("}^{");
      LoadNodeToStr(str, node->r_child);
      str->Append("}");
      break;
    case Operator::kLogFunction: //FIXME
      str->Append("\\log_{");
      LoadNodeToStr(str, node->l_child);
      str->Append("} {(");
      LoadNodeToStr(str, node->r_child);
      str->Append(")} ");
      break;
    case Operator::kLnFunction: //FIXME
      str->Append("\\ln {(");
      LoadNodeToStr(str, node->r_child);
      str->Append(")} ");
      break;
    case Operator::kSinFunction: //FIXME
      str->Append("\\sin {(");
      LoadNodeToStr(str, node->r_child);
      str->Append(")} ");
      break;
    case Operator::kCosFunction: //FIXME
      str->Append("\\cos {(");
      LoadNodeToStr(str, node->r_child);
      str->Append(")} ");
      break;
    case Operator::kUninitOperator:
      break;
    default:
      ASSERT(0 && "UNKNOWN OPERATOR");
      break;
  }

  return TreeError::kSuccess;
}

TreeError BTree::PushConst(String* str, TreeNode* node) {
  ASSERT(str != nullptr);
  ASSERT(node != nullptr);

  if (node->data.type != TypeOfElem::kConstant) {$$( return TreeError::kBadLoad; )}

  char tmp[100] = "";
  if (IsEqual(node->data.value.num, round(node->data.value.num))) {
    sprintf(tmp, " %d ", (int)node->data.value.num);
  } else {
    sprintf(tmp, " %.4lf ", node->data.value.num);
  }
  str->Append(tmp);

  return TreeError::kSuccess;
}

bool BTree::IsValid(String* raw_tree) {//REVIEW mb valid func?
  ASSERT(raw_tree != nullptr);

  Counter brackets = 0;

  const char* str = raw_tree->Data();
  while (str < raw_tree->Data() + raw_tree->Size()) {
    brackets += (*str == '(') - (*str == ')');
    if (brackets < 0) { return false; }

    str++;
  }

  return true;
}

//static-----------------------------------------------------------------------
