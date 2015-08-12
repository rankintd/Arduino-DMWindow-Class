#ifndef DMWindow_h
#define DMWindow_h
#ifdef __cplusplus

#include "DotMatrix.h"

struct Point
{
	float x;
	float y;
};

class DMWindow: public DotMatrix
{
private:
	float xMin;
	float deltaX;	//change in x corresponding to moving one LED over
	float yMin;
	float deltaY;	//change in y corresponding to moving one LED up/down

	int yInverted;	//when nonzero (true), higher y-values correspond to lower points (higher array indices) on the LED board

public:
	DMWindow();
	DMWindow(float x_Min, float x_Max, float y_Min, float y_Max, int y_Inv);

	void setWindow(float x_Min, float x_Max, float y_Min, float y_Max);
	void setDeltas(float delt_X, float delt_Y);
	void shiftWindow(float x_Shift, float y_Shift);

	float getxMin()	const			{return xMin;}
	float getxMax()	const			{return xMin + nCOLS*deltaX;}
	float getyMin()	const			{return yMin;}
	float getyMax()	const			{return yMin + nROWS*deltaY;}
	float getDeltaX() const			{return deltaX;}
	float getDeltaY() const			{return deltaY;}

	void zoomMin(float zoom_factor);
	void zoomTo(float xCen, float yCen, float zoom_factor);
	void zoomTo(Point cen, float zoom_factor);
	void zoom(float zoom_factor);

	int drawPoint(float xVal, float yVal, int sign);
	int drawPoint(float xVal, float yVal);
	int drawDimPoint(float xVal, float yVal);

	void drawLine(float x1, float y1, float x2, float y2, int sign);
	void drawLine(float x1, float y1, float x2, float y2);
	void drawDimLine(float x1, float y1, float x2, float y2);
	void animateLine(float x1, float y1, float x2, float y2);

	int drawPoint(Point pt, int sign)				{return drawPoint(pt.x, pt.y, sign);}
	int drawPoint(Point pt)							{return drawPoint(pt.x, pt.y);}
	int drawDimPoint(Point pt)						{return drawDimPoint(pt.x, pt.y);}

	void drawLine(Point pt1, Point pt2, int sign)	{drawLine(pt1.x, pt1.y, pt2.x, pt2.y, sign);}
	void drawLine(Point pt1, Point pt2)				{drawLine(pt1.x, pt1.y, pt2.x, pt2.y);}
	void drawDimLine(Point pt1, Point pt2)			{drawDimLine(pt1.x, pt1.y, pt2.x, pt2.y);}
	void animateLine(Point pt1, Point pt2)			{animateLine(pt1.x, pt1.y, pt2.x, pt2.y);}
};

//default constructor
DMWindow::DMWindow()
{
	clear();

	deltaX = 1;
	deltaY = 1;
	xMin=0;
	yMin=0;
	yInverted=0;
}

//constructor
//(setWindow handles input protection)
DMWindow::DMWindow(float x_Min, float x_Max, float y_Min, float y_Max, int y_Inv)
{
	clear();
	setWindow(x_Min, x_Max, y_Min, y_Max);
	yInverted = y_Inv;
}

//sets the window area.
//the values of x_Min and y_Min will be used, but deltaX and deltaY will be made positive 
void DMWindow::setWindow(float x_Min, float x_Max, float y_Min, float y_Max)
{
	xMin = x_Min;
	yMin = y_Min;

	setDeltas((x_Max-x_Min)/nCOLS, (y_Max-y_Min)/nROWS);
}

//sets deltaX and deltaY to the specified values (but ensuring their value is positive)
//resets deltas to 1 if the values passed in are too extreme
void DMWindow::setDeltas(float delt_X, float delt_Y)
{
	if(delt_X < 0) delt_X *= -1;
	if(delt_X <.000000000000001 || delt_X > 99999999999999)
	{
		delt_X = 1;
		delt_Y = 1;
	}

	if(delt_Y < 0) delt_Y *= -1;
	if(delt_Y <.000000000000001 || delt_Y > 99999999999999)
	{
		delt_X = 1;
		delt_Y = 1;
	}

	deltaX = delt_X;
	deltaY = delt_Y;
}

