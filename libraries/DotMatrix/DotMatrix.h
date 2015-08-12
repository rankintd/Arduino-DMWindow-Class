#ifndef DotMatrix_h
#define DotMatrix_h
#ifdef __cplusplus

#include "elapsedMillis.h"

const int nROWS = 8;
const int nCOLS = 8;

const int DM_DEFAULT_CONTRAST = 5;

const int PINS[nROWS] = {10, 11, 12, 13, 14, 15, 16, 17};   //The pins used to control the anode of the display

typedef short ElemType;
ElemType whoops=0;						//dummy element accessed when array indices are out of bounds

class DotMatrix
{
protected:
	ElemType theArray[nROWS][nCOLS];
	
public:
	DotMatrix();
	DotMatrix(const ElemType arr[][nCOLS]);

	void showRow(int rowNo, int positiveOnly) const;		//make private/protected, possibly

	void showAll() const;
	void showAll(unsigned int millisec) const;
	void show(unsigned int millisec) const;
	void show(unsigned int millisec, int contrast) const;
	
	void clear();
	void setTo(const ElemType arr[][nCOLS]);

	void wrap(int xShift, int yShift);
	void swapRows(int rowA, int rowB);
	void swapCols(int colA, int colB);

	void setRow(int rowNo, ElemType val);
	void setCol(int colNo, ElemType val);

	void reorient(int dir);

	void rotate90()				{if(nROWS==nCOLS) reorient(1);}
	void rotate180()			{reorient(2);}
	void rotate270()			{if(nROWS==nCOLS) reorient(3);}
	void transpose()			{if(nROWS==nCOLS) reorient(7);}
	void flipVert()				{reorient(6);}
	void flipAntidiag()			{if(nROWS==nCOLS) reorient(5);}
	void flipHoriz()			{reorient(4);}

	//ObjectName(a, b) returns a reference to theArray[a][b] (or to a dummy element if out of range)
	ElemType& operator()(int row, int col)
	{
		if(row < 0 || row >= nROWS || col < 0 || col >= nCOLS) return whoops;
		return theArray[row][col];
	}
};

//default constructor
DotMatrix::DotMatrix()
{
	clear();
}

//constructor
//***ASSUMES THE ARGUMENT PASSED IN HAS AT LEAST nROWS ROWS***
DotMatrix::DotMatrix(const ElemType arr[][nCOLS])
{
	setTo(arr);
}

//outputs the indicated row to the LED board based on the contents of theArray.
//if positiveOnly is nonzero (true), only displays LEDs which correspond to positive values in the array.
//if positiveOnly is zero (false), displays LEDs which correspond to all nonzero values in the array
void DotMatrix::showRow(int rowNo, int positiveOnly) const
{
	if(rowNo < 0 || rowNo >= nROWS) return;

	int j;
	for (j=0; j<nCOLS; j++)
	{
		if ( theArray[rowNo][j]>0 || (!positiveOnly && theArray[rowNo][j]<0) )
			digitalWrite(2+j, LOW);			//if display appears rotated 180 degrees, replace 2+j with 9-j
		else
			digitalWrite(2+j, HIGH);		//ditto
	}
	digitalWrite(PINS[rowNo], HIGH);
	delayMicroseconds(400);
}

//briefly displays the board represented by theArray (by calling showRow once for each row).
//ignores signs of entries; all LEDs have the same brightness
void DotMatrix::showAll() const
{
	int k;
	for(k=0; k<nROWS; k++)
	{
		showRow(k, 0);
		digitalWrite(PINS[k], LOW);
	}
}

//displays the board represented by theArray for (at least) the specified number of milliseconds.
//ignores signs of entries; all LEDs have the same brightness
void DotMatrix::showAll(unsigned int millisec) const
{
	elapsedMillis eM;
	while(eM < millisec) showAll();
}

//see comment to function below
void DotMatrix::show(unsigned int millisec) const
{
	show(millisec, DM_DEFAULT_CONTRAST);
}

//displays the board represented by theBoardArray for (at least) the specified number of milliseconds.
//negative-valued LEDs "blink" to reduce their brightness relative to positive-valued LEDs
void DotMatrix::show(unsigned int millisec, int contrast) const
{
	int i, k;
	elapsedMillis eM;
	while(eM < millisec)
	{
		showAll();						//show all nonzero-valued LEDs
		
		for(i=1; i<=contrast; i++)		//show only the positive-valued LEDs
			for(k=0; k<nROWS; k++)
			{
				showRow(k, 1);
				digitalWrite(PINS[k], LOW);
			}
	}
}

//sets all entries in theArray to zero (corresponding to turning off all LEDs)
void DotMatrix::clear()
{
	int i, j;
	for(i=0; i<nROWS; i++)
		for(j=0; j<nCOLS; j++)
			theArray[i][j] = 0;
}

