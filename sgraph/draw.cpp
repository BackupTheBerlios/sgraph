#include "draw.hh"

void Plotter::CreateColors(SGraphOptions *o)
{
  colors = new Color[o->NameCount];
  srandom(0x4242);
  int brightness=155;
  if(o->reverse)
    brightness=0;
  for(int i=0; i<o->NameCount ; i++)
  {

    int c=(int)floor(genrand()*3);
    if(c==0)
    {
      colors[i].r = 255;
      colors[i].g = (int)floor( 50*genrand() + brightness);
      colors[i].b = (int)floor( 50*genrand() + brightness);
    }
    if(c==1)
    {
      colors[i].g = 255;
      colors[i].r = (int)floor( 50*genrand() + brightness);
      colors[i].b = (int)floor( 50*genrand() + brightness);
    }

    if(c==2)
    {
      colors[i].b = 255;
      colors[i].r = (int)floor( 50*genrand() + brightness);
      colors[i].g = (int)floor( 50*genrand() + brightness);
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

  if(o->reverse)
  {
    fg->r=0;
    fg->g=0;
    fg->b=0;
    fg->a=200;
    
    bg->r=255;
    bg->g=255;
    bg->b=255;
    bg->a=255;
  }
  else 
  {
    fg->r=255;
    fg->g=255;
    fg->b=255;
    fg->a=70;
    
    bg->r=0;
    bg->g=0;
    bg->b=0;
    bg->a=255;
  }

  plotCount=0;
  dirty=1;
}

//! This is currently thread safe to accomodate SDLPlotter, this will be changed with a more basic,
// get data and plot functionality when we write another driver.
void Plotter::PlotData(Data *d, View *v)
{
  Point **points;
  Point *p = new Point();
  Point *lastPoint = new Point();
  int c;
  int slice=10000;
  int morePoints=1;
  int gotN=0;

  QuitPlotting();
  InitPlot(d);  
  continuePlotting=1;
  
  /* plot by slice */
  while(morePoints > 0 && continuePlotting)
  {
    c=0;
    morePoints=0;
    for(int j=0; j<opts->NameCount ; j++)
    {
      while(d->MorePoints(j) && c < slice && continuePlotting)
      {
	d->ReadPoint(j);
	gotN++;
      }
      morePoints += d->MorePoints(j);
    }
    
    if(v==NULL)
      graphics->view = d->GetDefaultView();
    else
      graphics->view = v;
    
    if(gotN < 1 && !dirty)
    {
      PlotFinished(d);
      return;
    }

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
	if(i%10000 == 0 && dirty)
	  graphics->Updated();
      }
    }
    graphics->Updated();
    if(continuePlotting)
      dirty=0;
  }
  plotCount++;
  PlotFinished(d);
}

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

  expX = (int)floor(log10(MAX(fabs(minX),fabs(maxX)))/3.0);
  expY = (int)floor(log10(MAX(fabs(minY),fabs(maxY)))/3.0);

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

  for(double x=xstart; x < maxXe && xstepRound>0  ; x+= xstepRound )
  {

    *p1->x=x*pow(10,expX);
    *p1->y=minY;
    *p2->x=x*pow(10,expX);
    *p2->y=maxY;
    graphics->drawLine(p1,p2,fg); 

    sprintf(text,"%3g",x);

    graphics->drawText(text, p1, fg, ALIGN_CENTER, ALIGN_TOP);

    //fprintf(stdout,"X axis %g %g %d\n",x,x*pow(10,expX),expX);
  }
  for(double y=ystart; y < maxYe && ystepRound > 0 ; y+= ystepRound )
  {
    *p1->x=minX;
    *p1->y=y*pow(10,expY);

    *p2->x=maxX;
    *p2->y=y*pow(10,expY);
    graphics->drawLine(p1,p2,fg);

    sprintf(text,"%3g",y);
    graphics->drawText(text, p1, fg, ALIGN_RIGHT, ALIGN_CENTER);

    // fprintf(stdout,"Y axis %g %g %d\n",y,y*pow(10,expY),expY);
  }
}



