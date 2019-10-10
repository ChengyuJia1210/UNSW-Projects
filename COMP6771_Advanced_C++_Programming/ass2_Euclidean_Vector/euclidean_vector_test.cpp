/*

  == Explanation and rational of testing ==
  In this test file, it covers all the constructors testing, operators testing, friends testing and
  functional methods testing.
  First we test the constructors, cause when create the object the first thing is using constructor
  to create that object.
  Then the operators are tested, cause the operators are the inner defined functions. And they
  overide the original operators, so we need to test their correctness.
  Third, the friends are be tested. Cause the friend can touch the private elements, so we need
  treat them carefully.
  Final, we test the functional methods, to test their correctness.
  In the testing we try to test every funtion that may throw the exception to make sure they throw
  the right exception information.
*/

#include "assignments/ev/euclidean_vector.h"
#include <string>
#include "catch.h"


// =====================> Constructor Testing <=====================
// Default Constructor
SCENARIO("Constructor testing with Default Constructor") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with the one parameter 10 to construct an object") {
      EuclideanVector ev(10);
      THEN("Should be [0 0 0 0 0 0 0 0 0 0]") {
        std::stringstream ss1;
        ss1 << ev;
        REQUIRE("[0 0 0 0 0 0 0 0 0 0]" == ss1.str());
      }
    }

    WHEN("A constructor with the none parameter to construct an object") {
      EuclideanVector ev2;
      THEN("Should be [0]") {
        std::stringstream ss2;
        ss2 << ev2;
        REQUIRE("[0]" == ss2.str());
      }
    }
  }
}

// Construct: EuclideanVector(int, double);
SCENARIO("Constructor with two parameters, one for int type and one for double type") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 6.6 for "
         "value of each dimension to construct an object") {
      EuclideanVector ev = EuclideanVector(5, 6.6);
      THEN("Should be [6.6 6.6 6.6 6.6 6.6]") {
        std::stringstream ss;
        ss << ev;
        REQUIRE("[6.6 6.6 6.6 6.6 6.6]" == ss.str());
      }
    }
  }
}

SCENARIO("Constructor with vector's begin() and end() iterators") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with the start and end of vector {2,4,6,8,10} to construct an object") {
      std::vector<double> test_vector{2, 4, 6, 8, 10};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      THEN("Should be [2 4 6 8 10]") {
        std::stringstream ss;
        ss << ev;
        REQUIRE("[2 4 6 8 10]" == ss.str());
      }
    }
  }
}

// Copy Constructor
SCENARIO("Copy Constructor") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with the start and end of vector {7, 6, 5, 4, 3, 2, 1} to construct an "
         "object") {
      std::vector<double> test_vector{7, 6, 5, 4, 3, 2, 1};
      EuclideanVector ev1(test_vector.begin(), test_vector.end());
      EuclideanVector ev2 = ev1;
      THEN("Then ev2 should print: [7 6 5 4 3 2 1]") {
        std::stringstream ss1;
        ss1 << ev2;
        REQUIRE("[7 6 5 4 3 2 1]" == ss1.str());
      }
      THEN("Then ev1 should print: [7 6 5 4 3 2 1] as well") {
        std::stringstream ss2;
        ss2 << ev1;
        REQUIRE("[7 6 5 4 3 2 1]" == ss2.str());
      }
    }
  }
}

// Move Constructor
SCENARIO("Move Constructor testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with the start and end of vector {12, 14, 16, 18, 110} to construct an "
         "object") {
      std::vector<double> test_vector{12, 14, 16, 18, 110};
      EuclideanVector ev1(test_vector.begin(), test_vector.end());
      EuclideanVector ev2{std::move(ev1)};
      THEN("ev2 should print: [12 14 16 18 110]") {  // and ev1 should be point to null
        std::stringstream ss1;
        ss1 << ev2;
        REQUIRE("[12 14 16 18 110]" == ss1.str());
      }
      THEN("ev1 should print: []") {
        std::stringstream ss2;
        ss2 << ev1;
        REQUIRE("[]" == ss2.str());
      }
    }
  }
}

// =====================> Operators Testing <=====================
// Copy Assignment
SCENARIO("Copy Assignment operator testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor take default initialization to construct an object") {
      EuclideanVector ev1(5, 3);
      EuclideanVector ev2;
      ev2 = ev1;
      THEN("ev2 should print: [3 3 3 3 3]") {
        std::stringstream ss1;
        ss1 << ev2;
        REQUIRE("[3 3 3 3 3]" == ss1.str());
      }
      THEN("ev1 should print: [3 3 3 3 3] as well") {
        std::stringstream ss2;
        ss2 << ev1;
        REQUIRE("[3 3 3 3 3]" == ss2.str());
      }
    }
  }
}

