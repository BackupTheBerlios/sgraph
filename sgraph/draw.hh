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


// lower left and upper right positions
class Color
{
public:
  int r;
  int g;
  int b;
  int a;
};

// override to support different renderers.. eg. BMPGraphics
class Graphics 
{
public:
  Graphics() {}
  virtual ~Graphics() {}

  // line
  virtual void drawLine(Point *from, Point *to, Color *col) {}

  // one pixel point
  virtual void drawPoint(Point *p, Color *col) {}

  // filled rectangle
  virtual void drawRect(Point *ll, Point *ur, Color *col) {}

  // circle
  virtual void drawCircle(Point *center, int rad, Color *col) {}

  // draw text at position lower left. Font size fixed
  virtual void drawText(char *str, Point *ll, Color *fg, int alignx, int aligny) {}

  virtual void Clear() {}
  // update plot
  virtual void Updated() {}
  
  View *view;
};

// render abstract data-valued graphics with pixels on screen (and cool anti-aliasing)
class SDLGraphics : public Graphics
{
public:
  SDLGraphics();
  virtual ~SDLGraphics();
  // override stuff from graphics

  // line
  virtual void drawLine(Point *from, Point *to, Color *col);

  // one pixel point
  virtual void drawPoint(Point *p, Color *col);

  // filled rectangle
  virtual void drawRect(Point *ll, Point *ur, Color *col);

  // circle
  virtual void drawCircle(Point *center, int rad, Color *col);

  // draw text at position lower left. Font size fixed
  virtual void drawText(char *str, Point *p, Color *fg, int alignx, int aligny);

  virtual void Clear();
  
  /* internal funcs */ 

  // translation
  int PointToPixelX(Point *p);
  int PointToPixelY(Point *p);
  // inverse translation
  
  // point is live until next call.
  Point *PixelsToPoint(int x, int y);

  // called after update performed
  virtual void Updated();

  void SetScreenSize(int w, int h);

  Point *GetPlotAreaSize();

  Point *p;
  SDL_Surface *screen;
  Point *plotArea;

  TTF_Font *font;

  int done;
  int glob;
  
  int screen_height;
  int screen_width;

  int plot_margin_right;
  int plot_margin_left;
  int plot_margin_top;
  int plot_margin_bottom;

};

// Plot data to given graphics context override to support
// eg. SDLPlotter, GLEPlotter (maybe StreamedPlotter and UpdatingPlotter)
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
};

class SDLPlotter : public Plotter
{
public:
  SDLPlotter(SGraphOptions *o, Data *d);
  ~SDLPlotter();
  
  virtual int NMaxXTicks();
  virtual int NMaxYTicks();
  virtual void DrawLegend(Data *d);
  virtual int GetLegendWidth(Data *d);
  virtual void InitPlot(Data *d);

  SDLGraphics *GetGraphics();

  int number_width;
  int number_height;
  int legend_width;
  int title_height;;
};

#endif
