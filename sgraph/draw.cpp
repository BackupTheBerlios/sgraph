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

  /*    TTF_Init();
	font=TTF_OpenFont("cmss12.ttf", 12);
	TTF_SetFontStyle(font, TTF_STYLE_NORMAL);*/
  
  p = new Point();
  view = new View();
  screen_height=DEFAULT_HEIGHT;
  screen_width=DEFAULT_WIDTH;
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

void SDLGraphics::drawText(char *str, Point *ll, Color *fg)
{
  // todo
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

// similar grid drawing as in xgraph
void Plotter::DrawGrid(Data *d, View *view)
{
  int     expX,expY;
  Point *p1 = new Point();
  Point *p2 = new Point();

  double minY = *view->ll->y;
  double maxY = *view->ur->y;

  double minX = *view->ll->x;
  double maxX = *view->ur->x;

  double xstep =  (maxX-minX)/5.0;
  double ystep =  (maxY-minY)/5.0;

  for(double x=minX; x<maxX+1 ; x+= xstep )
  {
    *p1->x=x;
    *p1->y=minY;
    *p2->x=x;
    *p2->y=maxY;
    graphics->drawLine(p1,p2,fg);    
  }
  for(double y=minY; y<maxY+1 ; y+= ystep )
  {
    *p1->x=minX;
    *p1->y=y;

    *p2->x=maxX;
    *p2->y=y;
    graphics->drawLine(p1,p2,fg);
  }
}

Plotter::Plotter(SGraphOptions *o) 
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

SDLPlotter::SDLPlotter(SGraphOptions *o) : Plotter(o)
{
  SDLGraphics *g = new SDLGraphics();

  number_width = 70;
  number_height = 20;

  // todo determine legend width from options (largest string)
  legend_width = 100;
  title_height = 20;

  g->plot_margin_left=10+number_width;
  g->plot_margin_right=10+legend_width;
  g->plot_margin_top=10+title_height;
  g->plot_margin_bottom=10+number_height;
  
  graphics=(Graphics *) g;
}

SDLGraphics *SDLPlotter::GetGraphics()
{
  return (SDLGraphics *)graphics;
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
    
    DrawGrid(d,graphics->view);

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

