#ifndef _DRAW_HH_
#define _DRAW_HH_

#include "defaults.hh"
#include "data.hh"
#include "options.hh"
#include "view.hh"
#include "randfuncs.hh"

#define ALIGN_RIGHT 1
#define ALIGN_LEFT 2
#define ALIGN_TOP  3
#define ALIGN_BOTTOM 4
#define ALIGN_CENTER 5


//! Internal color 
/*!
  \param a an integer argument.
  \param s a constant character pointer.
  \return The test results
  \sa Test(), ~Test(), testMeToo() and publicVar()
*/
class Color
{
public:
  int r;
  int g;
  int b;
  int a;
};

//! Low level graphics primitives interface
class Graphics 
{
public:
  Graphics() {}
  virtual ~Graphics() {}

  //! line
  virtual void drawLine(Point *from, Point *to, Color *col) {}

  //! one pixel point
  virtual void drawPoint(Point *p, Color *col) {}

  //! filled rectangle 
  virtual void drawRect(Point *ll, Point *ur, Color *col) {}

  //! circle
  virtual void drawCircle(Point *center, int rad, Color *col) {}

  //! draw text at position lower left. Font size fixed
  virtual void drawText(char *str, Point *ll, Color *fg, int alignx, int aligny) {}

  //! clear screen with color bg
  virtual void Clear(Color *bg) {}

  //! update plot (eg. flip buffer)
  virtual void Updated() {}
  
  //! The view in data-coordinates (is used eg., to translate data to pixels or vice versa)
  View *view;
};

//! Abstract plotter class. Contains nearly all of the functionality, but cannot be instantiated. 
class Plotter
{
public:
  Plotter(SGraphOptions *o, Data *d);

  virtual void InitPlot(Data *d) { }
  virtual void PlotData(Data *d, View *v);
  virtual void CreateColors(SGraphOptions *o);
  virtual void DrawGrid(Data *d, View *v);
  virtual void DrawLegend(Data *d) {}
  virtual int GetLegendWidth(Data *d) { return 0; }
  virtual void PlotFinished(Data *d) {}
  virtual void QuitPlotting() { continuePlotting = 0; dirty=1; }

  virtual int NMaxXTicks() { return 5; }
  virtual int NMaxYTicks() { return 5; }
  
  Color *colors;
  Color *fg;
  Color *bg;
  SGraphOptions *opts;
  Graphics *graphics;

  int UseGrid;
  int UseLegend;
  int plotCount;
  int continuePlotting;

  // is plot stale
  int dirty;
};


#endif
