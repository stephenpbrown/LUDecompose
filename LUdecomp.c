// Steve Brown
// CS330
// Programming Assignment #4
// Due 11/11/15

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "LUdecomp.h"

// Creates the matrix
double **createMatrix(int N) {
  double **M = (double **) malloc(N*sizeof(double*));
  for (int i = 0; i < N; i++)
    M[i] = (double*) malloc(N*sizeof(double));
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      M[i][j] = (i == j) ? 1.0 : 0.0;
  return M;
}

//Deallocates the data allocated in LUdecompose() and frees the matrix
void LUdestroy(LUdecomp *decomp)
{
   int N = decomp->N;
   for(int i = 0; i < N; i++)
      free(decomp->LU[i]); // Loops through and frees the elements of the array
   free(decomp->LU);
   free(decomp);
   return;
}

// Decomposes the matrix and returns an LUdecomp object
LUdecomp *LUdecompose(int N, double **A) {
   LUdecomp* LU = (LUdecomp*) malloc(sizeof(LUdecomp));
   LU->N = N;
   LU->LU = createMatrix(N);
   LU->mutate = (short *) malloc(N*sizeof(short));
   LU->d = +1;

   // Initialize row permutation
   for (int i = 0; i < N; i++)
      LU->mutate[i] = (short) i;


   // Clone A into LU
   double **A_ = LU->LU;
   for (int i = 0; i < N; i++)
      for (int j = 0; j < N; j++)
         A_[i][j] = A[i][j];

   for (int j = 0; j < N; j++) // Main loop for replacing A with LU
   {
      for (int i = 0; i <= j; i++) // Loop for computing on and above the diagonal
      { 
   	 double sum = 0.0;
         for (int k = 0; k < i; k++) // Loop for the first sum
	 {
            sum += (A_[i][k]*A_[k][j]); // Sum
	 }
         A_[i][j] -= sum;
      }

      double p = fabs(A_[j][j]);
      int n = j;

      for (int i = (j + 1); i < N; i++) // Loop for computing below the diagonal
      {
	 double sum = 0.0;
	 for(int k = 0; k < j; k++) // Loop for the second sum
	 {
	    sum += (A_[i][k]*A_[k][j]); // Sum
	 }
	 A_[i][j] -= sum;

         if (fabs(A_[i][j]) > p) // If better pivot found
         {
            p = fabs(A_[i][j]); // Then record new pivot
	    n = i;
         }
      }
      // Abort!
      if (p == 0)
      {
         LUdestroy(LU);
         return NULL;
      } 
      else if (n != j) // If best pivot off diagonal
      {  
         // Swap rows n and j of A_
       	 double *temp;
	 temp = A_[n];
	 A_[n] = A_[j];
	 A_[j] = temp;

         // Swap mutate[n] and mutate[j]
	 short tempMutate = LU->mutate[n];
	 LU->mutate[n] = LU->mutate[j];
	 LU->mutate[j] = tempMutate;
 	 LU->d *= -1; // Flip parity
      }
      for (int i = (j+1); i < N; i++) A_[i][j] /= A_[j][j];   
   }

/*
   // DEBUG for printing the decomposed matrix
   for(int i = 0; i < N; i++)
   {
      printf("mutate[%i] = [%i]\n",i,LU->mutate[i]);
      for(int k = 0; k < N; k++)
      {
         printf("array[%i][%i] = %f\n",i,k,A_[i][k]);
      }
   }
*/
  return LU;
}


// Solves the system for Ax = b for x
void LUsolve(LUdecomp *decomp, double *b, double *x)
{
   int N = decomp->N; // Make N equal to the dimension of the array
   double *y = (double *) malloc(N*sizeof(double)); // Allocate memory for y 

   y[0] = b[decomp->mutate[0]]; 

   for(int i = 1; i < N; i++) // Loop for forward substitution
   {
      double sum = 0.0; 
      for(int j = 0; j < i; j++) // Loop for the first sum
         sum += (decomp->LU[i][j]*y[j]);
      y[i] = b[decomp->mutate[i]] - sum;
   }

   x[N-1] = (y[N-1])/(decomp->LU[N-1][N-1]);
   
   for(int i = (N-2); i >= 0; i--) // Loop for doing back substitution
   {
      double sum = 0.0;
      for(int j = (i+1); j < N; j++) // Loop for second sum
         sum += (decomp->LU[i][j]*x[j]);
      x[i] = (y[i] - sum)/(decomp->LU[i][i]);
   }
   free(y); // Free the allocated memory for y
}


