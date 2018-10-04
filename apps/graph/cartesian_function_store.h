#ifndef GRAPH_CARTESIAN_FUNCTION_STORE_H
#define GRAPH_CARTESIAN_FUNCTION_STORE_H

#include "cartesian_function.h"
#include "../shared/function_store.h"
#include <stdint.h>
#include <escher.h>

namespace Graph {

class CartesianFunctionStore : public Shared::FunctionStore {
public:
  CartesianFunctionStore();
  uint32_t storeChecksum() override;
  CartesianFunction * modelAtIndex(int i) override { return &m_functions[i]; }
  CartesianFunction * activeFunctionAtIndex(int i) override { return (CartesianFunction *)Shared::FunctionStore::activeFunctionAtIndex(i); }
  CartesianFunction * definedFunctionAtIndex(int i) override { return (CartesianFunction *)Shared::FunctionStore::definedFunctionAtIndex(i); }
  int maxNumberOfModels() const override {
    return k_maxNumberOfFunctions;
  }
  char symbol() const override;
  void removeAll() override;
  static constexpr int k_maxNumberOfFunctions = 4;
private:
  static constexpr const char * k_functionNames[k_maxNumberOfFunctions] = {
    "f", "g", "h", "p",
  };
  CartesianFunction * emptyModel() override;
  CartesianFunction * nullModel() override;
  void setModelAtIndex(Shared::ExpressionModel * f, int i) override;
  const char * firstAvailableName() override {
    return firstAvailableAttribute(k_functionNames, FunctionStore::name);
  }
  CartesianFunction m_functions[k_maxNumberOfFunctions];
};

}

#endif
