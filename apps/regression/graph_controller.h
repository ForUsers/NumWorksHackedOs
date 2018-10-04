#ifndef REGRESSION_GRAPH_CONTROLLER_H
#define REGRESSION_GRAPH_CONTROLLER_H

#include <escher.h>
#include "banner_view.h"
#include "store.h"
#include "graph_options_controller.h"
#include "graph_view.h"
#include "initialisation_parameter_controller.h"
#include "../shared/interactive_curve_view_controller.h"
#include "../shared/curve_view_cursor.h"
#include "../shared/cursor_view.h"
#include "../shared/round_cursor_view.h"

namespace Regression {

class GraphController : public Shared::InteractiveCurveViewController {

public:
  GraphController(Responder * parentResponder, ButtonRowController * header, Store * store, Shared::CurveViewCursor * cursor, uint32_t * modelVersion, uint32_t * rangeVersion, int * selectedDotIndex, int * selectedSeriesIndex);
  ViewController * initialisationParameterController() override;
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  void viewWillAppear() override;
  void selectRegressionCurve();
  int selectedSeriesIndex() const { return *m_selectedSeriesIndex; }
private:
  constexpr static int k_maxLegendLength = 16;
  constexpr static int k_maxNumberOfCharacters = 50;
  constexpr static float k_viewHeight = 174.0f;
  Poincare::Context * globalContext();
  Shared::CurveView * curveView() override;
  Shared::InteractiveCurveViewRange * interactiveCurveViewRange() override;
  bool handleEnter() override;
  void reloadBannerView() override;
  void initRangeParameters() override;
  void initCursorParameters() override;
  bool moveCursorHorizontally(int direction) override;
  bool moveCursorVertically(int direction) override;
  uint32_t modelVersion() override;
  uint32_t rangeVersion() override;
  bool isCursorVisible() override;

  // InteractiveCurveViewController
  float displayTopMarginRatio() override;
  float displayBottomMarginRatio() override;

  float cursorTopMarginRatio() { return 0.07f; } // (cursorHeight/2) / graphViewHeight
  float cursorBottomMarginRatio();
  float estimatedBannerHeight() const;

  // InteractiveCurveViewRangeDelegate
  Shared::InteractiveCurveViewRangeDelegate::Range computeYRange(Shared::InteractiveCurveViewRange * interactiveCurveViewRange) override;

  Shared::CursorView m_crossCursorView;
  Shared::RoundCursorView m_roundCursorView;
  BannerView m_bannerView;
  GraphView m_view;
  Store * m_store;
  InitialisationParameterController m_initialisationParameterController;
  GraphOptionsController m_graphOptionsController;
  /* The selectedDotIndex is -1 when no dot is selected, m_numberOfPairs when
   * the mean dot is selected and the dot index otherwise */
  int * m_selectedDotIndex;
  int * m_selectedSeriesIndex;
  Model::Type m_modelType[Store::k_numberOfSeries];
};

}


#endif
