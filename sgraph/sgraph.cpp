#include "defaults.hh"
#include "draw.hh"
#include "view.hh"
#include "options.hh"
#include "data.hh"

SGraphOptions *opts;
int ticks;
SDLPlotter *plotter; 
int start;
Data *d;
SDLGraphics *graphics;
View *customView;
int done = 0;
int zoomSelection=0;
SDL_Rect zoomRect;
int origX, origY;
int prevX, prevY;
Point *p1;
Point *p2;

//! a kludge for quickly doing painting, a GUI Component framework would be better in the future testing the classes
int paint(void *unused)
{
  if(opts->follow)
    ticks=1000;
  else 
    ticks=1000;

  plotter->PlotData(d,customView);
  while(!done)
  {
    SDL_Delay(ticks);
    if(opts->follow || opts->update) 
    {
      if(opts->update)
	d->ResetData();
      if(opts->follow)
	d->SetEofReached(0);
      plotter->PlotData(d,customView);
    }
  }
}

void SaveRegion()
{
  int code = SDL_BlitSurface(graphics->screen, &zoomRect, graphics->tmpSurface, &zoomRect);
}

int FilterEvents(const SDL_Event *event) 
{
  int x,y;

  if( event->type == SDL_MOUSEBUTTONUP) 
  {
    if ( event->button.button == SDL_BUTTON_LEFT  && zoomSelection )
    {
      zoomSelection = 0;
      int code = SDL_BlitSurface(graphics->tmpSurface, &zoomRect, graphics->screen, &zoomRect);
      graphics->Updated();
      if(customView==NULL)
	customView=new View();

      p1= graphics->PixelsToPoint(zoomRect.x, zoomRect.y+zoomRect.h);
      *customView->ll->x = *p1->x;
      *customView->ll->y = *p1->y;
      p1= graphics->PixelsToPoint(zoomRect.x+zoomRect.w, zoomRect.y);
      *customView->ur->x = *p1->x;
      *customView->ur->y = *p1->y;
      plotter->PlotData(d, customView);
    }

    if ( event->button.button == SDL_BUTTON_RIGHT )
    {
      customView=NULL;
      plotter->PlotData(d, customView);
    }

    return(0);
  } 
  if ( event->type == SDL_MOUSEMOTION && zoomSelection)
  {
    if(zoomSelection) 
    {
      SDL_GetMouseState(&x, &y);
      // erase previous selection
      if(abs(x-prevX) > 5 && abs(y- prevY) > 5)
      {
	// revert
	int code = SDL_BlitSurface(graphics->tmpSurface, &zoomRect, graphics->screen, &zoomRect);
	
	double tmp;
	
	if((x - origX) < 0)
	{
	  zoomRect.w = abs(x-origX);
	  zoomRect.x = x;
	} else {
	  zoomRect.w = x - zoomRect.x;
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
	

	SDL_UpdateRect(graphics->screen, zoomRect.x, zoomRect.y, zoomRect.w, zoomRect.h);
	
	prevX=x;
	prevY=y;
      }
    }
    return(0);
  } 
  if( event->type == SDL_MOUSEBUTTONDOWN ) 
  {
    if ( event->button.button == SDL_BUTTON_LEFT && !zoomSelection )
    {
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
    }
    return(0);
  }
  return(1);  
}

int main(int argc, char **argv)
{
  opts=new SGraphOptions();
  opts->ParseOpts(argc,argv);
  
  d=new Data(opts);

  plotter = new SDLPlotter(opts,d);
  if(opts->follow)
  {
    ticks=100;
  }

  graphics = plotter->GetGraphics();

  customView = NULL;
  

  start = SDL_GetTicks();

  //! one thread paints (important for --follow and --update or if datafile is huge) and the other listens to events.
  SDL_Thread *painter = SDL_CreateThread(paint,NULL);

  /* Filter mouse motion events */
  SDL_SetEventFilter(FilterEvents);

  while(true)
  {
    SDL_Event event;

    SDL_Delay(150);
 
      while ( SDL_WaitEvent(&event) >= 0 )
      {
	Uint8 *keys;
	keys = SDL_GetKeyState(NULL);

	if ( event.type == SDL_QUIT )  
	{
	  done = 1;  
	} 
	if ( event.type == SDL_KEYDOWN )
	{
	  if ( event.key.keysym.sym == SDLK_ESCAPE ) 
	  { 
	    done = 1; 
	  }
	  if( event.key.keysym.sym == SDLK_c )
	  {
	    if( event.key.keysym.mod & KMOD_CTRL & KMOD_CTRL ) 
	      done = 1; 
	  }
	} else
	if( event.type == SDL_VIDEORESIZE ) 
	{
	  graphics->SetScreenSize(event.resize.w, event.resize.h);
	  plotter->PlotData(d,customView);
	} 

	if(done == 1) 
	{
	  fprintf(stderr,"quitting\n");
	  plotter->QuitPlotting();
	  SDL_KillThread(painter);
	  SDL_Quit();
	  exit(0);
	}
      }
  }
}
