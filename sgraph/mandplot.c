/* 
   instead of plotting the trad. mandelbrot, we plot all
   converged points, i.e. the final result after iterating 
   z = z2 + c
*/
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h>
#include "graphics.h"
#include "imag.h"
#include "rndfuncs.h"
#include "SDL_gfxPrimitives.h"

#define DEBUG_MAND              1

extern SDL_Surface *screen;
extern TTF_Font *font;
SDL_Surface *bbuffer;
SDL_Surface *canvas;
double *histogram;
int maxCount;
Imag_ctx ictx;
int prevMouseX=0,prevMouseY=0;
// imag.h

double diffMouseX = 0;
double diffMouseY = 0;
int ready=1;

void pollMandEvents();

void mandelbrot(struct imag *z, struct imag *c, struct imag *tmp)
{
  square_complex(z, tmp);  
  add_complex(tmp, c, z);
}

void zoomPlot(Imag *center)
{
  ictx.max.re = center->re + ictx.factor;
  ictx.min.re = center->re - ictx.factor;
  ictx.max.im = center->im + ictx.factor;
  ictx.min.im = center->im - ictx.factor;
}

void initMand() 
{
  initGraphics();
}


double mandStep=.01;
void plotMandelbrot()
{
  double re;
  double im;
  int iteration,maxIterations=50;
  int color,plotx,ploty;
  Imag c,z,tmp,prev;
  double step;
  ready=0;
  step = (ictx.max.re - ictx.min.re)/SCREEN_HEIGHT;
  
  boxRGBA(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 255,255, 255, 255);
  
  for(re= ictx.min.re; re < ictx.max.re ; re+=step)
  {
    SDL_Flip(screen);
    for(im=ictx.min.im; im < ictx.max.im; im+=step) 
    {
      iteration = 0;
      c.re = re;
      c.im = im;
      z.re =0.0;
      z.im =0.0;
      
      while((iteration < maxIterations) && abs_complex(&z) < 2.0)
      {
	prev.re=z.re;
	prev.im=z.im;
	
	mandelbrot(&z,&c,&tmp);
	iteration++;
	
	tmp.re = prev.re-z.re;
	tmp.im = prev.im-z.im;
      }
      plotx = reToPix(&ictx,&c);
      ploty = imToPix(&ictx,&c);
      
      if(((plotx < SCREEN_WIDTH) && (plotx > 0)) && ((ploty < SCREEN_HEIGHT) && (ploty > 0)))
      {
	color=(int)255-floor(100.0*iteration/(double)maxIterations);
	// printf("plotting (%g,%g) => %d,%d\n",c.re,c.im,plotx,ploty);
	pixelRGBA(screen, plotx, ploty, color, color, color, 255);
      }
    }
    pollMandEvents();
  }
  
  // store it to backbuffer
  SDL_Flip(screen);
  SDL_BlitSurface(screen, NULL, bbuffer, NULL);
  ready=1;
}

