#include "defaults.hh"
#include "draw.hh"
#include "view.hh"
#include "options.hh"
#include "data.hh"
#include "sdldraw.hh"

SGraphOptions *opts;
SDLPlotter *plotter; 
Data *d;
SDLGraphics *graphics;
SDLController *controller;
int done = 0;

int control(void *diu)
{
  controller = new SDLController(plotter);
  return(0);
}

int plot(void *notused)
{
  plotter->PlotData(d);
  return(0);
}

int pollforquit(const SDL_Event *event)
{
  Uint8 *keys;

  keys = SDL_GetKeyState(NULL);
  if ( event->type == SDL_QUIT )  
  {
    done = 1;  
  } 
  if ( event->type == SDL_KEYDOWN )
  {
    if ( event->key.keysym.sym == SDLK_ESCAPE ) 
    { 
      done = 1; 
    }
    if( event->key.keysym.sym == SDLK_c )
    {
      if( event->key.keysym.mod & KMOD_CTRL & KMOD_CTRL ) 
	done = 1; 
    }
  }
}

int main(int argc, char **argv)
{
  opts=new SGraphOptions();
  opts->ParseOpts(argc,argv);
  
  d=new Data(opts);

  plotter = new SDLPlotter(opts,d);

  graphics = plotter->GetGraphics();

  SDL_Thread *controller = SDL_CreateThread (control,NULL);
  SDL_Thread *plotThread= SDL_CreateThread (plot,NULL);
  SDL_SetEventFilter(pollforquit);

  while(!done)
  {
    SDL_Delay(50);
    if(opts->follow) 
    {
      d->SetEofReached(0);
      SDL_Delay(1000);
      plotter->PlotData(d);
    } 
    if(opts->update)
    {
      d->ResetData();
      SDL_Delay(1000);
      plotter->PlotData(d);
    }
  }

  plotter->QuitPlotting();
  SDL_KillThread(controller);
  SDL_KillThread(plotThread);
  SDL_Quit();
  exit(0);
}
