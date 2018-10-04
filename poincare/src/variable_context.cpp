#include <poincare/variable_context.h>
#include <poincare/preferences.h>
#include <poincare/symbol.h>

#include <cmath>

namespace Poincare {

template<typename T>
VariableContext<T>::VariableContext(char name, Context * parentContext) :
  m_name(name),
  m_value(Float<T>(NAN)),
  m_parentContext(parentContext)
{
}

template<typename T>
void VariableContext<T>::setApproximationForVariable(T value) {
  m_value = Float<T>(value);
}

template<typename T>
void VariableContext<T>::setExpressionForSymbolName(const Expression & expression, const Symbol & symbol, Context & context) {
  if (symbol.name() == m_name) {
    if (expression.isUninitialized()) {
      return;
    }
    m_value = Float<T>(expression.approximateToScalar<T>(context, Preferences::sharedPreferences()->angleUnit()));
  } else {
    m_parentContext->setExpressionForSymbolName(expression, symbol, context);
  }
}

template<typename T>
const Expression VariableContext<T>::expressionForSymbol(const Symbol & symbol) {
  if (symbol.name() == m_name) {
    return m_value;
  } else {
    return m_parentContext->expressionForSymbol(symbol);
  }
}

template class Poincare::VariableContext<float>;
template class Poincare::VariableContext<double>;

}
