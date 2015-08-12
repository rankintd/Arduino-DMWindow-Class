#include<DMWindow.h>
#include<DotMatrix.h>
#include<elapsedMillis.h>

const float XMIN = 0;
const float XMAX = 100;
const float YMIN = 0;
const float YMAX = 100;

DMWindow theWindow(XMIN, XMAX, YMIN, YMAX, 1);

const int CONTRAST = 5;
const int DELAY_TIME = 20;

const int nPOINTS = 4;

struct MovingPoint
{
  float x;
  float y;
  float xPrime;             //units per second
  float yPrime;
  //(written before DMWindow.h included a Point struct)
};

MovingPoint P0 = {30, 30, 90, 20};
MovingPoint P1 = {40, 100, 50, -20};
MovingPoint P2 = {100, 60, -10, -50};
MovingPoint P3 = {60, 0, -50, 30};

//for rotational symmetry:
//MovingPoint P1 = {P0.y, XMAX-P0.x, P0.yPrime, -P0.xPrime};
//MovingPoint P2 = {XMAX-P0.x, YMAX-P0.y, -P0.xPrime, -P0.yPrime};
//MovingPoint P3 = {YMAX-P0.y, P0.x, -P0.yPrime, P0.xPrime};

MovingPoint Points[nPOINTS] = {P0, P1, P2, P3};

elapsedMillis elps;

void setup()
{
  //Serial.begin(9600);

  int i;
  for (i=2; i<=17; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }
}

void loop()
{
  int k;
  theWindow.clear();

  updatePoints();

  for(k=0; k<nPOINTS; k++)
      theWindow.drawDimLine(Points[(k+1)%nPOINTS].x, Points[(k+1)%nPOINTS].y, Points[k].x, Points[k].y);

  for(k=0; k<nPOINTS; k++)
    theWindow.drawPoint(Points[k].x, Points[k].y);

  //theWindow.zoomTo(Points[0].x, Points[0].y, 1), 
  theWindow.show(DELAY_TIME, CONTRAST);

}

void updatePoints()
{
  float dt = elps;
  elps = 0;
  
  int i;
  for(i=0; i<nPOINTS; i++)
  {
    Points[i].x += Points[i].xPrime*dt/1000;
    Points[i].y += Points[i].yPrime*dt/1000;

    //bounce off edges
    if(Points[i].x < XMIN && Points[i].xPrime < 0) Points[i].xPrime *= -1;
    if(Points[i].x > XMAX && Points[i].xPrime > 0) Points[i].xPrime *= -1;
    
    if(Points[i].y < YMIN && Points[i].yPrime < 0) Points[i].yPrime *= -1;
    if(Points[i].y > YMAX && Points[i].yPrime > 0) Points[i].yPrime *= -1;
  }
}

