#include "sequence_title_cell.h"
#include <assert.h>

using namespace Shared;
using namespace Poincare;

namespace Sequence {

SequenceTitleCell::SequenceTitleCell() :
  Shared::FunctionTitleCell(),
  m_titleTextView(0.5f, 0.5f)
{
}

void SequenceTitleCell::setLayout(Poincare::Layout layout) {
  m_titleTextView.setLayout(layout);
}

void SequenceTitleCell::setHighlighted(bool highlight) {
  EvenOddCell::setHighlighted(highlight);
  m_titleTextView.setHighlighted(highlight);
}

void SequenceTitleCell::setEven(bool even) {
  EvenOddCell::setEven(even);
  m_titleTextView.setEven(even);
}

void SequenceTitleCell::setColor(KDColor color) {
  Shared::FunctionTitleCell::setColor(color);
  m_titleTextView.setTextColor(color);
}

int SequenceTitleCell::numberOfSubviews() const {
  return 1;
}

View * SequenceTitleCell::subviewAtIndex(int index) {
  assert(index == 0);
  return &m_titleTextView;
}

void SequenceTitleCell::layoutSubviews() {
  KDRect textFrame(0, k_colorIndicatorThickness, bounds().width(), bounds().height() - k_colorIndicatorThickness);
  if (m_orientation == Orientation::VerticalIndicator){
    textFrame = KDRect(k_colorIndicatorThickness, 0, bounds().width() - k_colorIndicatorThickness-k_separatorThickness, bounds().height()-k_separatorThickness);
  }
  m_titleTextView.setFrame(textFrame);
}

}
