#include "view.hh"

Point::Point() : Vector(2)
{
  x = &c[0];
  y = &c[1];
}


View::View()
{
  ur=new Point();
  ll=new Point();
}

View::~View()
{
  delete(ur);
  delete(ll);
}

Vector::Vector(int d)
{
  dim=d;
  c=(double *)calloc(d,sizeof(double));
}

Vector::~Vector()
{
  free(c);
}
