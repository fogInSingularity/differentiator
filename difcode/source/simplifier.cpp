#include "../include/simplifier.h"

//static-----------------------------------------------------------------------

static Counter SimplifyNode(BTree* tree, TreeNode* node);
static Counter SimplifyOperator(BTree* tree, TreeNode* node);
static Counter SimplifyAddAndSub(BTree* tree, TreeNode* node);
static Counter SimplifyMult(BTree* tree, TreeNode* node);
static Counter SimplifyDiv(BTree* tree, TreeNode* node);

static Counter SimplifyOnConstBoth(BTree* tree, TreeNode* node, double cnst);
static Counter SimplifyOnConstOne(BTree* tree, TreeNode* node, double cnst, bool on_left);
static bool IsNodeEqualTo(TreeNode* node, double num);

//public-----------------------------------------------------------------------

DifError Diffirentiator::Simplify() {$$$
  Counter n_simpl = 0;

  do {
    n_simpl = SimplifyNode(&expression_tree_, expression_tree_.root_->l_child);

    if (n_simpl == -1) {$$( return DifError::kInvalidSimpl; )}
  } while (n_simpl != 0);

  $$( return DifError::kSuccess; )
}

//static-----------------------------------------------------------------------

static Counter SimplifyNode(BTree* tree, TreeNode* node) {$$$
  ASSERT(tree != nullptr);
  ASSERT(node != nullptr);

  if (node->data.type == TypeOfElem::kUninit) {
    $$( return -1; )
  }

  if (node->data.type == TypeOfElem::kConstant
      || node->data.type == TypeOfElem::kVariable) {
    $$( return 0; )
  }

  $$( return SimplifyOperator(tree, node); )
}

static Counter SimplifyOperator(BTree* tree, TreeNode* node) {$$$
  ASSERT(tree != nullptr);
  ASSERT(node != nullptr);
  ASSERT(node->data.type == TypeOfElem::kOperator);

  switch (node->data.value.op) {
    case Operator::kAddition:
    case Operator::kSubtraction:
      $$( return SimplifyAddAndSub(tree, node); )
    case Operator::kMultiplication:
      $$( return SimplifyMult(tree, node); )
    case Operator::kDivision:
      $$( return SimplifyDiv(tree, node); )
    case Operator::kPowerFunction:
    case Operator::kLogFunction: {
      Counter n_simpl_l = SimplifyNode(tree, node->l_child);
      if (n_simpl_l == -1) {$$( return -1; )}
      Counter n_simpl_r = SimplifyNode(tree, node->l_child);
      if (n_simpl_r == -1) {$$( return -1; )}
      $$( return n_simpl_l + n_simpl_r; )
    }
    case Operator::kLnFunction:
      $$( return SimplifyNode(tree, node->r_child); )
    case Operator::kUninitOperator:
      $$( return -1; )
    default:
      ASSERT(0 && "UNKNOWN OPERATOR");
  }
}

static Counter SimplifyAddAndSub(BTree* tree, TreeNode* node) {$$$
  ASSERT(tree != nullptr);
  ASSERT(node != nullptr);
  ASSERT(node->data.type == TypeOfElem::kOperator); //REVIEW
  ASSERT(node->data.value.op == Operator::kAddition
         || node->data.value.op == Operator::kSubtraction);

  Counter n_simpl_l = SimplifyNode(tree, node->l_child);
  if (n_simpl_l == -1) {$$( return -1; )}
  Counter n_simpl_r = SimplifyNode(tree, node->r_child);
  if (n_simpl_r == -1) {$$( return -1; )}

  Counter n_cnst_smpl = SimplifyOnConstBoth(tree, node, 0.0);

  $$( return n_cnst_smpl + n_simpl_l + n_simpl_r; )
}

