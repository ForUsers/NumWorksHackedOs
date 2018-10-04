#include <poincare/sequence.h>
#include <poincare/decimal.h>
#include <poincare/symbol.h>
#include <poincare/undefined.h>
#include <poincare/variable_context.h>
extern "C" {
#include <assert.h>
#include <stdlib.h>
}
#include <cmath>

namespace Poincare {

Layout SequenceNode::createLayout(Preferences::PrintFloatMode floatDisplayMode, int numberOfSignificantDigits) const {
  return createSequenceLayout(childAtIndex(0)->createLayout(floatDisplayMode, numberOfSignificantDigits), childAtIndex(1)->createLayout(floatDisplayMode, numberOfSignificantDigits), childAtIndex(2)->createLayout(floatDisplayMode, numberOfSignificantDigits));
}

template<typename T>
Evaluation<T> SequenceNode::templatedApproximate(Context& context, Preferences::AngleUnit angleUnit) const {
  Evaluation<T> aInput = childAtIndex(1)->approximate(T(), context, angleUnit);
  Evaluation<T> bInput = childAtIndex(2)->approximate(T(), context, angleUnit);
  T start = aInput.toScalar();
  T end = bInput.toScalar();
  if (std::isnan(start) || std::isnan(end) || start != (int)start || end != (int)end || end - start > k_maxNumberOfSteps) {
    return Complex<T>::Undefined();
  }
  VariableContext<T> nContext = VariableContext<T>('n', &context);
  Evaluation<T> result = Complex<T>((T)emptySequenceValue());
  for (int i = (int)start; i <= (int)end; i++) {
    if (Expression::shouldStopProcessing()) {
      return Complex<T>::Undefined();
    }
    nContext.setApproximationForVariable((T)i);
    result = evaluateWithNextTerm(T(), result, childAtIndex(0)->approximate(T(), nContext, angleUnit));
    if (result.isUndefined()) {
      return Complex<T>::Undefined();
    }
  }
  return result;
}

}
