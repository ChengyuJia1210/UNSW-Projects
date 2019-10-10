#ifndef ASSIGNMENTS_EV_EUCLIDEAN_VECTOR_H_
#define ASSIGNMENTS_EV_EUCLIDEAN_VECTOR_H_

#include <cmath>
#include <exception>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class EuclideanVectorError : public std::exception {
 public:
  explicit EuclideanVectorError(const std::string& what) : what_(what) {}
  const char* what() const noexcept { return what_.c_str(); }

 private:
  std::string what_;
};

class EuclideanVector {
 public:
  // ========> constructor <========
  explicit EuclideanVector(int i = 1);
  EuclideanVector(int, double);
  EuclideanVector(std::vector<double>::const_iterator, std::vector<double>::const_iterator);
  EuclideanVector(const EuclideanVector&);
  EuclideanVector(EuclideanVector&& ev) noexcept : magnitudes_{std::move(ev.magnitudes_)},
                                                   array_size_{ev.array_size_} {
    ev.array_size_ = 0;
  }

  // ========> operators <========
  // copy assignment, may have bug, cause it already be allocated memory
  EuclideanVector operator=(const EuclideanVector& ev1) {
    this->array_size_ = ev1.array_size_;
    // release first, then allocate new memory
    this->magnitudes_.release();
    this->magnitudes_ = std::make_unique<double[]>(ev1.array_size_);
    for (auto iter = 0; iter < this->array_size_; ++iter) {
      this->magnitudes_[iter] = ev1.magnitudes_[iter];
    }
    //    std::cout << "copy assignment\n";
    return *this;
  }

  // move assignment
  EuclideanVector operator=(EuclideanVector&& ev) noexcept {
    this->magnitudes_ = std::move(ev.magnitudes_);
    this->array_size_ = ev.array_size_;
    // cause the ev's memory already pointed by this, so reset ev's array size to zero
    ev.array_size_ = 0;
    return *this;
  }

  // not assignable
  double operator[](int i) const {
    //    std::cout << "no assignable []\n";
    return this->magnitudes_[i];
  }

  // assignable
  double& operator[](int i) {
    //    std::cout << "assignable []\n";
    return this->magnitudes_[i];
  }

  // *****Addition*****
  // ev1 += ev2;
  // First check dimensions of two Euclidean Vectors,
  // if they are getting different dimensions, then throw exception
  // then do the addition
  EuclideanVector& operator+=(const EuclideanVector& ev2) {
    //    std::cout << "+=\n";
    if ((this->GetNumDimensions()) != ev2.GetNumDimensions()) {
      //      std::string error = "Dimensions of LHS(X) and RHS(Y) do not match";
      std::string error = "Dimensions of LHS(";
      error += std::to_string(this->GetNumDimensions());
      error += ") and RHS(";
      error += std::to_string(ev2.GetNumDimensions());
      error += ") do not match";
      throw EuclideanVectorError(error);
    }
    for (auto iter = 0; iter < this->array_size_; ++iter) {
      this->magnitudes_[iter] += ev2.magnitudes_[iter];
    }
    return *this;
  }

  // *****Subtraction*****
  // ev1 -= ev2;
  // First check dimensions of two Euclidean Vectors,
  // if they are getting different dimensions, then throw exception
  // then do the subtract operation
  EuclideanVector& operator-=(const EuclideanVector& ev2) {
    //    std::cout << "-=\n";
    if ((this->GetNumDimensions()) != ev2.GetNumDimensions()) {
      //      std::string error = "Dimensions of LHS(X) and RHS(Y) do not match";
      std::string error = "Dimensions of LHS(";
      error += std::to_string(this->GetNumDimensions());
      error += ") and RHS(";
      error += std::to_string(ev2.GetNumDimensions());
      error += ") do not match";
      throw EuclideanVectorError(error);
    }
    for (auto iter = 0; iter < this->array_size_; ++iter) {
      this->magnitudes_[iter] -= ev2.magnitudes_[iter];
    }
    return *this;
  }

  // *****Multiplication*****
  // ev *= double; => still return the EuclideanVector
  EuclideanVector operator*=(const double& mul) {
    for (auto iter = 0; iter < this->array_size_; ++iter) {
      this->magnitudes_[iter] *= mul;
    }
    return *this;
  }

