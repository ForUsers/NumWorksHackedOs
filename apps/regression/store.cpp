#include "store.h"
#include "apps/apps_container.h"
#include <poincare/preferences.h>
#include <assert.h>
#include <float.h>
#include <cmath>
#include <string.h>

using namespace Shared;

namespace Regression {

static inline float max(float x, float y) { return (x>y ? x : y); }
static inline float min(float x, float y) { return (x<y ? x : y); }

static_assert(Model::k_numberOfModels == 9, "Number of models changed, Regression::Store() needs to adapt");
static_assert(Store::k_numberOfSeries == 3, "Number of series changed, Regression::Store() needs to adapt (m_seriesChecksum)");

Store::Store() :
  InteractiveCurveViewRange(nullptr),
  DoublePairStore(),
  m_seriesChecksum{0, 0, 0},
  m_angleUnit(Poincare::Preferences::AngleUnit::Degree)
{
  for (int i = 0; i < k_numberOfSeries; i++) {
    m_regressionTypes[i] = Model::Type::Linear;
    m_regressionChanged[i] = false;
  }
}

void Store::tidy() {
  for (int i = 0; i < Model::k_numberOfModels; i++) {
    regressionModel(i)->tidy();
  }
}

/* Regressions */
void Store::setSeriesRegressionType(int series, Model::Type type) {
  assert(series >= 0 && series < k_numberOfSeries);
  if (m_regressionTypes[series] != type) {
    m_regressionTypes[series] = type;
    m_regressionChanged[series] = true;
  }
}

int Store::closestVerticalRegression(int direction, double x, double y, int currentRegressionSeries, Poincare::Context * globalContext) {
  int regressionSeries = -1;
  float closestDistance = INFINITY;
  /* The conditions to test on all the regressions are in this order:
   * - the current regression is not the current regression
   * - the next regression point should be within the window abscissa bounds
   * - it is the closest one in abscissa to x
   * - it is above y if direction > 0 and below otherwise */
  for (int series = 0; series < k_numberOfSeries; series ++) {
    if (!seriesIsEmpty(series) && series != currentRegressionSeries) {
      double regressionY = yValueForXValue(series, x, globalContext);
      if ((m_yMin <= regressionY && regressionY <= m_yMax)
          && (std::fabs(regressionY - y) < closestDistance)
          && (regressionY - y > 0) == (direction > 0)) {
        closestDistance = std::fabs(regressionY - y);
        regressionSeries = series;
      }
    }
  }
  return regressionSeries;
}

/* Dots */

int Store::closestVerticalDot(int direction, double x, double y, int currentSeries, int currentDot, int * nextSeries, Poincare::Context * globalContext) {
  double nextX = INFINITY;
  double nextY = INFINITY;
  int selectedDot = -1;
  /* The conditions to test on all dots are in this order:
   * - if the currentDot is valid, the next series should not be the current series
   * - the next dot should not be the current dot
   * - the next dot should be within the window abscissa bounds
   * - the next dot is the closest one in abscissa to x
   * - the next dot is above the regression curve if direction == 1 and below
   * otherwise
   * - the next dot is above/under y
   * - if the current dot is valid, do not select a dot of the same series */
  for (int series = 0; series < k_numberOfSeries; series ++) {
    if (!seriesIsEmpty(series) && (currentDot < 0 || currentSeries != series)) {
      for (int index = 0; index < numberOfPairsOfSeries(series); index++) {
        if ((currentSeries != series) || (index != currentDot)) {
          double currentDataX = m_data[series][0][index];
          double currentDataY = m_data[series][1][index];
          if ((m_xMin <= currentDataX && currentDataX <= m_xMax) &&
              (std::fabs(currentDataX - x) <= std::fabs(nextX - x)) &&
              ((currentDataY - yValueForXValue(currentSeries, currentDataX, globalContext) >= 0) == (direction > 0)) &&
              ((currentDataY > y) == (direction > 0))) {
            // Handle edge case: if 2 dots have the same abscissa but different ordinates
            if (nextX != currentDataX || ((nextY - currentDataY >= 0) == (direction > 0))) {
              nextX = currentDataX;
              nextY = currentDataY;
              selectedDot = index;
              *nextSeries = series;
            }
          }
        }
      }
      // Compare with the mean dot
      if ((currentSeries != series) || (numberOfPairsOfSeries(series) != currentDot)) {
        double meanX = meanOfColumn(series, 0);
        double meanY = meanOfColumn(series, 1);
        if (m_xMin <= meanX && meanX <= m_xMax &&
            (std::fabs(meanX - x) <= std::fabs(nextX - x)) &&
            ((meanY - yValueForXValue(currentSeries, meanX, globalContext) >= 0) == (direction > 0)) &&
            ((meanY > y) == (direction > 0))) {
          if (nextX != meanX || ((nextY - meanY >= 0) == (direction > 0))) {
            selectedDot = numberOfPairsOfSeries(series);
            *nextSeries = series;
          }
        }
      }
    }
  }
  return selectedDot;
}

int Store::nextDot(int series, int direction, int dot) {
  float nextX = INFINITY;
  int selectedDot = -1;
  double meanX = meanOfColumn(series, 0);
  float x = meanX;
  if (dot >= 0 && dot < numberOfPairsOfSeries(series)) {
    x = get(series, 0, dot);
  }
  /* We have to scan the Store in opposite ways for the 2 directions to ensure to
   * select all dots (even with equal abscissa) */
  if (direction > 0) {
    for (int index = 0; index < numberOfPairsOfSeries(series); index++) {
      /* The conditions to test are in this order:
       * - the next dot is the closest one in abscissa to x
       * - the next dot is not the same as the selected one
       * - the next dot is at the right of the selected one */
      if (std::fabs(m_data[series][0][index] - x) < std::fabs(nextX - x) &&
          (index != dot) &&
          (m_data[series][0][index] >= x)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_data[series][0][index] != x || (index > dot)) {
          nextX = m_data[series][0][index];
          selectedDot = index;
        }
      }
    }
    // Compare with the mean dot
    if (std::fabs(meanX - x) < std::fabs(nextX - x) &&
        (numberOfPairsOfSeries(series) != dot) &&
        (meanX >= x)) {
      if (meanX != x || (numberOfPairsOfSeries(series) > dot)) {
        selectedDot = numberOfPairsOfSeries(series);
      }
    }
  } else {
    // Compare with the mean dot
    if (std::fabs(meanX - x) < std::fabs(nextX - x) &&
        (numberOfPairsOfSeries(series) != dot) &&
        (meanX <= x)) {
      if ((meanX != x) || (numberOfPairsOfSeries(series) < dot)) {
        nextX = meanX;
        selectedDot = numberOfPairsOfSeries(series);
      }
    }
    for (int index = numberOfPairsOfSeries(series)-1; index >= 0; index--) {
      if (std::fabs(m_data[series][0][index] - x) < std::fabs(nextX - x) &&
          (index != dot) &&
          (m_data[series][0][index] <= x)) {
        // Handle edge case: 2 dots have same abscissa
        if (m_data[series][0][index] != x || (index < dot)) {
          nextX = m_data[series][0][index];
          selectedDot = index;
        }
      }
    }
  }
  return selectedDot;
}

