#include "sdldraw.hh"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_DEPTH 32
#define NUM_LEVELS 256



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

  titleFont=TTF_OpenFont("/usr/share/sgraph/cmss12.ttf", 18);
  TTF_SetFontStyle(titleFont, TTF_STYLE_BOLD);
  
  p = new Point();
  view = new View();
  plotArea = new Point();

  screen_height=DEFAULT_HEIGHT;
  screen_width=DEFAULT_WIDTH;

  tmpSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, DEFAULT_WIDTH, DEFAULT_HEIGHT,
				    DEFAULT_DEPTH,
				    screen->format->Rmask,
				    screen->format->Gmask,
				    screen->format->Bmask,
				    screen->format->Amask);
  if(tmpSurface==NULL)
    fprintf(stdout,"Creating tmp surface failed\n");
  
  SDL_SetAlpha(tmpSurface, 0, 255);

  SDL_WM_SetCaption("SGraph",NULL);

  // prevent seperate threads from tampering the plot at the same time.
  plotSemaphore = SDL_CreateMutex();
}

Point *SDLGraphics::GetPlotAreaSize()
{
  *plotArea->x = screen_width - plot_margin_right - plot_margin_left;
  *plotArea->y = screen_height - plot_margin_top - plot_margin_bottom;

  return plotArea;
}


void SDLGraphics::SetScreenSize(int w, int h)
{
  SDL_mutexP(plotSemaphore);
  screen_height=h;
  screen_width=w;
  SDL_LockSurface(screen);
  screen=SDL_SetVideoMode(screen_width, screen_height, DEFAULT_DEPTH,SDL_SWSURFACE|SDL_DOUBLEBUF|SDL_RESIZABLE);
  SDL_UnlockSurface(screen);
  SDL_FreeSurface(tmpSurface);
  tmpSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, screen_width, screen_height,
				    DEFAULT_DEPTH,
				    screen->format->Rmask,
				    screen->format->Gmask,
				    screen->format->Bmask,
				    screen->format->Amask);
  if(tmpSurface==NULL)
    fprintf(stdout,"Creating tmp surface failed\n");
  SDL_SetAlpha(tmpSurface, 0, 255);
  
  SDL_mutexV(plotSemaphore);
}

void SDLGraphics::drawLine(Point *from, Point *to, Color *col)
{
  int fromX=PointToPixelX(from);
  int fromY=PointToPixelY(from);
  int toX=PointToPixelX(to);
  int toY=PointToPixelY(to);
  aalineRGBA(screen, fromX, fromY, toX, toY, col->r, col->g, col->b, col->a);
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

  if(pixel>(screen_width-1))
    pixel=screen_width-1;

  if(pixel<0)
    pixel=0;

  return(pixel);
}
int SDLGraphics::PointToPixelY(Point *p)
{
  int height = screen_height - plot_margin_top - plot_margin_bottom;
  int pixel = plot_margin_bottom + (int)floor(height - (*p->y - *view->ll->y)*(height)/(*view->ur->y - *view->ll->y));

  if(pixel>(screen_height-1))
    pixel=screen_height-1;

  if(pixel<0)
    pixel=0;

  return(pixel);
}

// inverse translation
Point *SDLGraphics::PixelsToPoint(int x, int y)
{
  int width = screen_width-plot_margin_left-plot_margin_right;
  int height = screen_height - plot_margin_top - plot_margin_bottom;

  *p->x = (x - plot_margin_left)*(*view->ur->x - *view->ll->x)/((double)width) + *view->ll->x;
  *p->y = -1.0*((y - plot_margin_bottom)/((double)height) - 1)*(*view->ur->y - *view->ll->y) + *view->ll->y;

  return(p);
}

// called after update performed
void SDLGraphics::Updated()
{
  SDL_LockSurface(screen);
  SDL_Flip(screen);
  SDL_UnlockSurface(screen);
}


void SDLGraphics::Clear(Color *bg)
{
  SDL_LockSurface(screen);
  boxRGBA(screen, 0, 0, screen_width, screen_height, bg->r, bg->g, bg->b, 255);
  SDL_UnlockSurface(screen);
}

SDLGraphics *SDLPlotter::GetGraphics()
{
  return (SDLGraphics *)graphics;
}

