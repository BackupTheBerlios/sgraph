#ifndef __DATA_HH_
#define __DATA_HH_

#include "defaults.hh"
#include "view.hh"
#include "options.hh"

/*
  One dataset per file (two column format) 
 */
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

/*
  abstract data class, contains N data sets
  currently the implementation is oriented
  to one data-set/file type of data, although this could
  change in the future.
 */
class Data
{ 
public:
  Data(SGraphOptions *o);
  ~Data();

  View *GetDefaultView();
  DataFile **GetDataFiles();
  Point *ReadPoint(int n);
  int MorePoints(int n);
  void ResetData();
  Point **GetPoints(int n);
  int GetRowCount(int n);

  // hmm. kludge to enable following files
  void SetEofReached(int e);

  char *GetDataName(int n);

  int GetDataSetCount();

  View *defaultView;
  DataFile **dataFiles;
  SGraphOptions *opts;
};


#endif 
