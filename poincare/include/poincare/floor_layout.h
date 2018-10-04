#ifndef POINCARE_FLOOR_LAYOUT_NODE_H
#define POINCARE_FLOOR_LAYOUT_NODE_H

#include <poincare/bracket_pair_layout.h>
#include <poincare/layout_helper.h>
#include <poincare/serialization_helper.h>

namespace Poincare {

class FloorLayoutNode : public BracketPairLayoutNode {
public:
  using BracketPairLayoutNode::BracketPairLayoutNode;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override {
    return SerializationHelper::Prefix(this, buffer, bufferSize, floatDisplayMode, numberOfSignificantDigits, "floor");
  }

  // TreeNode
  size_t size() const override { return sizeof(FloorLayoutNode); }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "FloorLayout";
  }
#endif

protected:
  bool renderTopBar() const override { return false; }
};

class FloorLayout : public Layout {
public:
  explicit FloorLayout(Layout l) : Layout(TreePool::sharedPool()->createTreeNode<FloorLayoutNode>()) {
    replaceChildAtIndexInPlace(0, l);
  }
};

}

#endif
