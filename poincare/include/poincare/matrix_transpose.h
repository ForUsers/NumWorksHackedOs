#ifndef POINCARE_MATRIX_TRANSPOSE_H
#define POINCARE_MATRIX_TRANSPOSE_H

#include <poincare/expression.h>

namespace Poincare {

class MatrixTransposeNode : public ExpressionNode {
public:

  // TreeNode
  size_t size() const override { return sizeof(MatrixTransposeNode); }
  int numberOfChildren() const override { return 1; }
#if POINCARE_TREE_LOG
  virtual void logNodeName(std::ostream & stream) const override {
    stream << "MatrixTranspose";
  }
#endif

  // Properties
  Type type() const override { return Type::MatrixTranspose; }
private:
  // Layout
  Layout createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  int serialize(char * buffer, int bufferSize, Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const override;
  const char * name() const { return "transpose"; }
  // Simplification
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit) override;
  // Evaluation
  Evaluation<float> approximate(SinglePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<float>(context, angleUnit); }
  Evaluation<double> approximate(DoublePrecision p, Context& context, Preferences::AngleUnit angleUnit) const override { return templatedApproximate<double>(context, angleUnit); }
  template<typename T> Evaluation<T> templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const;
};

class MatrixTranspose : public Expression {
public:
  MatrixTranspose();
  MatrixTranspose(const MatrixTransposeNode * n) : Expression(n) {}
  explicit MatrixTranspose(Expression operand) : MatrixTranspose() {
    replaceChildAtIndexInPlace(0, operand);
  }
  Expression shallowReduce(Context & context, Preferences::AngleUnit angleUnit);
};

}

#endif
