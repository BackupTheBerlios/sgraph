#include "options.hh"

int SGraphOptions::GetNameCount()
{
  return NameCount;
}

void SGraphOptions::usage()
{
  printf("Usage: sgraph file1.dat file2.dat file3.dat\n");
  printf("\n");
  printf("Simple data plotter (imitating xgraph, but trying to look cool)\n");
  printf("use - for stdin\n");
  printf("\n");
  printf("Options: \n");
  printf("  -f --follow  Plots data in real time from file(s).\n");
  printf("  -u --update  Periodically check file(s) and update plot.\n");
  printf("\n");
  printf("  Several other xgraph options should work.\n");
  printf("\n");
  printf("\n");
  
  exit(0);
}

void SGraphOptions::ParseOpts(int argc, char **argv)
{
  // set defaults
  int colc=0;
  int debug=0;
  follow=0;
  update=0;

  while (1) 
  {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    int digit_optind = 0;
    int c;
    
    static struct option long_options[] = {
      {"follow", 0, 0, 0},
      {"update", 0, 0, 0},
      {0, 0, 0, 0}
    };
    
    c = getopt_long (argc, argv, "fu", long_options, &option_index);
    if (c == -1)
      break;
    
    switch (c) 
    {
    case 0:
      if(option_index == 0)
	follow=1;
      if(option_index == 1)
	update=1;
      break;

    case 'f':
      follow=1;
      break;
    case 'u':
      update=1;
      break;
    default:
      usage();
      break;
    }
  }

  NameCount=0;  
  if((argc - optind) == 0) 
  {
    usage();
    exit(1);
  }
  else 
  {
    while((argc - optind) > 0)
    {
      FileNames[NameCount] = argv[optind];
      optind++;
      NameCount++;
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