// Move Assignment
SCENARIO("Move Assignment operator testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 7 for size of vector and another is 2 for value "
         "of each dimension to construct an object;"
         "and an other constructor take default initialization to construct an object") {
      EuclideanVector ev1(7, 2);
      EuclideanVector ev2;
      ev2 = std::move(ev1);
      THEN("ev2 should print: [2 2 2 2 2 2 2]") {
        std::stringstream ss1;
        ss1 << ev2;
        REQUIRE("[2 2 2 2 2 2 2]" == ss1.str());
      }
      THEN("ev1 should print: []") {
        std::stringstream ss2;
        ss2 << ev1;
        REQUIRE("[]" == ss2.str());
      }
    }
  }
}

// Subscript
SCENARIO("Subscript operator testing, will test the assignable and non-assignable subscript") {
  GIVEN("EuclideanVector Class") {
    // assignable subscript test
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object") {
      EuclideanVector ev1(5, 3);
      ev1[2] = 5.5;
      ev1[3] = 7.3;
      THEN("ev1 should print: [3 3 5.5 7.3 3]") {
        std::stringstream ss1;
        ss1 << ev1;
        REQUIRE("[3 3 5.5 7.3 3]" == ss1.str());
      }
    }
    // non-assignable subscript test
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object") {
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      const EuclideanVector ev2(test_vector.begin(), test_vector.end());
      THEN("ev2[1] should print: 3") {
        std::stringstream ss1;
        ss1 << ev2[1];
        REQUIRE("3" == ss1.str());
      }
    }
  }
}

// Addition
SCENARIO("Addition operator testing, will test two Euclidean vector add together, and the "
         "exception scenario") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5, 6} to construct an object") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      ev2 += ev1;
      THEN("ev2 should print: [5 6 7 8 9]") {
        std::stringstream ss1;
        ss1 << ev2;
        REQUIRE("[5 6 7 8 9]" == ss1.str());
      }
    }
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5} to construct an object,"
         "cause two Euclidean vector have different dimensions, so the exception will be throw") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      THEN("An exception: 'Dimensions of LHS(X) and RHS(Y) do not match' will be throw") {
        try {
          ev2 += ev1;
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("Dimensions of LHS(4) and RHS(5) do not match" == ss2.str());
        }
      }
    }
  }
}

// Subtraction
SCENARIO("Subtraction operator testing, will test two Euclidean vector minus an other Euclidean "
         "vector, and the exception scenario") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5, 6} to construct an object") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      ev2 -= ev1;
      THEN("ev2 should print: [-1 0 1 2 3]") {
        std::stringstream ss1;
        ss1 << ev2;
        REQUIRE("[-1 0 1 2 3]" == ss1.str());
      }
    }
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5} to construct an object,"
         "cause two Euclidean vector have different dimensions, so the exception will be throw") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      THEN("An exception: 'Dimensions of LHS(X) and RHS(Y) do not match' will be throw") {
        try {
          ev2 -= ev1;
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("Dimensions of LHS(4) and RHS(5) do not match" == ss2.str());
        }
      }
    }
  }
}

// Multiplication
SCENARIO("Multiplication operator testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 7 for size of vector and another is 2 for value "
         "of each dimension to construct an object;"
         "and an other multiple element define as 5") {
      EuclideanVector ev1(7, 2);
      const double mul = 5;
      ev1 *= mul;
      THEN("ev1 should print: [10 10 10 10 10 10 10]") {
        std::stringstream ss1;
        ss1 << ev1;
        REQUIRE("[10 10 10 10 10 10 10]" == ss1.str());
      }
    }
  }
}

// Division
SCENARIO("Division operator testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {2, 3, 4, 5, 6} to construct an object, and division "
         "element will be double type 2") {
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      const double div = 2;
      ev /= div;
      THEN("ev2 should print: [1 1.5 2 2.5 3]") {
        std::stringstream ss1;
        ss1 << ev;
        REQUIRE("[1 1.5 2 2.5 3]" == ss1.str());
      }
    }
    WHEN("A constructor use std::vector {2, 3, 4, 5} to construct an object, and the division "
         "element will be 0"
         "cause the division element can't be zero, so the exception will be throw") {
      std::vector<double> test_vector{2, 3, 4, 5};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      const double div2 = 0;
      THEN("An exception: Invalid vector division by 0' will be throw") {
        try {
          ev2 /= div2;
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("Invalid vector division by 0" == ss2.str());
        }
      }
    }
  }
}

