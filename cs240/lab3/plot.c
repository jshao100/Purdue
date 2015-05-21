
#include <stdio.h>
#include <stdlib.h>

#include "rpn.h"

#define MAXCOLS 80
#define MAXROWS 40

char plot[MAXROWS][MAXCOLS];

void clearPlot()
{
  for (int i = 0; i < MAXROWS; i++) {
    for (int j = 0; j < MAXCOLS; j++) {
      plot[i][j] = ' ';
    }
  }
}

void printPlot()
{
  for (int i = 0; i < MAXROWS; i++) {
    for (int j = 0; j < MAXCOLS; j++) {
      printf("%c", plot[i][j]);
    }
    printf("\n");
  }
}

void plotXY(int x, int y, char c) {
  if ( x <0 || x>=MAXCOLS || y < 0 || y >=MAXROWS) return;
  plot[y][x]=c;
}

void createPlot( char * funcFile, double minX, double maxX) {
  	int nvals = MAXCOLS;
  	double yy[MAXCOLS];

  	clearPlot();

  	// Plot function. Use scaling.
  	// minX is plotted at column 0 and maxX is plotted ar MAXCOLS-1
  	// minY is plotted at row 0 and maxY is plotted at MAXROWS-1


	//get intervals
	double step = (maxX - minX) / MAXCOLS;
	double j = minX;

	//find step
	for(int i = 0; i < MAXCOLS; i++) {
		j += step;
		yy[i] = rpn_eval(funcFile, j);		
		//printf("%lf %lf\n", j, yy[i]);
	}

	double minY = yy[0];
	double maxY = yy[0];

	//find min and max Y
	for(int i = 0; i < MAXCOLS; i++) {
		if(minY > yy[i])	minY = yy[i];
		if(maxY < yy[i])	maxY = yy[i];
	}

	//printf("max: %lf, min: %lf\n", maxY, minY);

	double y_index;	

	//plot X axis
	if (minY >= 0) { //axis at bottom
		for(int i = 0; i < MAXCOLS; i++)	plot[MAXROWS-1][i] = '_';
	}
	else if (maxY < 0) { //axis at top
		for(int i = 0; i < MAXCOLS; i++)	plot[0][i] = '_';
	}
	else { //axis in middle
		for(int i = 0; i < MAXCOLS; i++)	plot[20][i] = '_';
	}

	//plot Y axis
	if (minX >= 0) {
		for(int i = 0; i < MAXROWS; i++)	plot[i][0] = '|';
	}
	else if (maxX < 0) {
		for(int i = 0; i < MAXROWS; i++)	plot[i][MAXCOLS-1] = '|';
	}
	else {
		for(int i = 0; i < MAXROWS; i++)	plot[i][40] = '|';
	}

	//print plot
	int val;	
	for(int i = 0; i < MAXCOLS; i++) {
		val = (yy[i] - minY)/(maxY - minY) * MAXROWS;	
		val = MAXROWS - val - 1;	
		plot[val][i] = '*';
	}

/*
 *	calculate y index: y = (yy[i] - minY)/(maxY-minY) * MAXROWS
 *	y = MAXROWS - y - 1
 *
 *
 */

  printPlot();

}

int main(int argc, char ** argv)
{
  printf("RPN Plotter.\n");
  
  if (argc < 4) {
    printf("Usage: plot func-file xmin xmax\n");
    exit(1);
  }

  // Get arguments
  	double minX = atof(argv[2]);
  	double maxX = atof(argv[3]);
	
  	createPlot(argv[1], minX, maxX);
}
