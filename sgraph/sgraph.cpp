#include "draw.hh"
#include "options.hh"
#include "data.hh"

int main(int argc, char **argv)
{
  SGraphOptions opts=new SGraphOptions();
  opts->parseOpts(argc,argv);
  
  Data d=new Data(opts);

  SDLPlotter plotter = new Plotter(d,opts);
  SDLGraphics graphics = plotter->getGraphics();

  View customView = new View();
  while(true)
  {
    int pid=fork();
    if(pid==0)
      plotter->plotData();

    while(events.getEvent())
    {
      // if esc, exit
      // if leftclick events,
      //    draw selection and set view
      //    customView->ll
      //    customView->ur
      //    plotter->setView(customView);
      // if rightclick
      //    plotter->setView(d->getDefaultView());
    }
  }
}
  //   
  //   
  //   if enough time passed:
  //     if(follow || update)
  //         data->ReadMoreData();
  //         if(UserDefinedView == null) 
  //            get data->defaultview
  //       
  //   sniff events
  //      if mouse clicked, start zoom stuff
  //      if rightclick, data->getDefaultView
  //      if update click, data->ReadMoreData();
}
