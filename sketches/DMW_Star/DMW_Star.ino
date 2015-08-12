#include<DMWindow.h>
#include<DotMatrix.h>
#include<elapsedMillis.h>

const float INIT=10;

DMWindow theWindow(-INIT, INIT, -INIT, INIT, 0);

const int CONTRAST = 5;
const float RPS = .4;          //revolutions per second (approximate)

const int nPOINTS = 5;

Point points[nPOINTS] = {{11, 0}, {4, 10}, {-8, 7}, {-8, -7}, {4, -10}};

elapsedMillis elps;
int count=0;
float zoom_factor = 1.02;
int k;

void setup()
{
  //Serial.begin(9600);

  int i = 2;
  //Set D2-D17 as OUTPUT
  for (i = 2; i < 18; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void loop()
{
  theWindow.clear();

  updatePoints();
  for(k=0; k<nPOINTS; k++) 
    theWindow.drawDimLine(points[k],points[(k+2)%nPOINTS]);
  for(k=0; k<nPOINTS; k++)
    theWindow.drawPoint(points[k]);
  count++;
  theWindow.zoom(zoom_factor);
  if(count%100 == 50) zoom_factor = 2-zoom_factor;

  theWindow.show(2, CONTRAST);
}

void updatePoints()
{
  float dt = elps;
  elps=0;

  float rad = dt*RPS/159;       // (2pi)/1000 is approximately 1/159
  int i;
  for(i=0; i<nPOINTS; i++)
  {
    points[i].x += -points[i].y*rad;
    points[i].y += points[i].x*rad;
  }
}
