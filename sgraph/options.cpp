#include "options.hh"

void SGraphOptions::usage()
{
  printf("Usage: sgraph file1.dat file2.dat file3.dat\n");
  printf("\n");
  printf("Simple data plotter (imitating xgraph, but trying to look cool)\n");
  printf("\n");
  printf("Options: \n");
  printf("  -f --follow  Plots data in real time from file(s).\n");
  printf("  -u --update  Periodically check file(s) and update plot.\n");
  printf("\n");
  printf("  Other xgraph options should work.\n");
  printf("\n");
  printf("\n");
  
  exit(0);
}

void SGraphOptions::parseOpts(int argc, char **argv)
{
  // set defaults
  colc=0;
  debug=0;

  while (1) 
  {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    int digit_optind = 0;
    int c;
    
    static struct option long_options[] = {
      {"train", 0, 0, 0},
      {0, 0, 0, 0}
    };
    
    c = getopt_long (argc, argv, "t", long_options, &option_index);
    if (c == -1)
      break;
    
    switch (c) 
    {
    case 0:
      break;
    case 't':
      break;
    case 'h':
      usage();
      break;
    default:
      usage();
      break;
    }
  }
}

int Options::getInt(char *str, char *errorMsg)
{
  int value;
  if(sscanf(str,"%d",&value) != 1) 
  {
    printf("%s\n", errorMsg);
    usage();
  }
  return(value);
}

double Options::getDouble(char *str, char *errorMsg)
{
  double value;
  if(sscanf(str,"%lf",&value) != 1) 
  {
    printf("%s\n", errorMsg);
    usage();
  }

  return(value);
}

int Options::getIntVector(char *line, char *errorMsg, int *result)
{
  int i=0;
  char *toke;
  toke = strtok(line,",");
  while (toke != NULL) 
  {
    if(sscanf(toke,"%d",&result[i++]) != 1)
    {
      printf("%s\n", errorMsg);
      usage();
    }
    toke = strtok(NULL, ",");  
  }
  return(i);
}