/* SDL specific stuff */
SDLPlotter::SDLPlotter(SGraphOptions *o, Data *d) : Plotter(o,d)
{
  SDLGraphics *g = new SDLGraphics();
  graphics=(Graphics *) g;

  number_width = 70;
  number_height = 20;
  currentView=NULL;
}


//! stupid and simple. just synch it, don't even attempt doing too many things at the same time.
void SDLPlotter::PlotData(Data *d)
{
  Point **points;
  Point *p = new Point();
  Point *lastPoint = new Point();
  int c;
  int slice=10000;
  int morePoints=1;
  int gotN=0;

  SDLGraphics *g = GetGraphics();
  data=d;

  QuitPlotting();
  
  SDL_mutexP(g->plotSemaphore);

  InitPlot(d);  

  continuePlotting=1;

  for(int j=0; j<opts->NameCount ; j++)
  {
    while(d->MorePoints(j) && continuePlotting)
    {
      d->ReadPoint(j);
    }
    morePoints += d->MorePoints(j);
  }
  
  if(currentView==NULL)
    graphics->view = d->GetDefaultView();
  else
    graphics->view = currentView;
  
  graphics->Clear(bg);
  
  DrawGrid(d,graphics->view);
  DrawLegend(d);
  
  for(int j=0; j<opts->NameCount && continuePlotting ; j++)
  {
    points=d->GetPoints(j);
    lastPoint = points[0];
    
    for(int i=1; i< d->GetRowCount(j) && continuePlotting ; i++)
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
  SDL_mutexV(g->plotSemaphore);

  plotCount++;
  PlotFinished(d);
}

void SDLPlotter::PlotFinished(Data *d)
{

}

int SDLPlotter::NMaxXTicks()
{
  // todo, take into account font size and plot size 
  SDLGraphics *g=GetGraphics();
  Point *area = g->GetPlotAreaSize();
  return (int)floor((*area->x)/200.0);
}

int SDLPlotter::NMaxYTicks()
{
  SDLGraphics *g=GetGraphics();
  Point *area = g->GetPlotAreaSize();
  return (int)floor((*area->y)/50.0);
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

    SDL_LockSurface(g->screen);
    aalineRGBA(g->screen, minX, y, maxX, y, colors[i].r, colors[i].g, colors[i].b, colors[i].a);
    SDL_UnlockSurface(g->screen);
  }

  // render title
  TTF_SizeText(g->titleFont, "SGraph", &w, &h);
  dstrect.x = g->screen_width - pad - w;
  dstrect.y = pad;
  dstrect.w = w;
  dstrect.h = h;
  
  canvas=TTF_RenderText_Blended(g->titleFont, "SGraph", color);
  SDL_BlitSurface(canvas, NULL, g->screen, &dstrect);
  SDL_FreeSurface(canvas);
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


SDLController::SDLController(SDLPlotter *p)
{
  plotter = p;
  graphics = plotter->GetGraphics();

  zoomSelection=0;
  customView = NULL;

  FilterEvents();
}

void SDLController::SaveRegion()
{
  int code = SDL_BlitSurface(graphics->screen, &zoomRect, graphics->tmpSurface, &zoomRect);
}

// we don't eat up any events, but we don't check if we are in our own territory either. be warned
void SDLController::FilterEvents() 
{
  while(true)
  {
    SDL_Event event;
    
    while ( SDL_WaitEvent(&event) >= 0 )
    {
      int x,y;
    
      if( event.type == SDL_MOUSEBUTTONUP) 
      {
	if ( event.button.button == SDL_BUTTON_LEFT  && zoomSelection )
	{

	  zoomSelection = 0;
	  int code = SDL_BlitSurface(graphics->tmpSurface, &zoomRect, graphics->screen, &zoomRect);
	  SDL_UpdateRect(graphics->screen, zoomRect.x, zoomRect.y, zoomRect.w, zoomRect.h);
	  
	  if(customView==NULL)
	    customView=new View();
	  
	  p1= graphics->PixelsToPoint(zoomRect.x, zoomRect.y+zoomRect.h);
	  *customView->ll->x = *p1->x;
	  *customView->ll->y = *p1->y;
	  p1= graphics->PixelsToPoint(zoomRect.x+zoomRect.w, zoomRect.y);
	  *customView->ur->x = *p1->x;
	  *customView->ur->y = *p1->y;
	  
	  plotter->viewChanged=1;
	  plotter->currentView = customView;

	  SDL_mutexV(graphics->plotSemaphore);

	  plotter->PlotData(plotter->data);
	}
	
	if ( event.button.button == SDL_BUTTON_RIGHT )
	{
	  SDL_mutexP(graphics->plotSemaphore);
	  // revert to default view
	  plotter->currentView=NULL;
	  SDL_mutexV(graphics->plotSemaphore);

	  plotter->PlotData(plotter->data);
	}
      } 
      if ( event.type == SDL_MOUSEMOTION && zoomSelection)
      {
	if(zoomSelection) 
	{

	  SDL_GetMouseState(&x, &y);
	  // erase previous selection
	  int t=SDL_GetTicks();
	  if((t-mouseTicks) > 50)
	  {
	    // revert area
	    int code = SDL_BlitSurface(graphics->tmpSurface, &zoomRect, graphics->screen, &zoomRect);
	    
	    SDL_Rect old;
	    old.x=zoomRect.x;
	    old.y=zoomRect.y;
	    old.w=zoomRect.w;
	    old.h=zoomRect.h;
	    double tmp;
	    
	    if((x - origX) < 0)
	    {
	      zoomRect.w = abs(x-origX);
	      zoomRect.x = x;
	    } 
	    else 
	    {
	      zoomRect.w = x-zoomRect.x;
	    }
	    
	    if( (y-origY) < 0)
	    {
	      zoomRect.h = abs(y-origY);
	      zoomRect.y = y;
	    }
	    else 
	    {
	      zoomRect.h = y-zoomRect.y;
	    }
	    
	    SaveRegion();
	    
	    // draw new selection
	    rectangleRGBA(graphics->screen, 
			  zoomRect.x+2,
			  zoomRect.y+2, 
			  zoomRect.x + zoomRect.w - 2, 
			  zoomRect.y + zoomRect.h - 2, 
			  plotter->fg->r,
			  plotter->fg->g,
			  plotter->fg->b,
			  100);

	    boxRGBA(graphics->screen,
		    zoomRect.x+3,
		    zoomRect.y+3, 
		    zoomRect.x + zoomRect.w - 3, 
		    zoomRect.y + zoomRect.h - 3, 
		    0,0,255,69);

	    SDL_UpdateRect(graphics->screen, old.x, old.y, old.w, old.h);
	    SDL_UpdateRect(graphics->screen, zoomRect.x, zoomRect.y, zoomRect.w, zoomRect.h);
	    prevX=x;
	    prevY=y;
	    mouseTicks=SDL_GetTicks();
	  }
	}
      }

      if( event.type == SDL_VIDEORESIZE ) 
      {
	SDL_mutexP(graphics->plotSemaphore);
	graphics->SetScreenSize(event.resize.w, event.resize.h);
	
	SDL_mutexV(graphics->plotSemaphore);

	plotter->PlotData(plotter->data);

      } 
      
      if( event.type == SDL_MOUSEBUTTONDOWN ) 
      {
	// roller mouse
	if(SDL_GetMouseState(&x,&y)&SDL_BUTTON(4) && !zoomSelection)
	{
	    fprintf(stdout,"implement zoom out\n");
	}
	if(SDL_GetMouseState(&x,&y)&SDL_BUTTON(5) && !zoomSelection)
        {
	    fprintf(stdout,"implement zoom in\n");
	}
	if ( event.button.button == SDL_BUTTON_LEFT && !zoomSelection )
	{
	  SDL_mutexP(graphics->plotSemaphore);

	  SDL_GetMouseState(&x, &y);
	  zoomRect.x=x;
	  zoomRect.y=y;
	  origX=x;
	  origY=y;
	  prevX=x;
	  prevY=y;
	  zoomRect.w=1;
	  zoomRect.h=1;
	  SaveRegion();
	  zoomSelection = 1;
	  mouseTicks=SDL_GetTicks();
	}
      }
    }
  }
}




