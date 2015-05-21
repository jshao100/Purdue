
#include <stdio.h>

#include "array.h"

// Return sum of the array
double sumArray(int n, double * array) {
  double sum = 0;
  
  double * p = array;
  double * pend = p+n;

  while (p < pend) {
    sum += *p;
    p++;
  }

  return sum;
}

// Return maximum element of array
double maxArray(int n, double * array) {
	double max = 0;

	double *p = array;
	double *p_end = p+n;

	max = *p;	

	while (p < p_end) {
		if (*p > max)	max = *p;
		p++;
	}	
  return max;
}

// Return minimum element of array
double minArray(int n, double * array) {
	double min = 0;

	double *p = array;
	double *p_end = p+n;

	min = *p;

	while (p < p_end) {
		if (*p < min)	min = *p;
		p++;
	}
	return min;
}

// Find the position int he array of the first element x 
// such that min<=x<=max or -1 if no element was found
int findArray(int n, double * array, double min, double max) {
  
	int pos = 0;

	double *p = array;
	double *p_end = p+n;

	while (p < p_end) {
		if (*p >= min && *p <= max) {
			return pos; 
		}
		pos++;
		p++;
	}
   
 	return -1;
}

// Sort array without using [] operator. Use pointers 
// Hint: Use a pointer to the current and another to the next element
int sortArray(int n, double * array) {

	int i, j, tmp;

	for (i = 0; i < n; i++) {
		for (j = i+1; j < n; j++) {
			if (*(array+j) < *(array+i)) {
				tmp = *(array+j);
				*(array+j) = *(array+i);
				*(array+i) = tmp;
			}
		}
	}

}

// Print array
void printArray(int n, double * array) {
	
	int pos = 0;

	double *p = array;
	double *p_end = p+n;

	while (p < p_end) {
		printf("%d:%lf\n", pos, *p);
		p++;
		pos++;
	}
}

