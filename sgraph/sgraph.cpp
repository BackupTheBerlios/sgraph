#include "defaults.hh"
#include "draw.hh"
#include "view.hh"
#include "options.hh"
#include "data.hh"

int main(int argc, char **argv)
{
  int start;
  int ticks=1000;
  SDLPlotter *plotter; 

  SGraphOptions *opts=new SGraphOptions();
  opts->ParseOpts(argc,argv);
  
  Data *d=new Data(opts);


  if(opts->follow)
  {
    plotter = new StreamSDLPlotter(opts);
    ticks=100;
  }
  else 
    plotter = new SDLPlotter(opts);

  SDLGraphics *graphics = plotter->GetGraphics();

  View *customView = new View();

  plotter->PlotData(d);

  int pid=fork();

  start = SDL_GetTicks();
  while(true)
  {
    SDL_Event event;
    int done = 0;
    int now;
    if(pid==0)
    {
      now = SDL_GetTicks();
      if(now -start > ticks && (opts->follow || opts->update)) 
      {
	if(opts->update)
	  d->ResetData();
	if(opts->follow)
	  d->SetEofReached(0);
	plotter->PlotData(d);
	start = SDL_GetTicks();
      }
    } else {
      while ( SDL_PollEvent(&event) )
      {
	int x,y;
	
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
	
	}
	if(done == 1) 
	{
	  SDL_Quit();
	  exit(0);
	}
      }
    }
  }
}
