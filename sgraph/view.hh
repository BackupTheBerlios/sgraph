#ifndef __VIEW_HH_
#define __VIEW_HH_

#include "defaults.hh"

//! generic n-dim data vector
class Vector
{
public:
  Vector(int d);
  ~Vector();
  double *c;
  int dim;
};

//! 2-dim data vector
class Point : public Vector
{
public:
  Point();
  
  double *x;
  double *y;
};

//! lower left and upper right points that asign a rectangle in 2-space
class View
{
public:
  View();
  ~View();
  Point *ll;
  Point *ur;
};

#endif
