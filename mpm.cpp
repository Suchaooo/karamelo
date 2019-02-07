#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpm.h"
#include "domain.h"
#include "material.h"
#include "input.h"
#include "output.h"
#include "update.h"
#include "modify.h"
#include "memory.h"
#include "group.h"

MPM::MPM(int narg, char **arg)
{
  memory = new Memory(this);
  input = new Input(this, narg, arg);
  output = new Output(this);
  update = new Update(this);

  domain = new Domain(this);
  material = new Material(this);
  modify = new Modify(this);
  group = new Group(this);

  // parse input switches

  int inflag = 0;

  // Process command line arguments:
  int iarg = 1;
  while (iarg < narg) {
    if (strcmp(arg[iarg],"-in") == 0 ||
	strcmp(arg[iarg],"-i") == 0) {
      if (iarg+2 > narg) {
	printf("Invalid command-line argument\n");
	exit(1);
      }
      inflag = iarg + 1;
      iarg += 2;
    }
  }

  if (inflag != 0) infile.open(arg[inflag], ios_base::in); // open in read only

  if (!infile.is_open()) {
    printf("Cannot open input script %s\n",arg[inflag]);
    exit(1); 
  }
}

MPM::~MPM()
{
  delete input;
  delete output;
  delete update;

  delete domain;
  delete material;
  delete modify;
  delete group;

  if (infile.is_open()) infile.close();
  if (logfile.is_open()) logfile.close();
}