/* Window */

void Store::setDefault() {
  float minX = FLT_MAX;
  float maxX = -FLT_MAX;
  for (int series = 0; series < k_numberOfSeries; series++) {
    if (!seriesIsEmpty(series)) {
      minX = min(minX, minValueOfColumn(series, 0));
      maxX = max(maxX, maxValueOfColumn(series, 0));
    }
  }
  float range = maxX - minX;
  setXMin(minX - k_displayHorizontalMarginRatio*range);
  setXMax(maxX + k_displayHorizontalMarginRatio*range);
  setYAuto(true);
}

/* Series */

bool Store::seriesIsEmpty(int series) const {
  return numberOfPairsOfSeries(series) < 2;
}

/* Calculations */

double * Store::coefficientsForSeries(int series, Poincare::Context * globalContext) {
  assert(series >= 0 && series <= k_numberOfSeries);
  assert(!seriesIsEmpty(series));
  uint32_t storeChecksumSeries = storeChecksumForSeries(series);
  Poincare::Preferences::AngleUnit currentAngleUnit = Poincare::Preferences::sharedPreferences()->angleUnit();
  if (m_angleUnit != currentAngleUnit) {
    m_angleUnit = currentAngleUnit;
    for (int i = 0; i < k_numberOfSeries; i++) {
      if (m_regressionTypes[i] == Model::Type::Trigonometric) {
        m_regressionChanged[i] = true;
      }
    }
  }
  if (m_regressionChanged[series] || (m_seriesChecksum[series] != storeChecksumSeries)) {
    Model * seriesModel = modelForSeries(series);
    seriesModel->fit(this, series, m_regressionCoefficients[series], globalContext);
    m_regressionChanged[series] = false;
    m_seriesChecksum[series] = storeChecksumSeries;
  }
  return m_regressionCoefficients[series];
}

