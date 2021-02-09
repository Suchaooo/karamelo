/* -*- c++ -*- ----------------------------------------------------------*/


#ifdef TEMPERATURE_CLASS

TemperatureStyle(plastic_work,TemperaturePlasticWork)

#else

#ifndef MPM_TEMPERATURE_PLASTIC_WORK_H
#define MPM_TEMPERATURE_PLASTIC_WORK_H

#include "temperature.h"
#include <Eigen/Eigen>

class TemperaturePlasticWork : public Temperature {

public:
  TemperaturePlasticWork(class MPM *, vector<string>);
  ~TemperaturePlasticWork() {};

  inline double cp() { return cp_; }
  inline double kappa() { return kappa_; }
  void compute_heat_source(double &, const double &, const double &);
  double compute_thermal_pressure(double);

protected:
  double chi, kappa_, cp_, alpha, T0;
  string usage = "Usage: temperature(temp-ID, plastic_work, chi, cp, kappa, alpha, T0)\n";
  int Nargs = 7;
};

#endif
#endif
