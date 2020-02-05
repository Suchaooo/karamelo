/* -*- c++ -*- ----------------------------------------------------------
 *
 *                    ***       Karamelo       ***
 *               Parallel Material Point Method Simulator
 * 
 * Copyright (2019) Alban de Vaucorbeil, alban.devaucorbeil@monash.edu
 * Materials Science and Engineering, Monash University
 * Clayton VIC 3800, Australia

 * This software is distributed under the GNU General Public License.
 *
 * ----------------------------------------------------------------------- */

#ifndef MPM_UPDATE_H
#define MPM_UPDATE_H

#include "pointers.h"
#include <vector>

class Update : protected Pointers {
 public:
  double run_duration;            // GCG stop simulation if elapsed simulation time exceeds this.
  double elapsed_time_in_run;	  // elapsed simulation time for a single run;
  double dt;                      // timestep
  double dt_factor;               // timestep factor
  bool dt_constant;               // is dt constant?
  bigint ntimestep;               // current step
  int nsteps;                     // # of steps to run
  double atime;                   // simulation time at atime_step
  double maxtime;                 // maximum simulation time (infinite if -1)
  bigint atimestep;               // last timestep atime was updated
  bigint firststep,laststep;      // 1st & last step of this run
  bigint beginstep,endstep;       // 1st and last step of multiple runs
  int first_update;               // 0 before initial update, 1 after

  class Scheme *scheme;
  string scheme_style;

  class Method *method;
  string method_type;
  string method_shape_function;

  Update(class MPM *);
  ~Update();
  void set_dt_factor(vector<string>);
  void set_dt(vector<string>);
  void create_scheme(vector<string>);
  void new_scheme(vector<string>);
  void create_method(vector<string>);
  void new_method(vector<string>);
  void update_time();
  int update_timestep();
protected:
  
};


#endif