#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

#ifndef __OPTIONS_HH_
#define __OPTIONS_HH_

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

class SGraphOptions : public Options
{
public:
  void usage();
  void ParseOpts(int argc, char *argv[]);
  int debug;
  int GetNameCount();

  int NameCount;
  int follow; 
  int update;
  char *FileNames[100];
};

#endif
