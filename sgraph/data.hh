#ifndef __DATA_HH_
#define __DATA_HH_

#include "defaults.hh"
#include "view.hh"
#include "options.hh"

//! One dataset per file (two column format) 
//! Load all points into RAM. If memory consumption becomes problematic, we could use a binary tmp file to store 
//! intermediate stages
class DataFile
{
public:
  DataFile(char *name);
  ~DataFile();


  //! read one point from file in succsessive manner, NULL is returned if row is corrupted or if 
  //! no more data, or if malformed row is encountered.
  Point *ReadRow();

  // get all data read until now
  Point **GetData();
  int MoreData();
  
  //! return number of rows read until now with ReadRow
  int GetRowCount();
  void Reset();

  //! Close file and "lose" all data, calls to ReadRow will start from beginning, 
  //! I don't remember if stdin is taken care of, I suspect it is retained.
  void CloseFile();

  //! opens data file. If filename - or stdin, open stdin.
  void OpenFile();

  Point **points;
  FILE *handle;
  char *name;
  double minX, minY, maxX, maxY, scalingFactorX, scalingFactorY;
  int logX, logY, RowCount, allocated, eofReached, localCount, charCounter, lastModified;
};

/*!
  abstract data class, contains N data sets
  currently the implementation is oriented
  to one data-set/file type of data, although this could
  change in the future.

  Basically you should only use this to read data, the current implementation
  utilizes DataFile.
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
