#ifndef POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H
#define POINCARE_ABSOLUTE_VALUE_LAYOUT_NODE_H

#include <poincare/bracket_pair_layout.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class AbsoluteValueLayoutNode : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;

  // SerializationHelperInterface
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "abs");
  }

  // TreeNode
  size_t size() const override { return sizeof(AbsoluteValueLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "AbsoluteValueLayout";
  }
#endif

private:
  KDCoordinate widthMargin() const override { return 2; }
  virtual KDCoordinate verticalExternMargin() const override { return 1; }
  bool renderTopBar() const override { return false; }
  bool renderBottomBar() const override { return false; }
};

class AbsoluteValueLayout : public Layout {
public:
  explicit AbsoluteValueLayout(Layout l) :
    Layout(TreePool::sharedPool()->createTreeNode<AbsoluteValueLayoutNode>())
  {
    replaceChildAtIndexInPlace(0, l);
  }
};

}

#endif
