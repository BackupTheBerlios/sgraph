#include "data.hh"

DataFile::DataFile(char *n)
{
  minX=0;
  minY=0;
  maxX=0;
  maxY=0;
  RowCount=0;
  scalingFactorX=1.0;
  scalingFactorY=1.0;
  logX=0;
  logY=0;

  allocated=1000;
  eofReached=0;
  localCount=0;

  points = (Point **)calloc(allocated,sizeof(Point *));
  for(int i=0; i<allocated ; i++)
  {
    points[i] = new Point();
  }
  name = n;
  OpenFile();

  charCounter=0;
}

void DataFile::OpenFile()
{
  if(!strcmp(name,"-") || !strcmp(name,"stdin"))
  {
    fprintf(stdout,"using stdin\n");
    handle=stdin;
  }
  else
    handle = fopen(name,"r");
}

void DataFile::CloseFile()
{
  if(!(!strcmp(name,"-") || !strcmp(name,"stdin")))
  {
    fclose(handle);
  }
}

void DataFile::Reset()
{
  CloseFile();
  OpenFile();
  RowCount=0;
  eofReached=0;
}

DataFile::~DataFile()
{
  CloseFile();
  free(points);
}

int DataFile::MoreData()
{
  if(eofReached == 0)
    return 1;
  else 
    return 0;
}

/*
  read one point from file in succsessive manner 
  todo, multicolumn
 */
Point *DataFile::ReadRow()
{
  char l[2000]; // 2000 should be enough for everyone
  char *line;
  int readCount=0;
  double x,y;
  char c;

  // we have to plot something every now and then
  while(charCounter<2000)
  {
    c=fgetc(handle);
    if(c==EOF || c==NULL)
    {
      eofReached=1;
      // wait a while and return, or return for good, your choice...
      return NULL;
    }
    l[charCounter++]=c;

    if(c=='\n' || charCounter==1999)
    {
      l[charCounter]='\0';
      charCounter=0;
      break;
    }
  }

  /* row must end with return */
  readCount=sscanf(l,"%lf %lf", &x, &y);
  
  if(readCount==2)
  {
    if(RowCount==(allocated-1))
    {
      int prevA = allocated;
      allocated+=1000;
      points=(Point **)realloc(points,allocated*sizeof(Point *));
      for(int t=prevA ; t<allocated ; t++)
      {
	points[t] = new Point();
      }
    }
    points[RowCount]->x = x;
    points[RowCount]->y = y;
    
    if(RowCount == 0)
    {
      minX=x;
      minY=y;
      maxX=x;
      maxY=y;
    }

    if(minX>x)
      minX=x;
    if(maxX<x)
      maxX=x;
    if(minY>y)
      minY=y;
    if(maxY<y)
      maxY=y;

    RowCount++;
    return points[RowCount];
  } 
  else 
  {
    fprintf(stderr,"Ignoring malformed row: %s\n",l);
    eofReached=1;
    return NULL;
  }
}

// get all data read until now
Point **DataFile::GetData()
{
  return points;
}

int DataFile::GetRowCount()
{
  return RowCount;
}

Data::Data(SGraphOptions *o)
{
  opts = o;
  dataFiles = (DataFile **)calloc(opts->NameCount,sizeof(DataFile *));

  // open files.. (- || stdin == stdin)
  for(int i=0; i<opts->NameCount ; i++)
  {
    dataFiles[i] = new DataFile(opts->FileNames[i]);
  }
  defaultView = new View();
}

Data::~Data() 
{
  // open files.. (- || stdin == stdin)
  for(int i=0; i<opts->NameCount ; i++)
  {
    delete dataFiles[i];
  }
}

Point *Data::ReadPoint(int col)
{
  // if multicol, read column, else read 
  return(dataFiles[col]->ReadRow());
}


int Data::MorePoints(int col)
{
  // if multicol, read column, else read 
  return(dataFiles[col]->MoreData());
}

Point **Data::GetPoints(int col)
{
  // if multicol, return data for column
  return(dataFiles[col]->GetData());
}

int Data::GetRowCount(int col)
{
  // if multicol, return count for correct column
  return(dataFiles[col]->GetRowCount());
}

DataFile **Data::GetDataFiles()
{
  return dataFiles;
}

void Data::ResetData()
{
  for(int i=0; i<opts->NameCount ; i++)
  {
    dataFiles[i]->Reset();
  }
}

void Data::SetEofReached(int e)
{
  for(int i=0; i<opts->NameCount ; i++)
  {
    dataFiles[i]->eofReached=e;
  }
}


View *Data::GetDefaultView()
{
  defaultView->ll->x = dataFiles[0]->minX;
  defaultView->ll->y = dataFiles[0]->minY;
  defaultView->ur->x = dataFiles[0]->maxX;
  defaultView->ur->y = dataFiles[0]->maxY;

  for(int i=1; i<opts->NameCount ; i++)
  {
    if(dataFiles[i]->minX < defaultView->ll->x)
    {
      defaultView->ll->x = dataFiles[i]->minX;
    }

    if(dataFiles[i]->minY < defaultView->ll->y)
    {
      defaultView->ll->y = dataFiles[i]->minY;
    }

    if(dataFiles[i]->maxX > defaultView->ur->x)
      defaultView->ur->x = dataFiles[i]->maxX;

    if(dataFiles[i]->maxY > defaultView->ur->y)
      defaultView->ur->y = dataFiles[i]->maxY;
  }
  return defaultView;
}
