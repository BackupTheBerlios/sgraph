#ifndef __DATA_HH_
#define __DATA_HH_

#include "defaults.hh"
#include "view.hh"
#include "options.hh"

class DataFile
{
public:
  DataFile(char *name);
  ~DataFile();

  Point *ReadRow();
  Point **GetData();
  int MoreData();
  int GetRowCount();
  void Reset();
  void CloseFile();
  void OpenFile();

  int lastModified;

  Point **points;
  FILE *handle;
  char *name;

  double minX;
  double minY;
  double maxX;
  double maxY;
  double scalingFactorX;
  double scalingFactorY;
  int logX;
  int logY;
  int RowCount;
  int allocated;
  int eofReached;
};

class Data
{ 
public:
  Data(SGraphOptions *o);
  ~Data();

  // return a very basic view 
  View *GetDefaultView();

  DataFile **GetDataFiles();

  // read one point
  Point *ReadPoint(int col);
  int MorePoints(int col);

  void ResetData();

  // get all points read so far
  Point **GetPoints(int col);
  int GetRowCount(int col);

  // default view (contains global limits)
  View *defaultView;
  DataFile **dataFiles;
  SGraphOptions *opts;
};


#endif 
