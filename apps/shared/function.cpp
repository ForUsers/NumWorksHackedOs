#include "function.h"
#include <string.h>
#include <cmath>
#include <assert.h>

using namespace Poincare;

namespace Shared {

Function::Function(const char * name, KDColor color) :
  ExpressionModel(),
  m_name(name),
  m_color(color),
  m_active(true)
{
}

uint32_t Function::checksum() {
  char data[k_dataLengthInBytes/sizeof(char)] = {};
  strlcpy(data, text(), TextField::maxBufferSize());
  data[k_dataLengthInBytes-2] = m_name != nullptr ? m_name[0] : 0;
  data[k_dataLengthInBytes-1] = m_active ? 1 : 0;
  return Ion::crc32((uint32_t *)data, k_dataLengthInBytes/sizeof(uint32_t));
}

void Function::setColor(KDColor color) {
  m_color = color;
}

const char * Function::name() const {
  return m_name;
}

bool Function::isActive() {
  return m_active;
}

void Function::setActive(bool active) {
  m_active = active;
}

template<typename T>
T Function::templatedApproximateAtAbscissa(T x, Poincare::Context * context) const {
  return expression(context).approximateWithValueForSymbol(symbol(), x, *context, Preferences::sharedPreferences()->angleUnit());
}

}

template float Shared::Function::templatedApproximateAtAbscissa<float>(float, Poincare::Context*) const;
template double Shared::Function::templatedApproximateAtAbscissa<double>(double, Poincare::Context*) const;