// Vector Type Conversion
SCENARIO("Vector Type Conversion operation testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 7 for size of vector and another is 2 for value "
         "of each dimension to construct an object;"
         "and an other multiple element define as 5") {
      EuclideanVector ev1(7, 2);
      auto vf = std::vector<double>{ev1};
      THEN("type of vf should print: St6vectorIdSaIdEE") {
        std::stringstream ss1;
        ss1 << typeid(vf).name();
        REQUIRE("St6vectorIdSaIdEE" == ss1.str());
      }
    }
  }
}

// List Type Conversion
SCENARIO("List Type Conversion operation testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 7 for size of vector and another is 2 for value "
         "of each dimension to construct an object;"
         "and an other multiple element define as 5") {
      EuclideanVector ev1(7, 2);
      auto lf = std::list<double>{ev1};
      THEN("type of vf should print: NSt7__cxx114listIdSaIdEEE") {
        std::stringstream ss1;
        ss1 << typeid(lf).name();
        REQUIRE("NSt7__cxx114listIdSaIdEEE" == ss1.str());
      }
    }
  }
}

// =====================> Friends Testing <=====================
// Output Stream
SCENARIO("Friend Output Stream testing, will test an Euclidean vector output") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {111, 15, 77, 60, 35,47, 89} to construct an object"
         "and an other element is the double type 2") {
      std::vector<double> test_vector{111, 15, 77, 60, 35, 47, 89};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      THEN("value should print: [111 15 77 60 35 47 89]") {
        std::stringstream ss;
        ss << ev;
        REQUIRE("[111 15 77 60 35 47 89]" == ss.str());
      }
    }
  }
}

// Equal a == b;
SCENARIO("Friend Equal testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {5,2,11,7,9} to construct an object, and use copy "
         "constructor to construct an other Euclidean vector") {
      std::vector<double> test_vector{5, 2, 11, 7, 9};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      EuclideanVector ev2{ev};
      bool state = (ev == ev2);
      THEN("The return value would be: true") {
        std::stringstream ss;
        ss << state;
        REQUIRE("1" == ss.str());
      }
    }
    WHEN("A constructor use std::vector {5,2,11,7,9} to construct an object, and construct an "
         "other element by use default constructor EuclideanVector ev2{5}") {
      std::vector<double> test_vector{5, 2, 11, 7, 9};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      EuclideanVector ev2{5};
      bool state = (ev == ev2);
      THEN("The return value would be: true") {
        std::stringstream ss2;
        ss2 << state;
        REQUIRE("0" == ss2.str());
      }
    }
  }
}

// Not Equal a != b;
SCENARIO("Friend Not Equal testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {5,2,11,7,9} to construct an object, and use copy "
         "constructor to construct an other Euclidean vector") {
      std::vector<double> test_vector{5, 2, 11, 7, 9};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      EuclideanVector ev2{ev};
      bool state = (ev != ev2);
      THEN("The return value would be: true") {
        std::stringstream ss;
        ss << state;
        REQUIRE("0" == ss.str());
      }
    }
    WHEN("A constructor use std::vector {5,2,11,7,9} to construct an object, and construct an "
         "other element by use default constructor EuclideanVector ev2{5}") {
      std::vector<double> test_vector{5, 2, 11, 7, 9};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      EuclideanVector ev2{5};
      bool state = (ev != ev2);
      THEN("The return value would be: true") {
        std::stringstream ss2;
        ss2 << state;
        REQUIRE("1" == ss2.str());
      }
    }
  }
}

// Addition a = bool+ c;
SCENARIO("Friend Addition testing, will test two Euclidean vector add together, and the exception "
         "scenario") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5, 6} to construct an object") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      EuclideanVector ev;
      ev = ev1 + ev2;
      THEN("ev should print: [5 6 7 8 9]") {
        std::stringstream ss1;
        ss1 << ev;
        REQUIRE("[5 6 7 8 9]" == ss1.str());
      }
    }
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5} to construct an object,"
         "cause two Euclidean vector have different dimensions, so the exception will be throw") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      EuclideanVector ev;
      THEN("An exception: 'Dimensions of LHS(X) and RHS(Y) do not match' will be throw") {
        try {
          ev = ev1 + ev2;
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("Dimensions of LHS(5) and RHS(4) do not match" == ss2.str());
        }
      }
    }
  }
}