//shifts the window bounds by the specified amounts (without changing zoom level)
void DMWindow::shiftWindow(float x_Shift, float y_Shift)
{
	xMin += x_Shift;
	yMin += y_Shift;
}

//zooms in by the specified zoom factor while preserving xMin and yMin
void DMWindow::zoomMin(float zoom_factor)
{
	if(zoom_factor <= 0) return;
	setDeltas(deltaX/zoom_factor, deltaY/zoom_factor);
}

//zooms in to the specified new center point by the specified zoom factor
void DMWindow::zoomTo(float xCen, float yCen, float zoom_factor)
{
	if(zoom_factor <= 0) return;
	setDeltas(deltaX/zoom_factor, deltaY/zoom_factor);

	xMin = xCen - (nCOLS/2.0)*deltaX;
	yMin = yCen - (nROWS/2.0)*deltaY;
}

//zooms in to the specified new center point by the specified zoom factor
void DMWindow::zoomTo(Point cen, float zoom_factor)
{
	return zoomTo(cen.x, cen.y, zoom_factor);
}

//zooms in to the current center by the specified zoom factor
void DMWindow::zoom(float zoom_factor)
{
	float xCenter = xMin + (nCOLS/2.0)*deltaX;
	float yCenter = yMin + (nROWS/2.0)*deltaY;

	zoomTo(xCenter, yCenter, zoom_factor);
}

//turns on the LED corresponding to the point with the specified coordinates.
//a negative value for sign corresponds to a dim (flashing) point
int DMWindow::drawPoint(float xVal, float yVal, int sign)
{
	if(xVal<xMin || xVal>getxMax() || yVal<yMin || yVal>getyMax()) return 0;

	int col = int((xVal - xMin)/deltaX);
	int row = int((yVal - yMin)/deltaY);

	if(col<0 || col>nCOLS-1 || row<0 || row>nROWS-1) return 0;

	if(yInverted)
		theArray[row][col]=sign;
	else theArray[nROWS-1-row][col]=sign;

	return 1;
}

int DMWindow::drawPoint(float xVal, float yVal)
{
	return drawPoint(xVal, yVal, 1);
}

int DMWindow::drawDimPoint(float xVal, float yVal)
{
	return drawPoint(xVal, yVal, -1);
}

//draws a line connecting the two specified points (by plotting a series of points at once).
//a negative value for sign corresponds to a dim (flashing) point
void DMWindow::drawLine(float x1, float y1, float x2, float y2, int sign)
{
	int num_intervals = 20;					//one less than the number of points plotted
	float dx = (x2-x1)/num_intervals;
	float dy = (y2-y1)/num_intervals;

	for(int i=0; i<=num_intervals; i++)
	{
		drawPoint(x1 + i*dx, y1 + i*dy, sign);
	}
}

void DMWindow::drawLine(float x1, float y1, float x2, float y2)
{
	return drawLine(x1, y1, x2, y2, 1);
}

void DMWindow::drawDimLine(float x1, float y1, float x2, float y2)
{
	return drawLine(x1, y1, x2, y2, -1);
}

//draws a line from the first point to the second one at a visible speed.
//(typically animation will be better handled by drawing points in the Arduino "loop"
//function, not by a function like this one which holds up the program)
void DMWindow::animateLine(float x1, float y1, float x2, float y2)
{
	int num_intervals = 20;
	float dx = (x2-x1)/num_intervals;
	float dy = (y2-y1)/num_intervals;

	for(int i=0; i<=num_intervals; i++)
	{
		drawPoint(x1 + i*dx, y1 + i*dy, 1);
		show(100);
	}
}

#endif // __cplusplus
#endif // DMWindow_h