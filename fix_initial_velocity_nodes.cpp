#include <iostream>
#include <vector>
#include <string>
#include <Eigen/Eigen>
#include "fix_initial_velocity_nodes.h"
#include "input.h"
#include "group.h"
#include "domain.h"
#include "update.h"
#include "error.h"

using namespace std;
using namespace FixConst;
using namespace Eigen;

FixInitialVelocityNodes::FixInitialVelocityNodes(MPM *mpm, vector<string> args) : Fix(mpm, args)
{
  if (domain->dimension == 3 && args.size()<6) {
    error->all(FLERR,"Error: too few arguments for fix_initial_velocity_nodes: requires at least 6 arguments. " + to_string(args.size()) + " received.\n");
  } else if (domain->dimension == 2 && args.size()<5) {
    error->all(FLERR,"Error: too few arguments for fix_initial_velocity_nodes: requires at least 5 arguments. " + to_string(args.size()) + " received.\n");
  } else if (domain->dimension == 1 && args.size()<4) {
    error->all(FLERR,"Error: too few arguments for fix_initial_velocity_nodes: requires at least 4 arguments. " + to_string(args.size()) + " received.\n");
  }

  if (group->pon[igroup].compare("nodes") !=0 && group->pon[igroup].compare("all") !=0) {
    error->all(FLERR,"fix_initial_velocity_nodes needs to be given a group of nodes" + group->pon[igroup] + ", " + args[2] + " is a group of " + group->pon[igroup] + ".\n");
  }
  cout << "Creating new fix FixInitialVelocityNodes with ID: " << args[0] << endl;
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

FixInitialVelocityNodes::~FixInitialVelocityNodes()
{
}

void FixInitialVelocityNodes::init()
{
}

void FixInitialVelocityNodes::setup()
{
}

void FixInitialVelocityNodes::setmask() {
  mask = 0;
  mask |= POST_UPDATE_GRID_STATE;
  mask |= POST_VELOCITIES_TO_GRID;
}


void FixInitialVelocityNodes::post_update_grid_state() {
  if (update->ntimestep !=1) return;
  // cout << "In FixInitialVelocityNodes::post_update_grid_state()" << endl;

  // Go through all the nodes in the group and set v_update to the right value:
  double vx, vy, vz;
  
  int solid = group->solid[igroup];

  vector<Eigen::Vector3d> *v_update;
  int nmax;
  int *mask;
  int n = 0;
  vector<Eigen::Vector3d> *x0;

  if (solid == -1) {
    for (int isolid = 0; isolid < domain->solids.size(); isolid++) {
      n = 0;
      v_update = &domain->solids[isolid]->grid->v_update;
      x0 = &domain->solids[isolid]->grid->x0;
      nmax = domain->solids[isolid]->grid->nnodes_local;
      mask = domain->solids[isolid]->grid->mask;

      for (int ip = 0; ip < nmax; ip++) {
	if (mask[ip] & groupbit) {
	  (*input->vars)["x"] = Var("x", (*x0)[ip][0]);
	  (*input->vars)["y"] = Var("y", (*x0)[ip][1]);
	  (*input->vars)["z"] = Var("z", (*x0)[ip][2]);
	  if (xset) {
	    vx = xvalue.result(mpm);
	    (*v_update)[ip][0] = vx;
	  }
	  if (yset) {
	    vy = yvalue.result(mpm);
	    (*v_update)[ip][1] = vy;
	  }
	  if (zset) {
	    vz = zvalue.result(mpm);
	    (*v_update)[ip][2] = vz;
	  }
	  n++;
	}
      }
      // cout << "(*v_update) for " << n << " nodes from solid " << domain->solids[isolid]->id << " set." << endl;
    }
  } else {

    v_update = &domain->solids[solid]->grid->v_update;
    x0 = &domain->solids[solid]->grid->x0;
    nmax = domain->solids[solid]->grid->nnodes_local;
    mask = domain->solids[solid]->grid->mask;

    for (int ip = 0; ip < nmax; ip++) {
      if (mask[ip] & groupbit) {
	(*input->vars)["x"] = Var("x", (*x0)[ip][0]);
	(*input->vars)["y"] = Var("y", (*x0)[ip][1]);
	(*input->vars)["z"] = Var("z", (*x0)[ip][2]);
	if (xset) {
	  vx = xvalue.result(mpm);
	  (*v_update)[ip][0] = vx;
	}
	if (yset) {
	  vy = yvalue.result(mpm);
	  (*v_update)[ip][1] = vy;
	}
	if (zset) {
	  vz = zvalue.result(mpm);
	  (*v_update)[ip][2] = vz;
	}
	n++;
      }
    }
    // cout << "(*v_update) for " << n << " nodes from solid " << domain->solids[solid]->id << " set." << endl;
  }
}

void FixInitialVelocityNodes::post_velocities_to_grid() {
  if (update->ntimestep !=1) return;
  // cout << "In FixInitialVelocityNodes::post_velocities_to_grid()" << endl;

  // Go through all the particles in the group and set v to the right value:
  double vx, vy, vz;
  
  int solid = group->solid[igroup];

  vector<Eigen::Vector3d> *v;
  int nmax;
  int *mask;
  int n = 0;
  vector<Eigen::Vector3d> *x0;

  if (solid == -1) {
    for (int isolid = 0; isolid < domain->solids.size(); isolid++) {
      n = 0;
      v = &domain->solids[isolid]->grid->v;
      x0 = &domain->solids[isolid]->grid->x0;
      nmax = domain->solids[isolid]->grid->nnodes_local;
      mask = domain->solids[isolid]->grid->mask;

      
      for (int ip = 0; ip < nmax; ip++) {
	if (mask[ip] & groupbit) {
	  (*input->vars)["x"] = Var("x", (*x0)[ip][0]);
	  (*input->vars)["y"] = Var("y", (*x0)[ip][1]);
	  (*input->vars)["z"] = Var("z", (*x0)[ip][2]);
	  if (xset) {
	    vx = xvalue.result(mpm);
	    (*v)[ip][0] = vx;
	  }
	  if (yset) {
	    vy = yvalue.result(mpm);
	    (*v)[ip][1] = vy;
	  }
	  if (zset) {
	    vz = zvalue.result(mpm);
	    (*v)[ip][2] = vz;
	  }
	  n++;
	}
      }
      // cout << "v for " << n << " nodes from solid " << domain->solids[isolid]->id << " set." << endl;
    }
  } else {

    v = &domain->solids[solid]->grid->v;
    x0 = &domain->solids[solid]->grid->x0;
    nmax = domain->solids[solid]->grid->nnodes_local;
    mask = domain->solids[solid]->grid->mask;

    for (int ip = 0; ip < nmax; ip++) {
      if (mask[ip] & groupbit) {
	(*input->vars)["x"] = Var("x", (*x0)[ip][0]);
	(*input->vars)["y"] = Var("y", (*x0)[ip][1]);
	(*input->vars)["z"] = Var("z", (*x0)[ip][2]);
	if (xset) {
	  vx = xvalue.result(mpm);
	  (*v)[ip][0] = vx;
	}
	if (yset) {
	  vy = yvalue.result(mpm);
	  (*v)[ip][1] = vy;
	}
	if (zset) {
	  vz = zvalue.result(mpm);
	  (*v)[ip][2] = vz;
	}
	n++;
      }
    }
    // cout << "v for " << n << " nodes from solid " << domain->solids[solid]->id << " set." << endl;
  }
}
