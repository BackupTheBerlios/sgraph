#include "defaults.hh"

#ifndef _DEFAULTS_HH_
#define _DEFAULTS_HH_

class Point
{
  double x;
  double y;
};

// lower left and upper right positions
class View
{
  Point ll;
  Point ur;
};

// Plot data to given graphics context
class Plotter
{
  int UseGrid;
  int UseLegend;

  void PlotData(Graphics g, View v, Data d);

  void DrawGrid(Graphics g, Data d);
  void DrawLabels(Graphics g, Data d);
  void DrawLabels(Graphics g, Data d);
};

class Graphics 
{
  // line
  void drawLine(Point from, Point to, Color col);

  // one pixel point
  void drawPoint(Point from, Point to, Color col);

  // filled rectangle
  void drawRect(Point ll, Point ur, Color col);

  // circle
  void drawCircle(Point center, Point to, Color col);

  // draw text at position lower left. Font size fixed
  void drawText(char *str, Point ll, Color fg);

  // called after update performed
  void Updated();
};

// render abstract data-valued graphics with pixels on screen (and cool anti-aliasing)
class SDLGraphics : public Graphics
{
  // override stuff from graphics

  // line
  void drawLine(Point from, Point to, Color col);

  // one pixel point
  void drawPoint(Point from, Point to, Color col);

  // filled rectangle
  void drawRect(Point ll, Point ur, Color col);

  // circle
  void drawCircle(Point center, Point to, Color col);

  // draw text at position lower left. Font size fixed
  void drawText(char *str, Point ll, Color fg);

  // translation
  int PointToPixelX(Point p);
  int PointToPixelY(Point p);
  // inverse translation
  int PixelToPoint(Point p);
  int PixelToPoint(Point p);

  // called after update performed
  void Updated();
};

// postscript graphics?
class PSGraphics : public Graphics
{
  // called after update performed
  void Updated();
};

// otherwise similar to ps graphics, but can save to file (maybe doesn't even open screen?)
class BMPGraphics : public SDLGraphics
{
  SaveBMP(char *name);

  // called after update performed
  void Updated();
};


#endif