static Counter SimplifyMult(BTree* tree, TreeNode* node) {$$$
  ASSERT(tree != nullptr);
  ASSERT(node != nullptr);
  ASSERT(node->data.type == TypeOfElem::kOperator);
  ASSERT(node->data.value.op == Operator::kMultiplication);

  Counter n_simpl_l = SimplifyNode(tree, node->l_child);
  if (n_simpl_l == -1) {$$( return -1; )}
  Counter n_simpl_r = SimplifyNode(tree, node->r_child);
  if (n_simpl_r == -1) {$$( return -1; )}

  bool is_left_zero = IsNodeEqualTo(node->l_child, 0.0);
  bool is_right_zero = IsNodeEqualTo(node->r_child, 0.0);

  if (!is_left_zero && !is_right_zero) {
    $$( return SimplifyOnConstBoth(tree, node, 1.0) + n_simpl_l + n_simpl_r; )
  }

  Elem data = {};
  data.type = TypeOfElem::kConstant;
  data.value.num = 0.0;

  TreeNode* zero_node = tree->CtorNode(node->parent, &data);
  if (zero_node == nullptr) {$$( return -1; )}
  RehangParent(node, zero_node);
  tree->DtorNode(node, ElemDtor);

  $$( return 1 + n_simpl_l + n_simpl_r; )
}

static Counter SimplifyDiv(BTree* tree, TreeNode* node) {$$$
  ASSERT(tree != nullptr);
  ASSERT(node != nullptr);
  ASSERT(node->data.type == TypeOfElem::kOperator);
  ASSERT(node->data.value.op == Operator::kDivision);

  Counter n_simpl = SimplifyNode(tree, node->l_child);
  if (n_simpl == -1) {$$( return -1; )}

  bool is_zero = IsNodeEqualTo(node->l_child, 0.0);

  if (!is_zero) {
    return n_simpl;
  }

  Counter n_cnst_smpl = SimplifyOnConstOne(tree, node, 0.0, true);
  if (n_cnst_smpl == -1) {$$( return -1; )}

  return n_cnst_smpl + n_simpl;
}

static Counter SimplifyOnConstBoth(BTree* tree, TreeNode* node, double cnst) {$$$
  ASSERT(tree != nullptr);
  ASSERT(node != nullptr);

  Counter n_simpl_l = SimplifyNode(tree, node->l_child);
  if (n_simpl_l == -1) {$$( return -1; )}
  Counter n_simpl_r = SimplifyNode(tree, node->r_child);
  if (n_simpl_r == -1) {$$( return -1; )}

  bool is_left_cnst = IsNodeEqualTo(node->l_child, cnst);
  bool is_right_cnst = IsNodeEqualTo(node->r_child, cnst);

  if (!is_left_cnst && !is_right_cnst) {
    $$( return n_simpl_l + n_simpl_r; )
  }

  TreeNode* unsimpl_node = nullptr;

  if (is_left_cnst) {
    if (node->parent->l_child == node) {
      node->parent->l_child = node->r_child;
    } else if (node->parent->r_child == node) {
      node->parent->r_child = node->r_child;
    }
    node->r_child->parent = node->parent;
    unsimpl_node = node->r_child;
    node->r_child = nullptr;
  } else {
    if (node->parent->l_child == node) {
      node->parent->l_child = node->l_child;
    } else if (node->parent->r_child == node) {
      node->parent->r_child = node->l_child;
    }
    node->l_child->parent = node->parent;
    unsimpl_node = node->l_child;
    node->l_child = nullptr;
  }

  RehangParent(node, unsimpl_node);
  tree->DtorNode(node, ElemDtor);

  $$( return 1 + n_simpl_l + n_simpl_r; )
}

static Counter SimplifyOnConstOne(BTree* tree, TreeNode* node, double cnst, bool on_left) {$$$
  ASSERT(tree != nullptr);
  ASSERT(node != nullptr);

  TreeNode* cnst_node = on_left ? node->l_child : node->r_child;

  Counter n_simpl = SimplifyNode(tree, cnst_node);
  if (n_simpl == -1) {$$( return -1; )}

  bool is_node_cnst = IsNodeEqualTo(cnst_node, cnst);
  if (!is_node_cnst) {
    $$( return n_simpl; )
  }

  cnst_node->parent = node->parent;
  RehangParent(node, cnst_node);
  tree->DtorNode(node, ElemDtor);

  $$( return 1 + n_simpl; )
}

static bool IsNodeEqualTo(TreeNode* node, double num) {$$$
  ASSERT(node != nullptr);

  if (node->data.type != TypeOfElem::kConstant) {
    $$( return false; )
  }

  $$( return IsEqual(node->data.value.num, num); )
}
