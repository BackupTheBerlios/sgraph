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

  Point **points;
  FILE *handle;
  char *name;
  double minX, minY, maxX, maxY, scalingFactorX, scalingFactorY;
  int logX, logY, RowCount, allocated, eofReached, localCount, charCounter, lastModified;
};

class Data
{ 
public:
  Data(SGraphOptions *o);
  ~Data();

  View *GetDefaultView();
  DataFile **GetDataFiles();
  Point *ReadPoint(int col);
  int MorePoints(int col);
  void ResetData();
  Point **GetPoints(int col);
  int GetRowCount(int col);
  void SetEofReached(int e);

  View *defaultView;
  DataFile **dataFiles;
  SGraphOptions *opts;
};


#endif 
