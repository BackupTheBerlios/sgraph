#ifndef __VIEW_HH_
#define __VIEW_HH_

class Point
{
public:
  double x;
  double y;
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
