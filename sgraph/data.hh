#include "draw.hh"
#include "defaults.hh"

#ifndef __DATA_HH_
#define __DATA_HH_

class Data
{ 
public:
  Data(int NameCount, char **FileNames);
  ~Data();

  // default view (contains global limits)
  View defaultView;

  // return a very basic view 
  View GetDefaultView();

  DataFile *GetDataFiles();

  // read one point
  Point ReadPoint(int col);

  // get all points read so far
  Point *GetPoints(int col);
  int GetRowCount(int col);
};

class MultiColumnFile
{
  
};

class DataFile
{
public:
  DataFile(char *name);
  ~DataFile();

  // 1 if got more data, or data changed
  Point ReadRow();
  Point *GetData();
  int GetRowCount();

  int lastModified;

  Point *points;
  FILE *handle;

  double minX;
  double minY;
  double maxX;
  double maxY;
  double scalingFactorX;
  double scalingFactorY;
  int logX;
  int logY;
  int rowCount;
  int allocated;
};

#endif 
