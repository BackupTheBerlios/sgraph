#include "demodata.hh"

DemoData::DemoData(SGraphOptions *o) 
{
  points = (Point **)calloc(1000,sizeof(Point *));

  for(int i=0; i<1000 ; i++)
  {
    points[i] = new Point();
    
    *points[i]->x=(double)i;
    *points[i]->y=(double)0.1*sin(0.05*i);
  }

  defaultView = new View();

  *defaultView->ll->x = 0.0;
  *defaultView->ll->y = -2.0;
  *defaultView->ur->x = 1000.0;
  *defaultView->ur->y = 2.0;
}

View *DemoData::GetDefaultView()
{
  return defaultView;
}

Point *DemoData::ReadPoint(int n)
{
  return NULL;
}

int DemoData::MorePoints(int n)
{
  // we never have to read any points, because we calculate them ourself.
  return 0;
}

//! reset the data for rereading from start
void DemoData::ResetData()
{
  
}

//! hmm.
Point **DemoData::GetPoints(int n)
{
  return points;
}

//! get single point with index, translated according to options
void DemoData::GetPoint(int col, int i, Point *p)
{
  *p->x = *points[i]->x;
  *p->y = *points[i]->y;
}

//! number of rows read so far in dataset n
int DemoData::GetRowCount(int n)
{
  return 1000;
}

// to enable following files a la tail -f (files that are constantly being written new data) without rereading from beginning
void DemoData::SetEofReached(int e)
{
}

// Get name of dataset n (could be eg. file name, or a name given with a XGraph type command)
char *DemoData::GetDataName(int n)
{
  return "Random-walk sinusoid";
}

// Get number of datasets 
int DemoData::GetDataSetCount()
{
  return 1;
}

// translate a point according to opts.
void DemoData::Translate(int i, Point *p)
{
  
}

//! list of DataFile objects
DataFile **DemoData::GetDataFiles()
{
  return NULL;
}
