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

  customView = new View();

  start = SDL_GetTicks();

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
	  SDL_KillThread(painter);
	  SDL_Quit();
	  exit(0);
	}
      }
  }
}
