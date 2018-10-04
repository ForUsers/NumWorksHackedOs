#include <poincare/factor.h>
#include <poincare/undefined.h>
#include <poincare/arithmetic.h>
#include <poincare/power.h>
#include <poincare/division.h>
#include <poincare/opposite.h>

extern "C" {
#include <stdlib.h>
#include <assert.h>
}
#include <cmath>

namespace Poincare {

Layout FactorNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return LayoutHelper::Prefix(Factor(this), floatDisplayMode, numberOfSignificantDigits, name());
}

Factor::Factor() : Expression(TreePool::sharedPool()->createTreeNode<FactorNode>()) {}

Expression FactorNode::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  return Factor(this).shallowBeautify(context, angleUnit);
}

Expression Factor::shallowBeautify(Context & context, Preferences::AngleUnit angleUnit) {
  Expression c = childAtIndex(0);
  if (c.type() != ExpressionNode::Type::Rational) {
    Expression result = Undefined();
    replaceWithInPlace(result);
    return result;
  }
  Rational r = static_cast<Rational &>(c);
  if (r.isZero()) {
    replaceWithInPlace(r);
    return r;
  }
  Multiplication numeratorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r.unsignedIntegerNumerator(), context, angleUnit);
  if (numeratorDecomp.numberOfChildren() == 0) {
    Expression result = Undefined();
    replaceWithInPlace(result);
    return result;
  }
  Expression result = numeratorDecomp.squashUnaryHierarchyInPlace();
  if (!r.integerDenominator().isOne()) {
    Multiplication denominatorDecomp = createMultiplicationOfIntegerPrimeDecomposition(r.integerDenominator(), context, angleUnit);
    if (denominatorDecomp.numberOfChildren() == 0) {
      Expression result = Undefined();
      replaceWithInPlace(result);
      return result;
    }
    result = Division(result, denominatorDecomp.squashUnaryHierarchyInPlace());
  }
  if (r.sign() == ExpressionNode::Sign::Negative) {
    result = Opposite(result);
  }
  replaceWithInPlace(result);
  return result;
}

Multiplication Factor::createMultiplicationOfIntegerPrimeDecomposition(Integer i, Context & context, Preferences::AngleUnit angleUnit) const {
  assert(!i.isZero());
  assert(!i.isNegative());
  Multiplication m;
  Integer factors[Arithmetic::k_maxNumberOfPrimeFactors];
  Integer coefficients[Arithmetic::k_maxNumberOfPrimeFactors];
  int numberOfPrimeFactors = Arithmetic::PrimeFactorization(i, factors, coefficients, Arithmetic::k_maxNumberOfPrimeFactors);
  if (numberOfPrimeFactors == 0) {
    m.addChildAtIndexInPlace(Rational(i), 0, 0);
    return m;
  }
  if (numberOfPrimeFactors < 0) {
    // Exception: the decomposition failed
    return m;
  }
  for (int index = 0; index < numberOfPrimeFactors; index++) {
    Expression factor = Rational(factors[index]);
    if (!coefficients[index].isOne()) {
      factor = Power(factor, Rational(coefficients[index]));
    }
    m.addChildAtIndexInPlace(factor, m.numberOfChildren(), m.numberOfChildren());
  }
  return m;
}

}
