#ifndef __SDLDRAW_HH_
#define __SDLDRAW_HH_

#include "draw.hh"
#include "defaults.hh"
#include "data.hh"
#include "options.hh"
#include "view.hh"
#include "randfuncs.hh"

//! SDL implementation of the Graphics interface with SDL_gfx and SDL_ttf
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

  virtual void Clear(Color *bg);
  
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


  SDL_Surface *screen;
  SDL_Surface *tmpSurface;
  SDL_mutex *plotSemaphore;

  Point *p;
  Point *plotArea;

  TTF_Font *font;
  TTF_Font *titleFont;

  int screen_height;
  int screen_width;

  //! margin is the space reserved for labels and text
  int plot_margin_right;
  //! margin is the space reserved for labels and text
  int plot_margin_left;
  //! margin is the space reserved for labels and text
  int plot_margin_top;
  //! margin is the space reserved for labels and text
  int plot_margin_bottom;
};

//! SDL specific portions. Mainly screen semaphores and label font metrics.
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
  virtual void PlotData(Data *d);
  virtual void PlotFinished(Data *d);

  void ReadData();

  //! the view that is plotted (setting this has affect on PlotData)
  View *currentView;
  Data *data;
  int viewChanged;

  SDLGraphics *GetGraphics();

  int number_width;
  int number_height;
  int legend_width;
  int title_height;
  int moreData;
  int readRows;
};

//! Handles events eg. zooming 
class SDLController
{
public:
  SDLController(SDLPlotter *p);
  //! save zooming region
  void SaveRegion(); 

  //! event filter for plotter
  void FilterEvents();

  //! the plotter to control
  SDLPlotter *plotter; 
  SDLGraphics *graphics;

  SDL_Rect zoomRect;

  int ticks;
  int start;
  Data *d;
  View *customView;
  int zoomSelection;

  int origX, origY;
  int prevX, prevY;
  Point *p1;
  Point *p2;
  int mouseTicks;
};

#endif
