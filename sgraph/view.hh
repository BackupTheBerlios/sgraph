#ifndef __VIEW_HH_
#define __VIEW_HH_

#include "defaults.hh"

class Vector
{
public:
  Vector(int d);
  ~Vector();
  double *c;
  int dim;
};

class Point : public Vector
{
public:
  Point();
  
  double *x;
  double *y;
};

// lower left and upper right positions
class View
{
public:
  View();
  ~View();
  Point *ll;
  Point *ur;
};


#endif
