#include <iostream>
#include <vector>
#include <string>
#include <Eigen/Eigen>
#include "fix_check_solution.h"
#include "input.h"
#include "group.h"
#include "domain.h"
#include "input.h"
#include "update.h"
#include "output.h"
#include "math_special.h"
#include "universe.h"
#include "error.h"

using namespace std;
using namespace FixConst;
using namespace MathSpecial;
using namespace Eigen;

FixChecksolution::FixChecksolution(MPM *mpm, vector<string> args) : Fix(mpm, args)
{
  if (domain->dimension == 3 && args.size()<6) {
    error->all(FLERR,"Error: too few arguments for fix_check_solution: requires at least 6 arguments. " + to_string(args.size()) + " received.\n");
  } else if (domain->dimension == 2 && args.size()<5) {
    error->all(FLERR,"Error: too few arguments for fix_check_solution: requires at least 5 arguments. " + to_string(args.size()) + " received.\n");
  } else if (domain->dimension == 1 && args.size()<4) {
    error->all(FLERR,"Error: too few arguments for fix_check_solution: requires at least 4 arguments. " + to_string(args.size()) + " received.\n");
  }

  if (group->pon[igroup].compare("nodes") !=0 && group->pon[igroup].compare("all") !=0) {
    error->all(FLERR,"_check_solution needs to be given a group of nodes" + group->pon[igroup] + ", " + args[2] + " is a group of " + group->pon[igroup] + ".\n");
  }
  cout << "Creating new fix FixChecksolution with ID: " << args[0] << endl;
  id = args[0];

  xset = yset = zset = false;

  if (args[3].compare("NULL") != 0) {
    xvalue = input->parsev(args[3]);
    xset = true;
  }

  if (domain->dimension >= 2) {
    if (args[4].compare("NULL") != 0) {
      yvalue = input->parsev(args[4]);
      yset = true;
    }
  }

  if (domain->dimension == 3) {
    if (args[5].compare("NULL") != 0) {
      zvalue = input->parsev(args[5]);
      zset = true;
    }
  }
}

FixChecksolution::~FixChecksolution()
{
}

void FixChecksolution::init()
{
}

void FixChecksolution::setup()
{
}

void FixChecksolution::setmask() {
  mask = 0;
  mask |= FINAL_INTEGRATE;
}


void FixChecksolution::final_integrate() {
  if (update->ntimestep != output->next && update->ntimestep != update->nsteps) return;
  // cout << "In FixChecksolution::post_particles_to_grid()\n";

  // Go through all the nodes in the group and set b to the right value:
  double ux, uy, uz;
    
  int solid = group->solid[igroup];

  int nmax;
  int *mask;
  double *mass;
  double *vol0;
  Eigen::Vector3d error, error_reduced;
  Eigen::Vector3d u_th;
  vector<Eigen::Vector3d> *x0;
  vector<Eigen::Vector3d> *x;

  error.setZero();
  u_th.setZero();

  double vtot;

  if (solid == -1) {
    vtot = 0;
    for (int isolid = 0; isolid < domain->solids.size(); isolid++) {
      vtot += domain->solids[isolid]->vtot;
      x0 = &domain->solids[isolid]->x0;
      x = &domain->solids[isolid]->x;
      vol0 = domain->solids[isolid]->vol0;
      nmax = domain->solids[isolid]->np_local;
      mask = domain->solids[isolid]->mask;

      for (int in = 0; in < nmax; in++) {
	if (mask[in] & groupbit) {
	  if (xset) {
	    (*input->vars)["x"] = Var("x", (*x0)[in][0]);
	    ux = xvalue.result(mpm);
	    error[0] += vol0[in]*square(ux-((*x)[in][0]-(*x0)[in][0]));
	    u_th[0] += vol0[in]*ux*ux;
	  }
	  if (yset) {
	    (*input->vars)["y"] = Var("y", (*x0)[in][1]);
	    uy = yvalue.result(mpm);
	    error[1] += vol0[in]*square(uy-((*x)[in][1]-(*x0)[in][1]));
	    u_th[1] += vol0[in]*uy*uy;
	  }
	  if (zset) {
	    (*input->vars)["z"] = Var("z", (*x0)[in][2]);
	    uz = zvalue.result(mpm);
	    error[2] += vol0[in]*square(uz-((*x)[in][2]-(*x0)[in][2]));
	    u_th[2] += vol0[in]*uz*uz;
	  }
	}
      }
    }
  } else {
    vtot = domain->solids[solid]->vtot;
    x0 = &domain->solids[solid]->x0;
    x = &domain->solids[solid]->x;
    vol0 = domain->solids[solid]->vol0;
    nmax = domain->solids[solid]->np_local;
    mask = domain->solids[solid]->mask;

    for (int in = 0; in < nmax; in++) {
      if (mask[in] & groupbit) {
	if (xset) {
	  (*input->vars)["x"] = Var("x", (*x0)[in][0]);
	  ux = xvalue.result(mpm);
	  error[0] += vol0[in]*square(ux-((*x)[in][0]-(*x0)[in][0]));
	  u_th[0] += vol0[in]*ux*ux;
	}
	if (yset) {
	  (*input->vars)["y"] = Var("y", (*x0)[in][1]);
	  uy = yvalue.result(mpm);
	  error[1] += vol0[in]*square(uy-((*x)[in][1]-(*x0)[in][1]));
	  u_th[1] += vol0[in]*uy*uy;
	}
	if (zset) {
	  (*input->vars)["z"] = Var("z", (*x0)[in][2]);
	  uz = zvalue.result(mpm);
	  error[2] += vol0[in]*square(uz-((*x)[in][2]-(*x0)[in][2]));
	  u_th[2] += vol0[in]*uz*uz;
	}
      }
    }

  }

  // Reduce error:
  MPI_Allreduce(error.data(),error_reduced.data(),3,MPI_DOUBLE,MPI_SUM,universe->uworld);

  (*input->vars)[id+"_s"]=Var(id+"_s", sqrt((error_reduced[0] + error_reduced[1] + error_reduced[2])/vtot));
  (*input->vars)[id+"_x"]=Var(id+"_x", (*input->vars)[id+"_x"].result() + update->dt*(error_reduced[0] + error_reduced[1] + error_reduced[2]));
  (*input->vars)[id+"_y"]=Var(id+"_y", (*input->vars)[id+"_y"].result() + update->dt*(u_th[0] + u_th[1] + u_th[2]));
  (*input->vars)[id+"_z"]=Var(id+"_z", sqrt((*input->vars)[id+"_x"].result()/(*input->vars)[id+"_y"].result()));
    // cout << "f for " << n << " nodes from solid " << domain->solids[solid]->id << " set." << endl;
  // cout << "ftot = [" << ftot[0] << ", " << ftot[1] << ", " << ftot[2] << "]\n"; 
}
