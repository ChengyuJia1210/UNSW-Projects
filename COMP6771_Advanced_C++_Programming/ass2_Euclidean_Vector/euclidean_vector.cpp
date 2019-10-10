#include "assignments/ev/euclidean_vector.h"

#include <algorithm>  // Look at these - they are helpful https://en.cppreference.com/w/cpp/algorithm
#include <iostream>
#include <string>

// =====================> constructor <=====================
// *****Default Constructor*****
EuclideanVector::EuclideanVector(int i) {
  this->magnitudes_ = std::make_unique<double[]>(i);
  this->array_size_ = i;
  for (auto iter = 0; iter < i; ++iter) {
    this->magnitudes_[iter] = 0.0;
  }
}

// This constructor's parameters: first means dimensions of this Euclidean vector
// second parameter means the initial value of each dimension.
EuclideanVector::EuclideanVector(int i, double d) {
  this->magnitudes_ = std::make_unique<double[]>(i);
  this->array_size_ = i;
  for (auto iter = 0; iter < i; ++iter) {
    this->magnitudes_[iter] = d;
  }
}

// *****Copy Constructor*****
EuclideanVector::EuclideanVector(const EuclideanVector& ev1) {
  this->array_size_ = ev1.array_size_;
  this->magnitudes_ = std::make_unique<double[]>(ev1.array_size_);
  for (int iter = 0; iter < ev1.array_size_; ++iter) {
    this->magnitudes_[iter] = ev1.magnitudes_[iter];
  }
}

// This constructor copy the std vector's value into Euclidean vector,
// pass the begin and end of std vector then do the iteration to
// assign each value of the std vector's to Euclidean vector
EuclideanVector::EuclideanVector(std::vector<double>::const_iterator begin,
                                 std::vector<double>::const_iterator end) {
  std::vector<double> a(begin, end);
  this->array_size_ = a.size();
  this->magnitudes_ = std::make_unique<double[]>(a.size());
  for (int iter = 0; iter < this->array_size_; ++iter) {
    this->magnitudes_[iter] = a[iter];
  }
}

// =====================> friend functions <=====================

// 2 parts to check if equal equal or not:
// 1st check the dimensions of two Euclidean vector,
// if dimensions are same, execute the following codes, else return false;
// 2nd check two Euclidean vectors' sub-value for the same dimension,
// if all the dimensions' value are same return true, else return false.
bool operator==(const EuclideanVector& v1, const EuclideanVector& v2) {
  if (v1.GetNumDimensions() != v2.GetNumDimensions()) {
    return false;
  }
  for (int i = 0; i < v1.GetNumDimensions(); ++i) {
    if (v1[i] != v2[i]) {
      return false;
    }
  }
  return true;
}

// 2 parts to check if not equal or equal equal:
// 1st check the dimensions of two Euclidean vector,
// if dimensions are same, execute the following codes, else return true;
// 2nd check two Euclidean vectors' sub-values for the same dimension,
// if the sub-values of two Euclidean vectors are not equal return ture,
// else all the dimensions' value are same return false.
bool operator!=(const EuclideanVector& v1, const EuclideanVector& v2) {
  if (v1.GetNumDimensions() != v2.GetNumDimensions()) {
    return true;
  }
  for (int i = 0; i < v1.GetNumDimensions(); ++i) {
    if (v1[i] != v2[i]) {
      return true;
    }
  }
  return false;
}

// One Euclidean vector plus another Euclidean vector requires they are having same dimensions,
// if the dimensions are not same, throw exception, else execute the following codes.
// Cause the plus operation doesn't change the original Euclidean vectors,
// so we need use copy constructor to construct new Euclidean vector, then do the plus operation.
EuclideanVector operator+(const EuclideanVector& v1, const EuclideanVector& v2) {
  if (v1.GetNumDimensions() != v2.GetNumDimensions()) {
    std::string error = "Dimensions of LHS(";
    error += std::to_string(v1.GetNumDimensions());
    error += ") and RHS(";
    error += std::to_string(v2.GetNumDimensions());
    error += ") do not match";
    throw EuclideanVectorError(error);
  }
  EuclideanVector result{v1};
  result += v2;
  return result;
}

EuclideanVector operator-(const EuclideanVector& v1, const EuclideanVector& v2) {
  if (v1.GetNumDimensions() != v2.GetNumDimensions()) {
    //    std::string error = "Dimensions of LHS(X) and RHS(Y) do not match";
    std::string error = "Dimensions of LHS(";
    error += std::to_string(v1.GetNumDimensions());
    error += ") and RHS(";
    error += std::to_string(v2.GetNumDimensions());
    error += ") do not match";
    throw EuclideanVectorError(error);
  }
  EuclideanVector result{v1};
  result -= v2;
  return result;
}

// *****Multiplication*****
// double sum = ev1 * ev2;
// check the dimension first, if not equal throw exception
// then multiple each dimension value and add equal them into auto variable
double operator*(const EuclideanVector& v1, const EuclideanVector& v2) {
  if (v1.GetNumDimensions() != v2.GetNumDimensions()) {
    //    std::string error = "Dimensions of LHS(X) and RHS(Y) do not match";
    std::string error = "Dimensions of LHS(";
    error += std::to_string(v1.GetNumDimensions());
    error += ") and RHS(";
    error += std::to_string(v2.GetNumDimensions());
    error += ") do not match";
    throw EuclideanVectorError(error);
  }
  double sum = 0;
  for (int iter = 0; iter < v1.GetNumDimensions(); ++iter) {
    sum += (v1[iter] * v2[iter]);
  }
  return sum;
}

// *****Multiply*****
// EuclideanVector sumE = double * ev;
// Can't pollute the original Euclidean vector,
// so we need to use copy constructor to construct new Euclidean vector
EuclideanVector operator*(const EuclideanVector& v, const double& mult) {
  EuclideanVector result{v};
  for (int iter = 0; iter < v.GetNumDimensions(); ++iter) {
    result[iter] *= mult;
  }
  return result;
}

// *****Multiply*****
// EuclideanVector sumE = ev * double;
// Can't pollute the original Euclidean vector,
// so we need to use copy constructor to construct new Euclidean vector
EuclideanVector operator*(const double& mult, const EuclideanVector& v) {
  EuclideanVector result{v};
  for (int iter = 0; iter < v.GetNumDimensions(); ++iter) {
    result[iter] *= mult;
  }
  return result;
}

// *****Divide*****
// EuclideanVector ev1 = ev2 / double;
// the div shouldn't be zero, otherwise throw exception
// Can't pollute the original Euclidean vector,
// so we need to use copy constructor to construct new Euclidean vector
EuclideanVector operator/(const EuclideanVector& v, const double& div) {
  if (div == 0) {
    std::string error = "Invalid vector division by 0";
    throw EuclideanVectorError(error);
  }
  EuclideanVector result{v};
  for (int iter = 0; iter < v.GetNumDimensions(); ++iter) {
    result[iter] /= div;
  }
  return result;
}

// *****Output Stream*****
std::ostream& operator<<(std::ostream& os, const EuclideanVector& v) {
  if (v.array_size_ == 0) {
    os << "[]";
    return os;
  }
  os << "[";
  int iter = 0;
  for (; iter < v.GetNumDimensions() - 1; ++iter) {
    os << v[iter] << " ";
  }
  os << v[iter];
  os << "]";
  return os;
}
