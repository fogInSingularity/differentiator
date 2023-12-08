#include "../include/b_tree.h"

//static-----------------------------------------------------------------------

const char* kDotName = "dotdump.dot";

static const char* SkipSpaces(const char* move);
static Counter SkippedAlpAndNums(const char* str);

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
  DtorNode(node->r_child, ElemDtor);

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


TreeError BTree::LoadToStr(String* str) {//REVIEW
  ASSERT(str != nullptr);

  StringError str_error = StringError::kSuccess;
  str_error = str->Clear();
  if (str_error != StringError::kSuccess) { return TreeError::kBadLoad; }

  return LoadNodeToStr(str, root_);
}

TreeError BTree::LoadFromStr(String* str) {//REVIEW
  ASSERT(str != nullptr)

  if (!IsValid(str)) { return TreeError::kInvalidTree; }

  if (root_ != nullptr) { return TreeError::kInvalidTree; }

  Counter shift = 0;
  root_ = LoadNodeFromStr(str->Data(), &shift, nullptr);
  if (root_ == nullptr) { return TreeError::kInvalidTree; }

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

TreeError BTree::LoadNodeToStr(String* str, TreeNode* node) {//REVIEW
  ASSERT(str != nullptr);
  // node can be nullptr

  // StringError str_error = StringError::kSuccess;
  // TreeError tree_error = TreeError::kSuccess;

  // if (node != nullptr) {
  //   str_error = PushNodeToStr(str, node);
  //   if (str_error != StringError::kSuccess) { return TreeError::kStringError; }

  //   tree_error = LoadNodeToStr(str, node->l_child);
  //   if (tree_error != TreeError::kSuccess) { return tree_error; }
  //   tree_error = LoadNodeToStr(str, node->r_child);
  //   if (tree_error != TreeError::kSuccess) { return tree_error; }

  //   str_error = str->PushBack(')');
  //   if (str_error != StringError::kSuccess) { return TreeError::kStringError; }
  // } else {
  //   str_error = str->PushBack('_');
  //   if (str_error != StringError::kSuccess) { return TreeError::kStringError; }
  // }

  USE_VAR(node)

  return TreeError::kSuccess;
}

StringError BTree::PushNodeToStr(String* str, TreeNode* node) {//REVIEW
  ASSERT(str != nullptr);
  ASSERT(node != nullptr);

  // StringError str_error = StringError::kSuccess;

  // str_error = str->Reserve(node->data.str.Length() + 4);
  // if (str_error != StringError::kSuccess) { return str_error; }

  // str_error = str->PushBack('(');
  // if (str_error != StringError::kSuccess) { return str_error; }

  // str_error = str->PushBack('\"');
  // if (str_error != StringError::kSuccess) { return str_error; }

  // str_error = str->Append(node->data.str.Data());
  // if (str_error != StringError::kSuccess) { return str_error; }

  // str_error = str->PushBack('\"');
  // if (str_error != StringError::kSuccess) { return str_error; }

  return StringError::kSuccess;
}

TreeNode* BTree::LoadNodeFromStr(const char* str,
                                 Counter* shift,
                                 TreeNode* parent) {//REVIEW
  ASSERT(str != nullptr);
  ASSERT(shift != nullptr);
  // parent can be nullptr

  // (*shift)++;

  // TreeNode* new_node = {};
  // Elem data = {};

  // if (*(str + *shift) == '_') {
  //   (*shift)++;

  //   size_t num_len = SkippedAlpAndNums(str);
  //   double var = ParseNum(str + *shift, num_len);
  //   if (isnan(var)) {
  //     data.type = TypeOfElem::kVariable;
  //     data.value.value = var;
  //   } else {
  //     data.type = TypeOfElem::kConstant;

  //   }

  //   *shift += num_len + 1;
  //   if (*(str + *shift) != '_') { return nullptr; }

  //   return nullptr; //FIXME
  // }

  // new_node = CtorNode(parent, &data);
  // if (new_node == nullptr) { return nullptr; }

  // new_node->l_child = LoadNodeFromStr(str, shift, new_node);
  // if (new_node->l_child == nullptr) {
  //   DtorNode(new_node);
  //   return nullptr;
  // }

  // //data setup and move shift

  // new_node->r_child = LoadNodeFromStr(str, shift, new_node);
  // if (new_node->r_child == nullptr) {
  //   DtorNode(new_node);
  //   return nullptr;
  // }

  // return new_node;

  //FIXME ^

  // const char* left_quat = strchr(str + *shift, '\"');
  // const char* right_quat = strchr(left_quat + 1, '\"');
  // *shift += right_quat - (str + *shift) + 1;

  // Elem data = {};
  // // data.str.Ctor((size_t)(right_quat - left_quat - 1), left_quat + 1);

  // TreeNode* new_node = {};
  // if (*(str + *shift) == '_') {
  //   data.type = TypeOfElem::kObject;
  //   new_node = CtorNode(parent, &data);
  //   *shift += 3;
  //   return new_node;
  // } else {
  //   data.type = TypeOfElem::kProperty;
  //   new_node = CtorNode(parent, &data);
  //   if (new_node == nullptr) { return nullptr; }

  //   new_node->l_child = LoadNodeFromStr(str, shift, new_node);
  //   if (new_node->l_child == nullptr) {
  //     DtorNode(new_node);
  //     return nullptr;
  //   }

  //   new_node->r_child = LoadNodeFromStr(str, shift, new_node);
  //   if (new_node->r_child == nullptr) {
  //     DtorNode(new_node);
  //     return nullptr;
  //   }
  // }

  // return new_node;
  USE_VAR(parent);

  return nullptr;
}

bool BTree::IsValid(String* raw_tree) {//REVIEW mb valid func?
  ASSERT(raw_tree != nullptr);

  Counter brackets = 0;
  Counter quat     = 0;
  Counter star     = 0;

  Counter error    = 0;

  const char* str = raw_tree->Data();
  while (*str != '\0') {
    brackets += (*str == '(') - (*str == ')');
    quat     += (*str == '\"');
    star     += (*str == '_');

    error    += (*str == ')') * (quat % 2);

    str++;
  }

  return !brackets && (quat % 2 == 0) && (star % 2 == 0) && !error;
}

//static-----------------------------------------------------------------------

__attribute__((__unused__))
static const char* SkipSpaces(const char* move) {
  ASSERT(move != nullptr);

  while (isspace(*move)) { move++; }

  return move;
}

__attribute__((__unused__))
static Counter SkippedAlpAndNums(const char* str) {
  ASSERT(str != nullptr);

  const char* move_str = str;

  while((isalpha(*move_str) || isdigit(*move_str)) && (*move_str != '\0')) {
    move_str++;
  }

  return move_str - str;
}
