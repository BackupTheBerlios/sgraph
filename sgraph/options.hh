#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#ifndef __OPTIONS_HH_
#define __OPTIONS_HH_

//! generic option parser using getopt, mainly for parsing options to computer readable types.
class Options 
{
public:
  virtual void usage() {}
  virtual void ParseOpts(int argc, char *argv[]) {}
  int getInt(char *str, char *errorMsg);
  double getDouble(char *str, char *errorMsg);
  int getIntVector(char *str, char *errorMsg, int *result);
  /* int getStringVector(char *str, char *errorMsg, char **result); */
};

//! sgraph specific option parser, contains as attributes all possible options.
class SGraphOptions : public Options
{
public:
  void usage();

  //! pass argc and argv to this and it will take care of everything
  void ParseOpts(int argc, char *argv[]);
  int debug;
  int GetNameCount();

  int NameCount;
  int follow; 
  int update;
  int histogram;
  int histogram2d;
  int reverse;
  char *FileNames[100];
};

#endif
