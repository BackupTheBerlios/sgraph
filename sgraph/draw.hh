#ifndef _DRAW_HH_
#define _DRAW_HH_

#include "defaults.hh"
#include "data.hh"
#include "options.hh"
#include "view.hh"
#include "randfuncs.hh"

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
  // line
  void drawLine(Point *from, Point *to, Color *col);

  // one pixel point
  void drawPoint(Point *p, Color *col);

  // filled rectangle
  void drawRect(Point *ll, Point *ur, Color *col);

  // circle
  void drawCircle(Point *center, int rad, Color *col);

  // draw text at position lower left. Font size fixed
  void drawText(char *str, Point *ll, Color *fg);

  // called after update performed
  void Updated();
  
  View *view;
};

// render abstract data-valued graphics with pixels on screen (and cool anti-aliasing)
class SDLGraphics : public Graphics
{
public:
  SDLGraphics();
  ~SDLGraphics();
  // override stuff from graphics

  // line
  void drawLine(Point *from, Point *to, Color *col);

  // one pixel point
  void drawPoint(Point *p, Color *col);

  // filled rectangle
  void drawRect(Point *ll, Point *ur, Color *col);

  // circle
  void drawCircle(Point *center, int rad, Color *col);

  // draw text at position lower left. Font size fixed
  void drawText(char *str, Point *ll, Color *fg);

  void Clear();

  // translation
  int PointToPixelX(Point *p);
  int PointToPixelY(Point *p);
  // inverse translation
  
  // point is live until next call.
  Point *PixelsToPoint(int x, int y);

  // called after update performed
  void Updated();

  Point *p;
  SDL_Surface *screen;

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
  int UseGrid;
  int UseLegend;

  void PlotData(Graphics g, View v, Data d);
};

class SDLPlotter : Plotter
{
public:
  SDLPlotter(SGraphOptions *o);
  ~SDLPlotter();
  virtual void PlotData(Data *d);
  void CreateColors(SGraphOptions *o);
  SDLGraphics *GetGraphics();

  SDLGraphics *graphics;
  SGraphOptions *opts;
  Color *colors;

  int plot_margin_right;
  int plot_margin_left;
  int plot_margin_top;
  int plot_margin_bottom;

  int plotCount;
};


class StreamSDLPlotter : public SDLPlotter
{
public:
  StreamSDLPlotter(SGraphOptions *o) : SDLPlotter(o) {}
  void PlotData(Data *d);
};

#endif
