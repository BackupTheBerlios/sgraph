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

int paint(void *unused)
{
  if(opts->follow)
    ticks=100;
  else 
    ticks=1000;

  while(true)
  {
    int now;
    now = SDL_GetTicks();
    if(now - start > ticks && (opts->follow || opts->update)) 
    {
      if(opts->update)
	d->ResetData();
      if(opts->follow)
	d->SetEofReached(0);
      plotter->PlotData(d);
      start = SDL_GetTicks();
    }
  }
}
int main(int argc, char **argv)
{
  opts=new SGraphOptions();
  opts->ParseOpts(argc,argv);
  
  d=new Data(opts);

  if(opts->follow)
  {
    plotter = new StreamSDLPlotter(opts);
    ticks=100;
  }
  else 
    plotter = new SDLPlotter(opts);

  graphics = plotter->GetGraphics();

  customView = new View();

  plotter->PlotData(d);

  start = SDL_GetTicks();

  SDL_CreateThread(paint,NULL);

  while(true)
  {
    SDL_Event event;
    int done = 0;
 
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
	if(done == 1) 
	{
	  SDL_Quit();
	  exit(0);
	}
      }
  }
}
