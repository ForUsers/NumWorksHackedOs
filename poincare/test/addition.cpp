#include <quiz.h>
#include <poincare.h>
#include <ion.h>
#include <assert.h>
#include "helper.h"
#include "./tree/helpers.h"

using namespace Poincare;

static inline void assert_approximation_equals(const Expression i, float f) {
  Poincare::GlobalContext c;
  quiz_assert(i.approximateToScalar<float>(c, Preferences::AngleUnit::Degree) == f);
}

static inline void assert_parsed_expression_is_equal_to(const char * exp, Expression e) {
  Expression result = Expression::parse(exp);
  quiz_assert(!result.isUninitialized());
  quiz_assert(result.isIdenticalTo(e));
}

QUIZ_CASE(poincare_addition_cast_does_not_copy) {
  Rational i1(1);
  Rational i2(2);
  Addition j(i1, i2);
  Expression k = j;
  quiz_assert(k.identifier() == (static_cast<Addition&>(k)).identifier());
  quiz_assert(i1.identifier() == (static_cast<Expression&>(i1)).identifier());
  quiz_assert(k.identifier() == (static_cast<Expression&>(k)).identifier());
}

QUIZ_CASE(poincare_addition_without_parsing) {
  Rational i1(1);
  Rational i2(2);
  Addition j(i1, i2);
  assert_approximation_equals(j, 3.0f);
}

QUIZ_CASE(poincare_addition_parsing) {
  Rational i1(1);
  Rational i2(2);
  Addition j1(i1, i2);
  assert_parsed_expression_is_equal_to("1+2", j1);
}

QUIZ_CASE(poincare_addition_evaluate) {
  assert_parsed_expression_evaluates_to<float>("1+2", "3");
  assert_parsed_expression_evaluates_to<float>("I", "I");
  assert_parsed_expression_evaluates_to<float>("I+I", "2*I");
  assert_parsed_expression_evaluates_to<double>("2+I+4+I", "6+2*I");
#if MATRICES_ARE_DEFINED
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]+3", "[[4,5][6,7][8,9]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+I][3,4][5,6]]+3+I", "[[4+I,5+2*I][6+I,7+I][8+I,9+I]]");
  assert_parsed_expression_evaluates_to<float>("3+[[1,2][3,4][5,6]]", "[[4,5][6,7][8,9]]");
  assert_parsed_expression_evaluates_to<double>("3+I+[[1,2+I][3,4][5,6]]", "[[4+I,5+2*I][6+I,7+I][8+I,9+I]]");
  assert_parsed_expression_evaluates_to<float>("[[1,2][3,4][5,6]]+[[1,2][3,4][5,6]]", "[[2,4][6,8][10,12]]");
  assert_parsed_expression_evaluates_to<double>("[[1,2+I][3,4][5,6]]+[[1,2+I][3,4][5,6]]", "[[2,4+2*I][6,8][10,12]]");
#endif
}

QUIZ_CASE(poincare_addition_simplify) {
  assert_parsed_expression_simplify_to("1/2+1/3+1/4+1/5+1/6+1/7", "223/140");
  assert_parsed_expression_simplify_to("1+x+4-i-2x", "(5-i)-x");
  assert_parsed_expression_simplify_to("2+1", "3");
  assert_parsed_expression_simplify_to("1+2", "3");
  assert_parsed_expression_simplify_to("1+2+3+4+5+6+7", "28");
  assert_parsed_expression_simplify_to("(0+0)", "0");
  assert_parsed_expression_simplify_to("2+A", "2+A");
  assert_parsed_expression_simplify_to("1+2+3+4+5+A+6+7", "28+A");
  assert_parsed_expression_simplify_to("1+A+2+B+3", "6+A+B");
  assert_parsed_expression_simplify_to("-2+6", "4");
  assert_parsed_expression_simplify_to("-2-6", "-8");
  assert_parsed_expression_simplify_to("-A", "-A");
  assert_parsed_expression_simplify_to("A-A", "0");
  assert_parsed_expression_simplify_to("-5P+3P", "-2*P");
  assert_parsed_expression_simplify_to("1-3+A-5+2A-4A", "(-7)-A");
  assert_parsed_expression_simplify_to("A+B-A-B", "0");
  assert_parsed_expression_simplify_to("A+B+(-1)*A+(-1)*B", "0");
  assert_parsed_expression_simplify_to("2+13cos(2)-23cos(2)", "2-10*cos(2)");
  assert_parsed_expression_simplify_to("1+1+ln(2)+(5+3*2)/9-4/7+1/98", "(2347+882*ln(2))/882");
  assert_parsed_expression_simplify_to("1+2+0+cos(2)", "3+cos(2)");
  assert_parsed_expression_simplify_to("A-A+2cos(2)+B-B-cos(2)", "cos(2)");
  assert_parsed_expression_simplify_to("x+3+P+2*x", "3+3*x+P");
  assert_parsed_expression_simplify_to("1/(x+1)+1/(P+2)", "(3+x+P)/(2+2*x+P+x*P)");
  assert_parsed_expression_simplify_to("1/x^2+1/(x^2*P)", "(1+P)/(x^2*P)");
  assert_parsed_expression_simplify_to("1/x^2+1/(x^3*P)", "(1+x*P)/(x^3*P)");
  assert_parsed_expression_simplify_to("4x/x^2+3P/(x^3*P)", "(3+4*x^2)/x^3");
  assert_parsed_expression_simplify_to("3^(1/2)+2^(-2*3^(1/2)*X^P)/2", "(1+2*2^(2*R(3)*X^P)*R(3))/(2*2^(2*R(3)*X^P))");
}
