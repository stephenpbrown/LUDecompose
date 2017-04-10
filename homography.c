// Steve Brown
// CS330
// Programming Assignment 5
// Due 12/2/15

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "LUdecomp.h"

int main()
{
   int N=0, i=0, j=0;
   scanf("%i",&N); // Scans in the first number which is N
   double A_[2*N][8]; // 2D array for the 2*N x 8 matrix
   double *A[8]; // double * type for copying A_ to
   double b_[2*N]; // Vector for the 2*N x 1 vector
   double a, b;
   double x[N], y[N], xPrime[N], yPrime[N];
  
   // Distribute coordinates
   while(scanf("%lf %lf", &a, &b) == 2)
   {
      if(i < N) // Splits the coordinates between x, y, xPrime, and yPrime
      {
         x[i] = a;
         y[i] = b;
         i++;
      }
      else
      {
         xPrime[j] = a;
         yPrime[j] = b;
         j++;
      }
   } 
   // (7) Constructs the 2*N x 8 matrix 
   int k = 0;
   for(i = 0; i < (2*N); i++)
   { 
      A_[i][0] = (i%2 == 0) ? x[k] : 0; 
      A_[i][1] = (i%2 == 0) ? y[k] : 0; 
      A_[i][2] = (i%2 == 0) ? 1 : 0; 
      A_[i][3] = (i%2 == 0) ? 0 : x[k];
      A_[i][4] = (i%2 == 0) ? 0 : y[k];
      A_[i][5] = (i%2 == 0) ? 0 : 1;
      A_[i][6] = (i%2 == 0) ? x[k]*xPrime[k]*(-1) : x[k]*yPrime[k]*(-1);
      A_[i][7] = (i%2 == 0) ? y[k]*xPrime[k]*(-1) : y[k]*yPrime[k]*(-1); 
      if(i%2 == 1) k++;
   }
   // Constructs the 2*N x 1 vector
   j = 0;
   for(i = 0; i < N; i++)
   {
      b_[j] = xPrime[i];
      b_[j+1] = yPrime[i];
      j += 2;
   }  

// Start ATA
   double ATA[8][8];
   for(i = 0; i < 8; i++)
   {
      for(j = i; j < 8; j++) // (9) Uses (7) to construct the upper triangular portion of ATA
      {
         double sum = 0.0;
         for(int k = 0; k < (2*N); k++)
            sum += A_[k][i]*A_[k][j]; // First sum
         ATA[i][j] = sum;
      }
   }

   for(i = 0; i < 8; i++) // (10)
      for(j = 0; j < i; j++)
         ATA[i][j] = ATA[j][i]; // Copies the upper portion of the matrix

   double ATb[8];
   for(i = 0; i < 8; i++) // (11) computes ATb
   {
      double sum = 0.0;
      for(int j = 0; j < (2*N); j++)
         sum += A_[j][i]*b_[j]; // Second sum
      ATb[i] = sum;
   }
// End ATA
   
   // Copy to type (double *) in order to send to LUdecompose
   for(i = 0; i < 8; i++)
   {
      A[i] = ATA[i];
   }
   LUdecomp *LU = LUdecompose(8, A); // Partial pivoting
   double xStar[9];
   LUsolve(LU, ATb, xStar); // Solves for x*
   xStar[i] = 1.0; // Always assumes 1 for the last value in element
   for(i = 0; i < 9; i++) // Prints out the homography matrix in row-major order
   {
      if(i%3 == 0 && i != 0) printf("\n");
      printf("%.10f ",xStar[i]);
   }
   LUdestroy(LU); // Frees the memory used
   printf("\n");
 
   return 0;
}