void plotDensity()
{
  double re;
  double im;
  int iteration,maxIterations=100;
  Imag c,z,tmp,prev;
  int color,plotx,ploty,i,j;
  int maxCount=0;
  double step;
  
  free(histogram);
  histogram = (double *)calloc(SCREEN_WIDTH*SCREEN_HEIGHT,sizeof(double));
  
  // clear screen
  boxRGBA(screen, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 255,255, 255, 255);
  
  for(re= ictx.min.re; re < ictx.max.re ; re+=mandStep) 
  {
    SDL_Flip(screen);
    for(im= ictx.min.im; im < ictx.max.im ; im+=mandStep) 
    {
      iteration = 0;
      c.re = re;
      c.im = im;
      z.re =0.0;
      z.im =0.0;
      prev.re = 0.0;
      prev.im = 0.0;
      
      while((iteration < maxIterations) && abs_complex(&z) < 2.0)
      {
	prev.re=z.re;
	prev.im=z.im;
	mandelbrot(&z,&c,&tmp);
	iteration++;
	
	plotx = reToPix(&ictx,&z);
	ploty = imToPix(&ictx,&z);
	
	if(((plotx < SCREEN_WIDTH) && (plotx > 0)) && ((ploty < SCREEN_HEIGHT) && (ploty > 0)) && abs_complex(&z) < 2.0)
	{
	  histogram[plotx*SCREEN_HEIGHT + ploty]++;
	  
	  if(maxCount < histogram[plotx*SCREEN_HEIGHT + ploty])
	  {
	    maxCount = histogram[plotx*SCREEN_HEIGHT + ploty];
	  }
	  pixelRGBA(screen, plotx, ploty, 0, 0, 0, 50);
	}
      }
      pollMandEvents();
    }
  }
  
  for(i=0; i<SCREEN_WIDTH ; i++)
  {
    for(j=0; j<SCREEN_HEIGHT ; j++)
    {
      color = floor(255*(double)log(histogram[i*SCREEN_HEIGHT + j])/(double)log(maxCount));
      DrawPixel(screen, i , j, 0,0,color);
    }
  }

  SDL_Flip(screen);
  
  // do very light mandelbrot with blue
  
  for(re= ictx.min.re; re < ictx.max.re ; re+=step) 
  {
    SDL_Flip(screen);
    for(im= ictx.min.im; im < ictx.max.im; im+=step) 
    {
      iteration = 0;
      c.re = re;
      c.im = im;
      z.re =0.0;
      z.im =0.0;
      
      while((iteration < maxIterations) && abs_complex(&z) < 2.0)
      {
	prev.re=z.re;
	prev.im=z.im;
	
	mandelbrot(&z,&c,&tmp);
	iteration++;
	tmp.re = prev.re-z.re;
	tmp.im = prev.im-z.im;

	if(abs_complex(&tmp) < 0.01)
	{
	  break;
	}
      }
      plotx = reToPix(&ictx,&c);
      ploty = imToPix(&ictx,&c);
      if(((plotx < SCREEN_WIDTH) && (plotx > 0)) && ((ploty < SCREEN_HEIGHT) && (ploty > 0)))
      {
	color=(int)floor(255.0*iteration/(double)maxIterations);

	pixelRGBA(screen, plotx, ploty, 0, 0, color, 100);
      }
    }
    pollMandEvents();
  }
  
  SDL_BlitSurface(screen, NULL, bbuffer, NULL);
}

SDL_Surface *printInfo(struct imag *start, struct imag *avg, int loops)
{
  SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };
  SDL_Color black = { 0x00, 0x00, 0x00, 0 };
  char text[500];
  sprintf(text,"Start (%.2f,%.2f)\nAvg(%.2f,%.2f)\nLoops: %d\n",start->re, start->im, avg->re, avg->im, loops);
  return(TTF_RenderText_Blended(font, text, white));
}