double Store::doubleCastedNumberOfPairsOfSeries(int series) const {
  return DoublePairStore::numberOfPairsOfSeries(series);
}

float Store::maxValueOfColumn(int series, int i) const {
  float maxColumn = -FLT_MAX;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    maxColumn = max(maxColumn, m_data[series][i][k]);
  }
  return maxColumn;
}

float Store::minValueOfColumn(int series, int i) const {
  float minColumn = FLT_MAX;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    minColumn = min(minColumn, m_data[series][i][k]);
  }
  return minColumn;
}

double Store::squaredValueSumOfColumn(int series, int i) const {
  double result = 0;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    result += m_data[series][i][k]*m_data[series][i][k];
  }
  return result;
}

double Store::columnProductSum(int series) const {
  double result = 0;
  for (int k = 0; k < numberOfPairsOfSeries(series); k++) {
    result += m_data[series][0][k]*m_data[series][1][k];
  }
  return result;
}

double Store::meanOfColumn(int series, int i) const {
  return numberOfPairsOfSeries(series) == 0 ? 0 : sumOfColumn(series, i)/numberOfPairsOfSeries(series);
}

double Store::varianceOfColumn(int series, int i) const {
  double mean = meanOfColumn(series, i);
  return squaredValueSumOfColumn(series, i)/numberOfPairsOfSeries(series) - mean*mean;
}

double Store::standardDeviationOfColumn(int series, int i) const {
  return std::sqrt(varianceOfColumn(series, i));
}

double Store::covariance(int series) const {
  return columnProductSum(series)/numberOfPairsOfSeries(series) - meanOfColumn(series, 0)*meanOfColumn(series, 1);
}

double Store::slope(int series) const {
  return covariance(series)/varianceOfColumn(series, 0);
}

double Store::yIntercept(int series) const {
  return meanOfColumn(series, 1) - slope(series)*meanOfColumn(series, 0);
}

double Store::yValueForXValue(int series, double x, Poincare::Context * globalContext) {
  Model * model = regressionModel((int)m_regressionTypes[series]);
  double * coefficients = coefficientsForSeries(series, globalContext);
  return model->evaluate(coefficients, x);
}

double Store::xValueForYValue(int series, double y, Poincare::Context * globalContext) {
  Model * model = regressionModel((int)m_regressionTypes[series]);
  double * coefficients = coefficientsForSeries(series, globalContext);
  return model->levelSet(coefficients, xMin(), xGridUnit()/10.0, xMax(), y, globalContext);
}

double Store::correlationCoefficient(int series) const {
  double sd0 = standardDeviationOfColumn(series, 0);
  double sd1 = standardDeviationOfColumn(series, 1);
  return (sd0 == 0.0 || sd1 == 0.0) ? 1.0 : covariance(series)/(sd0*sd1);
}

double Store::squaredCorrelationCoefficient(int series) const {
  double cov = covariance(series);
  double v0 = varianceOfColumn(series, 0);
  double v1 = varianceOfColumn(series, 1);
  return (v0 == 0.0 || v1 == 0.0) ? 1.0 : cov*cov/(v0*v1);
}

Model * Store::regressionModel(int index) {
  Model * models[Model::k_numberOfModels] = {&m_linearModel, &m_quadraticModel, &m_cubicModel, &m_quarticModel, &m_logarithmicModel, &m_exponentialModel, &m_powerModel, &m_trigonometricModel, &m_logisticModel};
  return models[index];
}

}
