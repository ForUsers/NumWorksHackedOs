#ifndef POINCARE_APPROXIMATION_HELPER_H
#define POINCARE_APPROXIMATION_HELPER_H

#include <poincare/complex.h>
#include <poincare/matrix_complex.h>
#include <poincare/expression_node.h>
#include <complex.h>

namespace Poincare {

namespace ApproximationHelper {
  template <typename T> std::complex<T> TruncateRealOrImaginaryPartAccordingToArgument(std::complex<T> c);

  template <typename T> using ComplexCompute = Complex<T>(*)(const std::complex<T>, Preferences::AngleUnit angleUnit);
  template<typename T> Evaluation<T> Map(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexCompute<T> compute);

  template <typename T> using ComplexAndComplexReduction = Complex<T>(*)(const std::complex<T>, const std::complex<T>);
  template <typename T> using ComplexAndMatrixReduction = MatrixComplex<T>(*)(const std::complex<T> c, const MatrixComplex<T> m);
  template <typename T> using MatrixAndComplexReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const std::complex<T> c);
  template <typename T> using MatrixAndMatrixReduction = MatrixComplex<T>(*)(const MatrixComplex<T> m, const MatrixComplex<T> n);
  template<typename T> Evaluation<T> MapReduce(const ExpressionNode * expression, Context& context, Preferences::AngleUnit angleUnit, ComplexAndComplexReduction<T> computeOnComplexes, ComplexAndMatrixReduction<T> computeOnComplexAndMatrix, MatrixAndComplexReduction<T> computeOnMatrixAndComplex, MatrixAndMatrixReduction<T> computeOnMatrices);

  template<typename T> MatrixComplex<T> ElementWiseOnMatrixComplexAndComplex(const MatrixComplex<T> n, std::complex<T> c, ComplexAndComplexReduction<T> computeOnComplexes);
  template<typename T> MatrixComplex<T> ElementWiseOnComplexMatrices(const MatrixComplex<T> m, const MatrixComplex<T> n, ComplexAndComplexReduction<T> computeOnComplexes);
};

}

#endif
