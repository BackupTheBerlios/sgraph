#ifndef __DEMODATA_HH_
#define __DEMODATA_HH_

#include "defaults.hh"
#include "view.hh"
#include "options.hh"
#include "data.hh"


//! an example of machine calculated data
class DemoData : public Data
{ 
public:
  DemoData(SGraphOptions *o);
  ~DemoData();

  //! return the bounds of all data read so far
  virtual View *GetDefaultView();

  //! read one point from dataset n. NULL is returned if point is corrupted some how (happens often with last row of stdin data that isn't ready yet)
  virtual Point *ReadPoint(int n);

  //! does dataset n potentially contain more points
  virtual int MorePoints(int n);

  //! reset the data for rereading from start
  virtual void ResetData();

  //! get direct data object for datasets (of points read so far with ReadPoint)
  virtual Point **GetPoints(int n);

  //! get single point with index, translated according to options
  virtual void GetPoint(int col, int i, Point *p);

  //! number of rows read so far in dataset n
  virtual int GetRowCount(int n);

  // to enable following files a la tail -f (files that are constantly being written new data) without rereading from beginning
  virtual void SetEofReached(int e);

  // Get name of dataset n (could be eg. file name, or a name given with a XGraph type command)
  virtual char *GetDataName(int n);

  // Get number of datasets 
  virtual int GetDataSetCount();

  // translate a point according to opts.
  virtual void Translate(int i, Point *p);

  //! list of DataFile objects
  virtual DataFile **GetDataFiles();

  View *defaultView;
  DataFile **dataFiles;
  SGraphOptions *opts;
  Point **points;
};


#endif
