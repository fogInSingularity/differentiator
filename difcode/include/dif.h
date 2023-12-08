#ifndef DIF_H_
#define DIF_H_

#include "../../treecode/include/tree_config.h"

#include "../../treecode/include/b_tree.h"
#include "../../libcode/include/utils.h"
#include "../../libcode/include/bin_file.h"

enum class DifError {
  kSuccess            = 0,
  kNotEnoughFiles     = 1,
  kCantOpenSourceFile = 2,
  kCantOpenDifFile    = 3,
  kBadArgsPassedToDif = 4,
  kCtorBadString      = 5,
  kInvalidExpression  = 6,
  kInvalidDif         = 7,
};

struct Diffirentiator {
 public:
  DifError Ctor(const int argc, const char** argv);
  void Dtor();
  void ThrowError(DifError error);
  DifError Diffirentiate();
 private:
  DifError DifNode(TreeNode* node);
  DifError DifNodeOperator(TreeNode* node);
  DifError DifNodeMult(TreeNode* node);
  DifError DifNodeDiv(TreeNode* node);
  TreeNode* CopyAndDifR(TreeNode* node, TreeNode* new_node);
  TreeNode* CopyAndDifL(TreeNode* node, TreeNode* new_node);
  TreeNode* CopyAndDif(TreeNode* node);

  BTree expression_tree_;
};

#endif // DIF_H_