//sets the entries in theArray to match the entries in the argument array
//***ASSUMES THE ARGUMENT PASSED IN HAS AT LEAST nROWS ROWS***
void DotMatrix::setTo(const ElemType arr[][nCOLS])
{
	int i, j;
	for(i=0; i<nROWS; i++)
		for(j=0; j<nCOLS; j++)
			theArray[i][j] = arr[i][j];
}

//shifts the values of theArray by the specified values, wrapping around when passing the edge.
//the value originally in theArray[0][0] will end up in theArray[vShift][hShift] (for indices in range)
void DotMatrix::wrap(int vShift, int hShift)
{
	ElemType temp[nROWS][nCOLS];
	int i, j;
	for(i=0; i<nROWS; i++)
		for(j=0; j<nCOLS; j++)
			temp[i][j] = theArray[i][j];

	for(i=0; i<nROWS; i++)
		for(j=0; j<nCOLS; j++)
		{
			int r = (i+vShift) % nROWS;
			if(r<0) r+=nROWS;

			int c = (j+hShift) % nCOLS;
			if(c<0) c+=nCOLS;

			theArray[r][c] = temp[i][j];
		}
}

//swaps the contents of the specified rows.
//(does nothing if an index is out of range)
void DotMatrix::swapRows(int rowA, int rowB)
{
	if(rowA < 0 || rowA >= nROWS || rowB < 0 || rowB >= nROWS || rowA == rowB) return;

	ElemType temp[nCOLS];
	int i;
	for(i=0; i<nCOLS; i++)
	{
		temp[i] = theArray[rowA][i];
		theArray[rowA][i] = theArray[rowB][i];
		theArray[rowB][i] = temp[i];
	}
}

//swaps the contents of the specified columns.
//(does nothing if an index is out of range)
void DotMatrix::swapCols(int colA, int colB)
{
	if(colA < 0 || colA >= nCOLS || colB < 0 || colB >= nCOLS || colA == colB) return;

	ElemType temp[nROWS];
	for(int i=0; i<nROWS; i++)
	{
		temp[i] = theArray[i][colA];
		theArray[i][colA] = theArray[i][colB];
		theArray[i][colB] = temp[i];
	}
}

//sets all values in the specified row to the indicated value.
//(does nothing if the row index is out of range)
void DotMatrix::setRow(int rowNo, ElemType val)
{
	if(rowNo < 0 || rowNo >= nROWS) return;
	
	for(int i=0; i<nCOLS; i++)
		theArray[rowNo][i] = val;
}

//sets all values in the specified column to the indicated value.
//(does nothing if the column index is out of range)
void DotMatrix::setCol(int colNo, ElemType val)
{
	if(colNo < 0 || colNo >= nCOLS) return;

	for(int i=0; i<nCOLS; i++)
		theArray[i][colNo] = val;
}

//rearranges the values in theArray, corresponding to a rotation/reflection of the LED board
//(does nothing if certain transformations are applied to a non-square matrix)
void DotMatrix::reorient(int dir)
{
	if(dir <= 0 || dir >= 8) return;
	if((nROWS != nCOLS) && (dir % 2)) return;

	ElemType temp[nROWS][nCOLS];
	int i, j;
	for(i=0; i<nROWS; i++)
		for(j=0; j<nCOLS; j++)
			temp[i][j] = theArray[i][j];

	switch(dir)							//case 7-n is the transpose of case n
	{
	case 1:		//rotate90
		for(i=0; i<nROWS; i++)
			for(j=0; j<nCOLS; j++)
				theArray[i][j] = temp[nCOLS-1-j][i];
		break;
	case 2:		//rotate180
		for(i=0; i<nROWS; i++)
			for(j=0; j<nCOLS; j++)
				theArray[i][j] = temp[nROWS-1-i][nCOLS-1-j];
		break;
	case 3:		//rotate270
		for(i=0; i<nROWS; i++)
			for(j=0; j<nCOLS; j++)
				theArray[i][j] = temp[j][nROWS-1-i];
		break;
	case 4:		//flipHoriz
		for(i=0; i<nROWS; i++)
			for(j=0; j<nCOLS; j++)
				theArray[i][j] = temp[i][nCOLS-1-j];
		break;
	case 5:		//flipAntidiag
		for(i=0; i<nROWS; i++)
			for(j=0; j<nCOLS; j++)
				theArray[i][j] = temp[nCOLS-1-j][nROWS-1-i];
		break;
	case 6:		//flipVert
		for(i=0; i<nROWS; i++)
			for(j=0; j<nCOLS; j++)
				theArray[i][j] = temp[nROWS-1-i][j];
		break;
	case 7:		//transpose
		for(i=0; i<nROWS; i++)
			for(j=0; j<nCOLS; j++)
				theArray[i][j] = temp[j][i];
		break;
	}
}

#endif
#endif