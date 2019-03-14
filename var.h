/* -*- c++ -*- ----------------------------------------------------------*/

#ifndef MPM_VAR_H
#define MPM_VAR_H

#include <vector>

class Var{
 public:
  // functions
  Var() {};
  Var(double);
  Var(string, double, bool c = false);
  ~Var();

  void evaluate(class MPM * = NULL);
  double result(class MPM *);
  double result() const {return value;};
  string str() const;
  string eq() const {return equation;};
  bool is_constant() const {return constant;};
  Var operator+(const Var&);
  Var operator-(const Var&);
  Var operator-();
  Var operator*(const Var&);
  Var operator/(const Var&);
  Var operator^(const Var&);
  //Var operator=(const Var&);
  operator int() {return (int) value;};

protected:
  string equation;             // formula
  double value;                // current value
  bool constant;               // is the variables constant?
};


Var powv(int, Var);            // power function
Var expv(Var);                 // exponential function

#endif