#ifndef __DATA_HH_
#define __DATA_HH_

#include "defaults.hh"
#include "view.hh"
#include "options.hh"

//! One dataset per file (two column format) Load all points into RAM. If memory consumption becomes problematic, we could use a binary tmp file to store intermediate stages.
class DataFile
{
public:
  DataFile(char *name);
  ~DataFile();


  //! read one point from file in succsessive manner, NULL is returned if row is corrupted or if no more data, or if malformed row is encountered.
  Point *ReadRow();

  //! get all data read until now
  Point **GetData();

  //! is there more data
  int MoreData();
  
  //! return number of rows read until now with ReadRow
  int GetRowCount();

  //! reopen file, reset row counter to 0
  void Reset();

  //! Close file and "lose" all data, calls to ReadRow will start from beginning, 
  void CloseFile();

  //! opens data file. If filename - or stdin, open stdin.
  void OpenFile();

  Point **points;
  FILE *handle;
  char *name;
  double minX, minY, maxX, maxY, scalingFactorX, scalingFactorY;
  int logX, logY, RowCount, allocated, eofReached, localCount, charCounter, lastModified;
};

//!  abstract data class, collection N datasets that are to be plotted

/*!
  Currently the implementation is oriented
  to one data-set/file type of data, although this could
  change in the future. Eg. multicolumn files, and XGraph type files with commands.

  Baiscally, you have to milk the data with ReadPoint function until there is not more, 
  otherwise data is not not be read, ie. we don't read all data at once, as sometimes this might 
  be suicidal. The application logic takes care of deciding the data reading strategy based on command line options.
*/
class Data
{ 
public:
  Data(SGraphOptions *o);
  ~Data();

  //! return the bounds of all data read so far
  View *GetDefaultView();

  //! read one point from dataset n. NULL is returned if point is corrupted some how (happens often with last row of stdin data that isn't ready yet)
  Point *ReadPoint(int n);

  //! does dataset n potentially contain more points
  int MorePoints(int n);

  //! reset the data for rereading from start
  void ResetData();

  //! get direct data object for datasets (of points read so far with ReadPoint)
  Point **GetPoints(int n);

  //! number of rows read so far in dataset n
  int GetRowCount(int n);

  // to enable following files a la tail -f (files that are constantly being written new data) without rereading from beginning
  void SetEofReached(int e);

  // Get name of dataset n (could be eg. file name, or a name given with a XGraph type command)
  char *GetDataName(int n);

  // Get number of datasets 
  int GetDataSetCount();

  //! list of DataFile objects
  DataFile **GetDataFiles();

  View *defaultView;
  DataFile **dataFiles;
  SGraphOptions *opts;
};

#endif 
