#include "view.hh"

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