void iteratePoint(int x, int y)
{
  double re = pixToRe(&ictx,x);
  double im = pixToIm(&ictx,y);
  double tmperr;
  int iteration = 0;
  Imag origin;
  Imag z, c, tmp, avg, tmp2;
#define ITERATION_HISTORY 10
  Imag last[ITERATION_HISTORY];
  int done = 0;
  int plotx, ploty,i,j,color,index=0;
  int prevx, prevy;
  int maxIterations = 500; 
  int unordered=1;
  double minErr=1.0;
  int minErrIt=0;
  SDL_Color white = { 0xFF, 0xFF, 0xFF, 0 };
  SDL_Color black = { 0x00, 0x00, 0x00, 0 };
  SDL_Rect dstrect = { 480, 70, 0, 0 };
  char text[500];
  
  //  printf("Iterateing %d %d = (%g,%g)\n",x,y,re,im);
  c.re = re+diffMouseX;
  c.im = im+diffMouseY;
  
  z.re = re;
  z.im = im;
  avg.re=0.0;
  avg.im=0.0;
  prevx = reToPix(&ictx,&c);
  prevy = imToPix(&ictx,&c);
  
  // background
  SDL_BlitSurface(bbuffer, NULL, screen, NULL);
  
  circleRGBA(screen, x, y, 5, 0, 0,0, 255);

  while((iteration < maxIterations) && abs_complex(&z) < 10.0) 
  {
    mandelbrot(&z,&c,&tmp);
    avg.re+=z.re;
    avg.im+=z.im;
    iteration++;
    
    tmp2.re=z.re-c.re;
    tmp2.im=z.im-c.im;
    tmperr=abs_complex(&tmp2);
    if(tmperr<minErr)
    {
      minErr=tmperr;
      minErrIt=iteration;
    }
    if(tmperr < 0.001 && unordered==1) 
    {
      unordered=iteration;
    }
    
    last[index % ITERATION_HISTORY].re = z.re;
    last[index % ITERATION_HISTORY].im = z.im;
    index++;
    plotx = reToPix(&ictx,&z);
    ploty = imToPix(&ictx,&z);
    
    if(((plotx < SCREEN_WIDTH) && (plotx > 0.0)) && ((ploty < SCREEN_HEIGHT) && (ploty > 0)) && abs_complex(&z) < 100.0)
    {
      if(iteration > 0 && iteration < 100)
      {
	aalineRGBA(screen, prevx, prevy, plotx, ploty, 0, 0, 0, 200);
      }
      prevx = plotx;
      prevy = ploty;
    }
  }
  avg.re=avg.re/iteration;
  avg.im=avg.im/iteration;
  aalineRGBA(screen, reToPix(&ictx,&avg)-5, imToPix(&ictx,&avg)-5, reToPix(&ictx,&avg)+5, imToPix(&ictx,&avg)+5, 0, 0, 0, 255);
  aalineRGBA(screen, reToPix(&ictx,&avg)+5, imToPix(&ictx,&avg)-5, reToPix(&ictx,&avg)-5, imToPix(&ictx,&avg)+5, 0, 0, 0, 255);
  
  sprintf(text,"(%.3f,%.3f) Start",c.re, c.im);
  canvas=TTF_RenderText_Blended(font, text, black);
  SDL_BlitSurface(canvas, NULL, screen, &dstrect);
  SDL_FreeSurface(canvas);
  
  sprintf(text,"(%.3f,%.3f) Avg", avg.re, avg.im, abs_complex(&avg));
  canvas=TTF_RenderText_Blended(font, text, black);
  dstrect.y=90;
  SDL_BlitSurface(canvas, NULL, screen, &dstrect);
  SDL_FreeSurface(canvas);
  
  if(unordered==1)
    unordered=0;
  
  sprintf(text,"Loops %d",unordered);
  canvas=TTF_RenderText_Blended(font, text, black);
  dstrect.y=110;
  SDL_BlitSurface(canvas, NULL, screen, &dstrect);
  SDL_FreeSurface(canvas);
  
  sprintf(text,"Last 10 iterations");
  canvas=TTF_RenderText_Blended(font, text, black);
  dstrect.y=130;
  SDL_BlitSurface(canvas, NULL, screen, &dstrect);
  SDL_FreeSurface(canvas);

  for(index=0; index<ITERATION_HISTORY; index++)
  {
    dstrect.y+=20;
    sprintf(text,"(%.3f,%.3f)", last[index].re, last[index].im);
    canvas=TTF_RenderText_Blended(font, text, black);
    SDL_BlitSurface(canvas, NULL, screen, &dstrect);
    SDL_FreeSurface(canvas);
  }
  //  printf("final point (%g,%g)\n",z.re,z.im);
  SDL_Flip(screen);
}

 void normalize()
 {
   int i,j,color;
   for(i=0; i<SCREEN_WIDTH ; i++)
   {
      for(j=0; j<SCREEN_HEIGHT ; j++)
      {
	color = floor(255*(double)log(histogram[i*SCREEN_HEIGHT + j])/(double)log(maxCount));
	DrawPixel(screen, i , j, color,color,color);
      }
    }
   SDL_Flip(screen);
 }