// Subtraction a = b - c;
SCENARIO("Friend Subtraction testing, will test two Euclidean vectors' each element minus with "
         "each other, and the exception scenario") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5, 6} to construct an object") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      EuclideanVector ev;
      ev = ev1 - ev2;
      THEN("ev should print: [1 0 -1 -2 -3]") {
        std::stringstream ss1;
        ss1 << ev;
        REQUIRE("[1 0 -1 -2 -3]" == ss1.str());
      }
    }
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5} to construct an object,"
         "cause two Euclidean vector have different dimensions, so the exception will be throw") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      EuclideanVector ev;
      THEN("An exception: 'Dimensions of LHS(X) and RHS(Y) do not match' will be throw") {
        try {
          ev = ev1 - ev2;
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("Dimensions of LHS(5) and RHS(4) do not match" == ss2.str());
        }
      }
    }
  }
}

// Multiplication double c {ev1 * ev2}
SCENARIO("Friend Multiplication testing, will test two Euclidean vectors multiple with each other, "
         "and the exception scenario") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5, 6} to construct an object") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      double value{ev1 * ev2};
      THEN("value should print: 66") {
        std::stringstream ss1;
        ss1 << value;
        REQUIRE("60" == ss1.str());
      }
    }
    WHEN("A constructor with two paramters, one is 5 for size of vector and another is 3 for value "
         "of each dimension to construct an object;"
         "and an other constructor use std::vector {2, 3, 4, 5} to construct an object"
         "cause two Euclidean vector have different dimensions, so the exception will be throw") {
      EuclideanVector ev1(5, 3);
      std::vector<double> test_vector{2, 3, 4, 5};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      THEN("An exception: 'Dimensions of LHS(X) and RHS(Y) do not match' will be throw") {
        try {
          ev1* ev2;
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("Dimensions of LHS(5) and RHS(4) do not match" == ss2.str());
        }
      }
    }
  }
}

// Multiply
SCENARIO("Friend Multiply testing, two normal senario: 1. ev = ev1 * double, 2. ev = 3 * ev2;"
         "will test two Euclidean vectors multiple with each other, and the exception scenario") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {2, 3, 4, 5} to construct an object"
         "and an other element is the double type 10") {
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev1(test_vector.begin(), test_vector.end());
      double d = 10;
      EuclideanVector ev = ev1 * d;
      THEN("value should print: [20 30 40 50 60]") {
        std::stringstream ss1;
        ss1 << ev;
        REQUIRE("[20 30 40 50 60]" == ss1.str());
      }
    }
    WHEN("A constructor use std::vector {7, 3, 5, 2, 1} to construct an object"
         "and an other element is the double type 10") {
      std::vector<double> test_vector{7, 3, 5, 2, 1};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      double d = 10;
      EuclideanVector ev = d * ev2;
      THEN("value should print: [70 30 50 20 10]") {
        std::stringstream ss1;
        ss1 << ev;
        REQUIRE("[70 30 50 20 10]" == ss1.str());
      }
    }
  }
}

// Divide
// double c;
// EuclideanVector a = b / c;
SCENARIO("Friend Divide testing, will test an Euclidean vector divide a double variable, and the "
         "exception scenario") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {2, 3, 4, 5, 6} to construct an object"
         "and an other element is the double type 2") {
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev1(test_vector.begin(), test_vector.end());
      double d = 2;
      EuclideanVector ev = ev1 / d;
      THEN("value should print: [1 1.5 2 2.5 3]") {
        std::stringstream ss1;
        ss1 << ev;
        REQUIRE("[1 1.5 2 2.5 3]" == ss1.str());
      }
    }
    WHEN("A constructor use std::vector {2, 3, 4, 5} to construct an object, and an other element "
         "is the double type 0"
         "cause the divisor can not be zero, so the exception will be throw") {
      std::vector<double> test_vector{2, 3, 4, 5};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      double d = 0;

      THEN("An exception: 'Invalid vector division by 0' will be throw") {
        try {
          EuclideanVector ev = ev2 / d;
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("Invalid vector division by 0" == ss2.str());
        }
      }
    }
  }
}

// =====================> Methods Testing <=====================
// double at(int) const
SCENARIO("Method at() return const value testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {2, 3, 4, 5, 6} to construct an object, and want to get "
         "the third element value") {
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      const EuclideanVector ev(test_vector.begin(), test_vector.end());
      const double value = ev.at(2);
      THEN("value should print: 4") {
        std::stringstream ss1;
        ss1 << value;
        REQUIRE("4" == ss1.str());
      }
    }
    WHEN("A constructor use std::vector {2, 3, 4, 5} to construct an object,"
         "cause the this Euclidean Vector object has 4 element, want to get the index 6th's "
         "element, so the exception will be throw") {
      std::vector<double> test_vector{2, 3, 4, 5};
      const EuclideanVector ev2(test_vector.begin(), test_vector.end());
      THEN("An exception: Index 6 is not valid for this EuclideanVector object") {
        try {
          ev2.at(6);
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("Index 6 is not valid for this EuclideanVector object" == ss2.str());
        }
      }
    }
  }
}

