#include "draw.hh"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_DEPTH 32
#define NUM_LEVELS 256
#define SGN(x) ((x)>0 ? 1 : ((x)==0 ? 0:(-1)))
#define ABS(x) ((x)>0 ? (x) : (-x))
#define FG_COLOR colors[0]
#define BG_COLOR colors[nlevels-1];

SDLGraphics::~SDLGraphics()
{
  exit(0);
}

SDLGraphics::SDLGraphics()
{
  int i;
 
  if ( SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0 )
  {
    printf("Unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }
  atexit(SDL_Quit);
    
  screen=SDL_SetVideoMode(DEFAULT_WIDTH,DEFAULT_HEIGHT,DEFAULT_DEPTH,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE);
  if ( screen == NULL )
  {
    fprintf(stderr,"Unable to set 640x480 video: %s\n", SDL_GetError());
    exit(1);
  }

  TTF_Init();
  font=TTF_OpenFont("/usr/share/sgraph/cmss12.ttf", 12);
  TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
  
  p = new Point();
  view = new View();
  plotArea = new Point();

  screen_height=DEFAULT_HEIGHT;
  screen_width=DEFAULT_WIDTH;
}

Point *SDLGraphics::GetPlotAreaSize()
{
  *plotArea->x = screen_width - plot_margin_right - plot_margin_left;
  *plotArea->y = screen_height - plot_margin_top - plot_margin_bottom;
  return plotArea;
}


void SDLGraphics::SetScreenSize(int w, int h)
{
  screen_height=h;
  screen_width=w;
  screen=SDL_SetVideoMode(screen_width, screen_height, DEFAULT_DEPTH,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE);
}

void SDLGraphics::drawLine(Point *from, Point *to, Color *col)
{
  aalineRGBA(screen, PointToPixelX(from), PointToPixelY(from), PointToPixelX(to), PointToPixelY(to), col->r, col->g, col->b, col->a);
}

void SDLGraphics::drawPoint(Point *p, Color *col)
{
  pixelRGBA(screen, PointToPixelX(p), PointToPixelY(p), col->r, col->g, col->b, col->a);
}

void SDLGraphics::drawRect(Point *ll, Point *ur, Color *col)
{
  rectangleRGBA(screen, PointToPixelX(ll), PointToPixelY(ll), PointToPixelX(ur), PointToPixelY(ur), col->r, col->g, col->b, col->a);
}

void SDLGraphics::drawCircle(Point *center, int rad, Color *col)
{
  circleRGBA(screen, PointToPixelX(center), PointToPixelY(center), rad, col->r, col->g, col->b, col->a);
}

void SDLGraphics::drawText(char *str, Point *ll, Color *fg, int alignx, int aligny)
{
  SDL_Color color = { fg->r, fg->g , fg->b, 0 };
  SDL_Rect dstrect;
  SDL_Surface *canvas;
  int w,h;
  int pad = 5;

  TTF_SizeText(font, str, &w, &h);

  if(alignx == ALIGN_LEFT)
  {
    dstrect.x = (int)floor(PointToPixelX(ll))+pad;
  }
  if(alignx == ALIGN_RIGHT)
  {
    dstrect.x = (int)floor(PointToPixelX(ll))- w - pad;
  }
  if(alignx == ALIGN_CENTER)
  {
    dstrect.x = (int)floor(PointToPixelX(ll) - w/2.0);
  }

  if(aligny == ALIGN_CENTER)
  {
    dstrect.y = (int)floor(PointToPixelY(ll) - h/2.0);
  }
  if(aligny == ALIGN_BOTTOM)
  {
    dstrect.y = (int)floor(PointToPixelY(ll) - h - pad);
  }
  if(aligny == ALIGN_TOP)
  {
    dstrect.y = (int)floor(PointToPixelY(ll) + pad);
  }

  dstrect.w = w;
  dstrect.h = h;

  canvas=TTF_RenderText_Blended(font, str, color);
  SDL_BlitSurface(canvas, NULL, screen, &dstrect);
  SDL_FreeSurface(canvas);
}
// translation
int SDLGraphics::PointToPixelX(Point *p)
{
  int width = screen_width-plot_margin_left-plot_margin_right;
  int pixel = plot_margin_left + (int)floor((*p->x - *view->ll->x)*(width/(*view->ur->x - *view->ll->x)));

  if(pixel>screen_width)
    pixel=screen_width;

  if(pixel<0)
    pixel=0;

  return(pixel);
}
int SDLGraphics::PointToPixelY(Point *p)
{
  int height = screen_height - plot_margin_top - plot_margin_bottom;
  int pixel = plot_margin_bottom + (int)floor(height - (*p->y - *view->ll->y)*(height)/(*view->ur->y - *view->ll->y));

  if(pixel>screen_height)
    pixel=screen_height;

  if(pixel<0)
    pixel=0;

  return(pixel);
}

// inverse translation
Point *SDLGraphics::PixelsToPoint(int x, int y)
{
  *p->x = (x/screen_width)*(*view->ur->x - *view->ll->x) + *view->ll->x;
  *p->y = *view->ur->y - (y/screen_height)*(*view->ur->y - *view->ll->y);
  return(p);
}

// called after update performed
void SDLGraphics::Updated()
{
  SDL_Flip(screen);
}


void SDLGraphics::Clear()
{
  boxRGBA(screen, 0, 0, screen_width, screen_height, 0,0,0, 255);
}

void Plotter::CreateColors(SGraphOptions *o)
{
  colors = new Color[o->NameCount];
  srandom(0x4242);
  for(int i=0; i<o->NameCount ; i++)
  {
    int c=(int)floor(genrand()*3);
    if(c==0)
    {
      colors[i].r = 255;
      colors[i].g = (int)floor( 50*genrand() + 155);
      colors[i].b = (int)floor( 50*genrand() + 155);
    }
    if(c==1)
    {
      colors[i].g = 255;
      colors[i].r = (int)floor( 50*genrand() + 155);
      colors[i].b = (int)floor( 50*genrand() + 155);
    }

    if(c==2)
    {
      colors[i].b = 255;
      colors[i].r = (int)floor( 50*genrand() + 155);
      colors[i].g = (int)floor( 50*genrand() + 155);
    }

    colors[i].a = 255;
  }
}


Plotter::Plotter(SGraphOptions *o, Data *d) 
{
  opts=o;  
  CreateColors(opts);
  fg = new Color();
  bg = new Color();

  fg->r=255;
  fg->g=255;
  fg->b=255;
  fg->a=70;
  
  bg->r=0;
  bg->g=0;
  bg->b=0;
  bg->a=255;

  plotCount=0;
}

void Plotter::PlotData(Data *d, View *v)
{
  Point **points;
  Point *p = new Point();
  Point *lastPoint = new Point();
  int c;
  int slice=100000;
  int morePoints=1;
  
  /* plot by slice */
  while(morePoints > 0)
  {
    c=0;
    morePoints=0;
    for(int j=0; j<opts->NameCount ; j++)
    {
      while(d->MorePoints(j) && c < slice)
	d->ReadPoint(j);
      morePoints += d->MorePoints(j);
    }

    if(v==NULL)
      graphics->view = d->GetDefaultView();
    else
      graphics->view = v;
    
    graphics->Clear();
    
    InitPlot(d);
    DrawGrid(d,graphics->view);
    DrawLegend(d);

    for(int j=0; j<opts->NameCount ; j++)
    {
      points=d->GetPoints(j);
      lastPoint = points[0];

      for(int i=1; i< d->GetRowCount(j) ; i++)
      {
	p=points[i];
	if(p!=NULL) 
	{
	  graphics->drawLine(lastPoint,p,&colors[j]);
	  lastPoint = points[i];
	}
	if(i%10000 == 0)
	  graphics->Updated();
      }
    }
    graphics->Updated();
  }
  plotCount++;
}


#ifndef MIN
#define MIN(a,b) (((a)<(b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a,b) (((a)>(b)) ? (a) : (b))
#endif


// similar grid drawing as in xgraph
void Plotter::DrawGrid(Data *d, View *view)
{
  int expX,expY;
  char text[100];

  Point *p1 = new Point();
  Point *p2 = new Point();

  double minY = *view->ll->y;
  double maxY = *view->ur->y;

  double minX = *view->ll->x;
  double maxX = *view->ur->x;

  double minYe, maxYe, minXe, maxXe;

  expX = (int)floor(log10(MAX(fabs(minX),fabs(maxX)))/4.0);
  expY = (int)floor(log10(MAX(fabs(minY),fabs(maxY)))/4.0);

  minYe = minY/(pow(10,expY));
  maxYe = maxY/(pow(10,expY));
  minXe = minX/(pow(10,expX));
  maxXe = maxX/(pow(10,expX));

  double xstep =  (maxXe-minXe)/NMaxXTicks();
  double ystep =  (maxYe-minYe)/NMaxYTicks();

  double xstepRound =  floor(xstep*1000.0)/1000.0;
  double ystepRound =  floor(ystep*1000.0)/1000.0;
  double xstart= ceil(minXe*1000.0)/1000.0;
  double ystart= ceil(minYe*1000.0)/1000.0;
  
  for(double x=xstart; x < maxXe ; x+= xstepRound )
  {
    *p1->x=x*pow(10,expX);
    *p1->y=minY;
    *p2->x=x*pow(10,expX);
    *p2->y=maxY;
    graphics->drawLine(p1,p2,fg); 

    sprintf(text,"%3g",x);

    graphics->drawText(text, p1, fg, ALIGN_CENTER, ALIGN_TOP);

    fprintf(stdout,"X axis %g %g %d\n",x,x*pow(10,expX),expX);
  }
  for(double y=ystart; y< maxYe ; y+= ystepRound )
  {
    *p1->x=minX;
    *p1->y=y*pow(10,expY);

    *p2->x=maxX;
    *p2->y=y*pow(10,expY);
    graphics->drawLine(p1,p2,fg);

    sprintf(text,"%3g",y);
    graphics->drawText(text, p1, fg, ALIGN_RIGHT, ALIGN_CENTER);

    fprintf(stdout,"Y axis %g %g %d\n",y,y*pow(10,expY),expY);
  }
}


/* SDL specific stuff */
SDLPlotter::SDLPlotter(SGraphOptions *o, Data *d) : Plotter(o,d)
{
  SDLGraphics *g = new SDLGraphics();
  graphics=(Graphics *) g;

  number_width = 70;
  number_height = 20;
}

int SDLPlotter::NMaxXTicks()
{
  // todo, take into account font size and plot size 
  SDLGraphics *g=GetGraphics();
  Point *area = g->GetPlotAreaSize();
  return (int)floor((*area->x)/200);
}

int SDLPlotter::NMaxYTicks()
{
  SDLGraphics *g=GetGraphics();
  Point *area = g->GetPlotAreaSize();
  return (int)floor((*area->y)/50);
}

SDLGraphics *SDLPlotter::GetGraphics()
{
  return (SDLGraphics *)graphics;
}


/*
  SDL specific legend drawing routine

 */
void SDLPlotter::DrawLegend(Data *d)
{
  SDLGraphics *g = GetGraphics();
  int width = GetLegendWidth(d);
  Point *p=g->GetPlotAreaSize();
  int padding = 5;
  int minX = (int)*p->x + padding + g->plot_margin_left;
  int maxX = g->screen_width - padding;
  SDL_Color color = { fg->r, fg->g , fg->b, 0 };
  SDL_Rect dstrect;
  SDL_Surface *canvas;
  int w,h;
  int pad = 5;
  int height = 20;

  
  for(int i = 0; i<d->GetDataSetCount() ; i++)
  {
    TTF_SizeText(g->font, d->GetDataName(i), &w, &h);
    dstrect.x = minX;
    dstrect.y = i*height + g->plot_margin_top;
    dstrect.w = w;
    dstrect.h = h;
    
    canvas=TTF_RenderText_Blended(g->font, d->GetDataName(i), color);
    SDL_BlitSurface(canvas, NULL, g->screen, &dstrect);
    SDL_FreeSurface(canvas);
    int y = i*height + g->plot_margin_top - pad;
    aalineRGBA(g->screen, minX, y, maxX, y, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
  }
}

int SDLPlotter::GetLegendWidth(Data *d)
{
  SDLGraphics *g = GetGraphics();
  int w,h,maxW,maxH;
  int padding=5;
  
  maxW=0;
  maxH=0;

  for(int i = 0; i<d->GetDataSetCount() ; i++)
  {
    TTF_SizeText(g->font, d->GetDataName(i), &w, &h);    
    if(w>maxW)
      maxW=w;
    if(h>maxH)
      maxH=h;
  }
  return maxW+2*padding;
}

void SDLPlotter::InitPlot(Data *d)
{
  SDLGraphics *g = GetGraphics();

  legend_width = GetLegendWidth(d);
  title_height = 30;
  
  g->plot_margin_left=10+number_width;
  g->plot_margin_right=10+legend_width;
  g->plot_margin_top=10+title_height;
  g->plot_margin_bottom=10+number_height;
}