void pollMandEvents()
{
  SDL_Event event;
  int done = 0;
  while ( SDL_PollEvent(&event) )
  {
    int x,y;
    // printf("type %x %x\n",event.type,SDL_MOUSEBUTTONUP);
    if ( event.type == SDL_QUIT )  
    {
      done = 1;  
    }
    if ( event.type == SDL_MOUSEBUTTONDOWN )
    {
      if ( event.button.button == SDL_BUTTON_LEFT ) // return to initial state
	{
	  // save trace
	  if(ready) 
	  {
	    boxRGBA(screen, SCREEN_HEIGHT, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 255,255, 255, 255);
	    SDL_BlitSurface(screen, NULL, bbuffer, NULL);	    
	  }
	} 
      else if ( event.button.button == SDL_BUTTON_RIGHT )
	{
	  Imag center;
	  if(ready) 
	  {
	    SDL_GetMouseState(&x, &y);
	    center.re = pixToRe(&ictx,x);
	    center.im = pixToIm(&ictx,y);
	    ictx.factor = ictx.factor*0.5;
	    zoomPlot(&center);
	    plotMandelbrot();
	  }
	}
    }
    if ( event.type == SDL_MOUSEMOTION )
      {
	if(ready) 
	{
	  diffMouseX=0.0;
	  diffMouseY=0.0;
	  SDL_GetMouseState(&x, &y);
	  if(x!=prevMouseX || y!=prevMouseY)
	    iteratePoint(x,y);
	}
      }
    if ( event.type == SDL_KEYDOWN )
      {
	if ( event.key.keysym.sym == SDLK_ESCAPE ) { done = 1; }
	if ( event.key.keysym.sym == SDLK_ESCAPE ) { done = 1; }
	
	if(ready) 
	  {
	    if(  event.key.keysym.sym==SDLK_UP )
	      {
		diffMouseY+=0.001;
		SDL_GetMouseState(&x, &y);
		if(x!=prevMouseX || y!=prevMouseY)
		  iteratePoint(x,y);
	      }
	    if(  event.key.keysym.sym==SDLK_DOWN )
	      {
		diffMouseY-=0.001;
		SDL_GetMouseState(&x, &y);
		if(x!=prevMouseX || y!=prevMouseY)
		  iteratePoint(x,y);
	      }
	    if(  event.key.keysym.sym==SDLK_LEFT )
	      {
		diffMouseX+=0.001;
		SDL_GetMouseState(&x, &y);
		if(x!=prevMouseX || y!=prevMouseY)
		  iteratePoint(x,y);
	      }
	    if(  event.key.keysym.sym==SDLK_RIGHT )
	      {
		diffMouseX-=0.001;
		SDL_GetMouseState(&x, &y);
		if(x!=prevMouseX || y!=prevMouseY)
		  iteratePoint(x,y);
	      }
	  
	    if(  event.key.keysym.sym==SDLK_c )
	      {
		normalize();
	      }
	    if(  event.key.keysym.sym==SDLK_m )
	      {
		Imag center;
		center.re = 0.0;
		center.im = 0.0;
		ictx.factor = 2.0;
		zoomPlot(&center);
		plotMandelbrot();
	    }
	    if(  event.key.keysym.sym==SDLK_d )
	    {
	      plotDensity();
	    }
	    if( event.key.keysym.sym==SDLK_s )
	      {
		char name[100];
		int x,y;
		FILE *f;
		/* This is the RWops structure we'll be using */
		SDL_RWops *rw;
		
		/* Here we get a file pointer to a bitmap */
		SDL_GetMouseState(&x, &y);
		sprintf(name,"shot(%.3f,%.3f).bmp",pixToRe(&ictx,x),pixToIm(&ictx,y));
		SDL_SaveBMP(screen,name);
	      }
	  }
      }
    if(done == 1) 
      {
	// Sulock(screen); 
	SDL_Quit();
	exit(0);
      }
  }
}



/*
   */
  int main(int argc,char** argv) 
  {
    struct imag origin;
    struct imag z, c, tmp, prev;
    SDL_Event event;
    int done = 0;
    double re, im;
    int iteration, plotx, ploty,i,j,color;
    int maxIterations = 100; 
    int rmask,gmask,bmask,amask;

    histogram = (double *)calloc(SCREEN_WIDTH*SCREEN_HEIGHT,sizeof(double));
    
    maxCount=0;
    ictx.max.re=2.0;
    ictx.min.re=-2.0;
    ictx.max.im=2.0;
    ictx.min.im=-2.0;
    ictx.factor=2.0;

    for(i=0; i<SCREEN_WIDTH ; i++)
    {
      for(j=0; j<SCREEN_HEIGHT ; j++)
      {
	histogram[i*SCREEN_HEIGHT + j] = 1.0;
      }
    }

    initMand();
    bbuffer = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT,
				   screen->format->BitsPerPixel,
				   screen->format->Rmask,
				   screen->format->Gmask,
				   screen->format->Bmask,
				   screen->format->Amask);
    
    plotMandelbrot();
    
    while(!done)
      pollMandEvents();
    return 0;
  }


