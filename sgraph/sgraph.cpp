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
Point *p1;
Point *p2;

//! a kludge for quickly doing painting, a GUI Component framework would be better in the future testing the classes
int paint(void *unused)
{
  if(opts->follow)
    ticks=1000;
  else 
    ticks=1000;

  plotter->PlotData(d,NULL);
  while(!done)
  {
    SDL_Delay(ticks);
    if(opts->follow || opts->update) 
    {
      if(opts->update)
	d->ResetData();
      if(opts->follow)
	d->SetEofReached(0);
      plotter->PlotData(d,NULL);
    }
  }
}

void SaveRegion()
{
  SDL_LockSurface(graphics->tmp);
  SDL_LockSurface(graphics->screen);
  SDL_BlitSurface(graphics->screen, &zoomRect, graphics->tmp, &zoomRect);
  SDL_UnlockSurface(graphics->screen);
  SDL_UnlockSurface(graphics->tmp);
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

  p1=new Point();
  p2=new Point();

  graphics = plotter->GetGraphics();

  customView = new View();

  start = SDL_GetTicks();

  //! one thread paints (important for --follow and --update or if datafile is huge) and the other listens to events.
  SDL_Thread *painter = SDL_CreateThread(paint,NULL);

  while(true)
  {
    SDL_Event event;

    SDL_Delay(150);
 
      while ( SDL_PollEvent(&event) )
      {
	int x,y;
	Uint8 *keys;
	keys = SDL_GetKeyState(NULL);

	if ( event.type == SDL_MOUSEMOTION )
	{
	  if(zoomSelection) 
	  {
	    SDL_GetMouseState(&x, &y);
	    // erase previous selection
	    SDL_LockSurface(graphics->screen);
	    SDL_LockSurface(graphics->tmp);
	    SDL_BlitSurface(graphics->screen, &zoomRect, graphics->tmp, &zoomRect);

	    SaveRegion();
	    
	    double tmp;

	    if(x-zoomRect.x < 0)
	    {
	      zoomRect.w = abs(x-zoomRect.x);
	      zoomRect.x = x;
	    } else {
	      zoomRect.w = x-zoomRect.x;
	    }

	    if(y-zoomRect.y < 0)
	    {
	      zoomRect.h = abs(y-zoomRect.y);
	      zoomRect.y = y; 
	    }
	    else 
	    {
	      zoomRect.h = y-zoomRect.y;
	    }
	    // draw new selection
	    rectangleRGBA(graphics->screen, 
			  zoomRect.x, 
			  zoomRect.y, 
			  zoomRect.x + zoomRect.w, 
			  zoomRect.y + zoomRect.h, 
			  plotter->fg->r,
			  plotter->fg->g,
			  plotter->fg->b,
			  200);
	    SDL_UpdateRect(graphics->screen, zoomRect.x, zoomRect.y, zoomRect.w, zoomRect.h);
	    SDL_Delay(50);
	    SDL_UnlockSurface(graphics->tmp);
	    SDL_UnlockSurface(graphics->screen);

	    // fprintf(stdout,"region (%d,%d) (%d,%d)\n",zoomRect.x,zoomRect.y,zoomRect.w,zoomRect.h);
	  }
	}
	if( event.type == SDL_MOUSEBUTTONDOWN ) 
	{
	  if ( event.button.button == SDL_BUTTON_LEFT && !zoomSelection )
	  {
	    SDL_GetMouseState(&x, &y);
	    zoomRect.x=x;
	    zoomRect.y=y;
	    zoomRect.w=1;
	    zoomRect.h=1;
	    SaveRegion();
	    zoomSelection = 1;
	  }
	}

	if( event.type == SDL_MOUSEBUTTONUP && zoomSelection ) 
	{
	  if ( event.button.button == SDL_BUTTON_LEFT )
	  {
	    zoomSelection = 0;
	    SDL_LockSurface(graphics->screen);
	    SDL_LockSurface(graphics->tmp);
	    SDL_BlitSurface(graphics->tmp, &zoomRect, graphics->screen, &zoomRect);
	    SDL_UpdateRect(graphics->screen, zoomRect.x, zoomRect.y, zoomRect.w, zoomRect.h);
	    SDL_UnlockSurface(graphics->screen);
	  }
	}
	
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
	}
	if( event.type == SDL_VIDEORESIZE ) 
	{
	  graphics->SetScreenSize(event.resize.w, event.resize.h);
	  plotter->PlotData(d,NULL);
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
