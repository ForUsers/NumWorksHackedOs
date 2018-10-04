#ifndef STATISTICS_HISTOGRAM_BANNER_VIEW_H
#define STATISTICS_HISTOGRAM_BANNER_VIEW_H

#include <escher.h>
#include "../shared/banner_view.h"
#include "../i18n.h"

namespace Statistics {

class HistogramBannerView : public Shared::BannerView {
public:
  HistogramBannerView();
private:
  static constexpr int k_numberOfSubviews = 6;
  int numberOfSubviews() const override { return k_numberOfSubviews; }
  TextView * textViewAtIndex(int i) const override;
  MessageTextView * messageTextViewAtIndex(int index) const override;
  MessageTextView m_intervalLegendView;
  BufferTextView m_intervalView;
  MessageTextView m_sizeLegendView;
  BufferTextView m_sizeView;
  MessageTextView m_frequencyLegendView;
  BufferTextView m_frequencyView;
};

}

#endif
