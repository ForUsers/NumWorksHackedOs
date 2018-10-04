#ifndef GRAPH_LIST_CONTROLLER_H
#define GRAPH_LIST_CONTROLLER_H

#include <escher.h>
#include "../../shared/function_list_controller.h"
#include "../cartesian_function_store.h"
#include "../../shared/buffer_function_title_cell.h"
#include "../../shared/function_expression_cell.h"
#include "../../shared/list_parameter_controller.h"

namespace Graph {

class ListController : public Shared::FunctionListController {
public:
  ListController(Responder * parentResponder, CartesianFunctionStore * functionStore, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
private:
  Shared::ListParameterController * parameterController() override;
  int maxNumberOfRows() override;
  HighlightCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  bool removeModelRow(Shared::ExpressionModel * function) override;
  constexpr static int k_maxNumberOfRows = 5;
  Shared::BufferFunctionTitleCell m_functionTitleCells[k_maxNumberOfRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfRows];
  Shared::ListParameterController m_parameterController;
};

}

#endif
