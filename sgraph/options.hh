#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>

class Options 
{
public:
  virtual void usage() {}
  virtual void parseOpts(int argc, char *argv[]) {}
  int getInt(char *str, char *errorMsg);
  double getDouble(char *str, char *errorMsg);
  int getIntVector(char *str, char *errorMsg, int *result);
  /* int getStringVector(char *str, char *errorMsg, char **result); */
};

class SGraphOptions : public Options
{
public:
  void usage();
  void parseOpts(int argc, char *argv[]);
  int debug;
};