// double& at(int)
SCENARIO("Method at() return reference of the Euclidean vector testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {2, 3, 4, 5, 6} to construct an object, and modified the "
         "3rd element") {
      std::vector<double> test_vector{2, 3, 4, 5, 6};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      ev.at(2) = 75;
      THEN("ev.at(2) should print: 75") {
        std::stringstream ss1;
        ss1 << ev.at(2);
        REQUIRE("75" == ss1.str());
      }
      THEN("ev[2] should print: 75") {
        std::stringstream ss2;
        ss2 << ev[2];
        REQUIRE("75" == ss2.str());
      }
    }
    WHEN("A constructor use std::vector {2, 3, 4, 5} to construct an object,"
         "cause the this Euclidean Vector object has 4 element, want to get the index 6th's "
         "element, so the exception will be throw") {
      std::vector<double> test_vector{2, 3, 4, 5};
      EuclideanVector ev2(test_vector.begin(), test_vector.end());
      THEN("An exception: Index 6 is not valid for this EuclideanVector object") {
        try {
          ev2.at(6);
        } catch (EuclideanVectorError& e) {
          std::stringstream ss3;
          ss3 << e.what();
          REQUIRE("Index 6 is not valid for this EuclideanVector object" == ss3.str());
        }
      }
    }
  }
}

// GetNumDimensions() Method
SCENARIO("GetNumDimensions() Method testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {5,2,11,7,9} to construct an object") {
      std::vector<double> test_vector{5, 2, 11, 7, 9};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      THEN("The return value would be: 5") {
        std::stringstream ss;
        ss << ev.GetNumDimensions();
        REQUIRE("5" == ss.str());
      }
    }
  }
}

// GetEuclideanNorm() Method
SCENARIO("GetEuclideanNorm() Method testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {5,2,11,7,9} to construct an object, then try to calculate "
         "its norm") {
      std::vector<double> test_vector{5, 2, 11, 7, 9};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      THEN("The return value would be: 16.7332") {
        std::stringstream ss;
        ss << ev.GetEuclideanNorm();
        REQUIRE("16.7332" == ss.str());
      }
    }
    WHEN("A constructor use std::vector {} to construct an object, then try to calculate its norm,"
         "cause the Euclidean Vector getting empty, so the dimension should be zero, so the "
         "exception will be throw") {
      std::vector<double> test_vector{};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      THEN("The return value would be: EuclideanVector with no dimensions does not have a norm") {
        try {
          ev.GetEuclideanNorm();
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("EuclideanVector with no dimensions does not have a norm" == ss2.str());
        }
      }
    }
  }
}

// CreateUnitVector() Method
SCENARIO("CreateUnitVector() Method testing") {
  GIVEN("EuclideanVector Class") {
    WHEN("A constructor use std::vector {5,2,5,5,9} to construct an object, then try to calculate "
         "its united Euclidean Vector") {
      std::vector<double> test_vector{5, 2, 5, 5, 9};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      THEN("The return value would be: ") {  // 12.6491
        std::stringstream ss;
        ss << ev.CreateUnitVector();
        REQUIRE("[0.395285 0.158114 0.395285 0.395285 0.711512]" == ss.str());
      }
    }
    WHEN("A constructor use std::vector {} to construct an object, then try to calculate its unit "
         "vector,"
         "cause the Euclidean Vector getting empty, so the dimension should be zero, so the "
         "exception will be throw") {
      std::vector<double> test_vector{};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      THEN("The return value would be: EuclideanVector with no dimensions does not have a unit "
           "vector") {
        try {
          ev.CreateUnitVector();
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("EuclideanVector with no dimensions does not have a unit vector" == ss2.str());
        }
      }
    }
    WHEN("A constructor use std::vector {0, 0, 0} to construct an object, then try to calculate "
         "its unit vector, the the norm will be zero"
         "cause the unit need every element divide the norm, and the norm is zero, so the "
         "exception will be throw") {
      std::vector<double> test_vector{0, 0, 0};
      EuclideanVector ev(test_vector.begin(), test_vector.end());
      THEN("The return value would be: EuclideanVector with euclidean normal of 0 does not have a "
           "unit vector") {
        try {
          ev.CreateUnitVector();
        } catch (EuclideanVectorError& e) {
          std::stringstream ss2;
          ss2 << e.what();
          REQUIRE("EuclideanVector with euclidean normal of 0 does not have a unit vector" ==
                  ss2.str());
        }
      }
    }
  }
}
