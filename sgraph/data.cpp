#include "data.hh"

DataFile::DataFile(char *name)
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

  points = calloc(allocated,sizeof(Point));

  if(streq(name,"-") || streq(name,"stdin"))
    handle=stdin;
  else
    handle = fopen(name,"r");
}

DataFile::~DataFile()
{
  fclose(handle);
  free(points);
}

void DataFile::SetLogarithmic()
{
  // todo
}

/*
  read one point from file in succsessive manner 
  todo, multicolumn
 */
Point DataFile::ReadRow()
{
  char l[2000]; // 2000 should be enough for everyone
  int readCount=0;
  double x,y;

  l = fgets(line, 2000, handle);
  readCount=sscanf(token,"%lf %lf", &x, &y);
  
  if(readCount==2)
  {
    if(rowCount==allocated)
    {
      allocated+=1000;
      realloc(points,allocated*sizeof(Point));
    }
    points[rowCount]->x = x;
    points[rowCount]->y = y;
    
    if(rowCount == 1)
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

    return points[rowCount];

    rowCount++;
  } 
  else 
  {
    fprintf(stderr,"Ignoring malformed row: %s\n",token);
    return NULL;
  }
}

// get all data read until now
Point *DataFile::GetData()
{
  return points;
}

int GetRowCount()
{
  return rowCount;
}

Data::Data(SGraphOptions o)
{
  opts = o;

  // open files.. (- || stdin == stdin)
  for(int i=0; i<opts->NameCount ; i++)
  {
    dataFile[i] = new DataFile(opts->FileNames[i]);
  }
}

Data::~Data() 
{
  // open files.. (- || stdin == stdin)
  for(int i=0; i<opts->NameCount ; i++)
  {
    delete dataFile[i];
  }
}

Point Data::ReadPoint(int col)
{
  // if multicol, read column, else read 
  return(dataFiles[col]->RowRow());
}

Point *Data::GetPoints(int col)
{
  // if multicol, return data for column
  return(dataFiles[col]->GetData());
}

int Data::GetRowCount(int col)
{
  // if multicol, return count for correct column
  return(dataFiles[col]->GetRowCount());
}

DataFile *Data::GetDataFiles()
{
  return dataFiles;
}

View Data::GetDefaultView()
{
  defailtView->ll->x = dataFile[0]->minX;
  defaultView->ll->y = dataFile[0]->minY;
  defaultView->ur->x = dataFile[0]->maxX;
  defaultView->ur->y = dataFile[0]->maxY;

  for(int i=1; i<opts->NameCount ; i++)
  {
    if(dataFile[i]->minX < defaultView->ll->x)
      defaultView->ll->x = dataFile[i]->minX;

    if(dataFile[i]->minY < defaultView->ll->y)
      defaultView->ll->y = dataFile[i]->minY;

    if(dataFile[i]->maxX > defaultView->ur->x)
      defaultView->ur->x = dataFile[i]->maxX;

    if(dataFile[i]->maxY > defaultView->ur->y)
      defaultView->ll->y = dataFile[i]->maxY;
  }
  return defaultView;
}
