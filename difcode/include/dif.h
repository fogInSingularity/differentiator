#ifndef DIF_H_
#define DIF_H_

#include <stdio.h>

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
  kInvalidSimpl       = 8,
};

struct Diffirentiator {
 public:
  DifError Ctor(const int argc, const char** argv);
  void Dtor();
  void ThrowError(DifError error);
  DifError Diffirentiate();
  DifError Simplify();
 private:
  DifError DifNode(TreeNode* node);
  DifError DifNodeOperator(TreeNode* node);
  DifError DifNodeMult(TreeNode* node);
  DifError DifNodeDiv(TreeNode* node);
  DifError DifNodePower(TreeNode* node);
  DifError DifNodePowerGeneral(TreeNode* node);
  DifError DifNodePowerCnstBase(TreeNode* node);
  DifError DifNodePowerCnstPower(TreeNode* node);
  DifError DifNodeLog(TreeNode* node);
  DifError DifNodeLn(TreeNode* node);
  DifError DifNodeSin(TreeNode* node);
  DifError DifNodeCos(TreeNode* node);

  BTree expression_tree_;
  BTree source_tree;
  const char* out_file_name_;
};

void ElemDtor(Elem* data);
bool RehangParent(TreeNode* old_child, TreeNode* new_child);

#endif // DIF_H_