  // *****Division*****
  // ev /= double; => still return the EuclideanVector
  EuclideanVector operator/=(const double& div) {
    if (div == 0) {
      std::string error = "Invalid vector division by 0";
      throw EuclideanVectorError(error);
    }
    for (auto iter = 0; iter < this->array_size_; ++iter) {
      this->magnitudes_[iter] /= div;
    }
    return *this;
  }

  // convert Euclidean Vector to normal std vector
  explicit operator std::vector<double>() {
    std::vector<double> converted;
    for (auto iter = 0; iter < this->array_size_; ++iter) {
      converted.push_back(this->magnitudes_[iter]);
    }
    return converted;
  }

  // convert Euclidean Vector to normal std list
  explicit operator std::list<double>() {
    std::list<double> converted;
    for (auto iter = 0; iter < this->array_size_; ++iter) {
      converted.push_back(this->magnitudes_[iter]);
    }
    return converted;
  }

  // ========> friends <========
  friend std::ostream& operator<<(std::ostream& os, const EuclideanVector& v);
  friend bool operator==(const EuclideanVector& v1, const EuclideanVector& v2);
  friend bool operator!=(const EuclideanVector& v1, const EuclideanVector& v2);
  friend EuclideanVector operator+(const EuclideanVector& v1, const EuclideanVector& v2);
  friend EuclideanVector operator-(const EuclideanVector& v1, const EuclideanVector& v2);
  friend double operator*(const EuclideanVector& v1, const EuclideanVector& v2);
  friend EuclideanVector operator*(const EuclideanVector& v, const double& mult);
  friend EuclideanVector operator*(const double& mult, const EuclideanVector& v);
  friend EuclideanVector operator/(const EuclideanVector& v, const double& div);

  // ========> methods <========
  // const at()
  // First check the the index if valid or not, if not throw exception
  double at(int i) const {
    if (i < 0 || i >= this->array_size_) {
      //      std::string error = "Index X is not valid for this EuclideanVector object";
      std::string error = "Index ";
      error += std::to_string(i);
      error += " is not valid for this EuclideanVector object";
      throw EuclideanVectorError(error);
    }
    return this->magnitudes_[i];
  }

  // assignable at()
  // First check the the index if valid or not, if not throw exception
  double& at(int i) {
    if (i < 0 || i >= this->array_size_) {
      //      std::string error = "Index X is not valid for this EuclideanVector object";
      std::string error = "Index ";
      error += std::to_string(i);
      error += " is not valid for this EuclideanVector object";
      throw EuclideanVectorError(error);
    }
    return this->magnitudes_[i];
  }

  int GetNumDimensions() const { return this->array_size_; }

  // Cause EuclideanNorm needs the vector not empty
  // so if vector is empty then throw exception
  double GetEuclideanNorm() {
    if (this->GetNumDimensions() == 0) {
      std::string error = "EuclideanVector with no dimensions does not have a norm";
      throw EuclideanVectorError(error);
    }
    double norm = 0;
    for (int iter = 0; iter < this->array_size_; ++iter) {
      norm += this->magnitudes_[iter] * this->magnitudes_[iter];
    }
    return sqrt(norm);
  }

  // Cause UnitVector needs the EuclideanNorm, so the vector shouldn't be empty
  // so if vector is empty then throw exception
  // also, the EuclideanNorm shouldn't be zero, otherwise throw exception as well
  EuclideanVector CreateUnitVector() {
    if (this->GetNumDimensions() == 0) {
      std::string error1 = "EuclideanVector with no dimensions does not have a unit vector";
      throw EuclideanVectorError(error1);
    }
    if (this->GetEuclideanNorm() == 0) {
      std::string error2 = "EuclideanVector with euclidean normal of 0 does not have a unit vector";
      throw EuclideanVectorError(error2);
    }
    EuclideanVector unit_vector{*this};
    double norm = this->GetEuclideanNorm();
    for (int iter = 0; iter < unit_vector.array_size_; ++iter) {
      unit_vector.magnitudes_[iter] /= norm;
    }
    return unit_vector;
  }

  // ========> Destructor <========
  ~EuclideanVector() = default;

 private:
  std::unique_ptr<double[]> magnitudes_;
  int array_size_;  // size of this Euclidean Vector
};

#endif  // ASSIGNMENTS_EV_EUCLIDEAN_VECTOR_H_
