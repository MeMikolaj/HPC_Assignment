#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void stencil(float *input_vec, int m, int n, float *filter_vec, int k, float *output_vec, int b) {
  float (*input)[m][n] = (float(*)[m][n]) input_vec;  /* This stores many inputs. This is 3D array with inputs*/
  float (*filter)[k]   = (float(*)[k]) filter_vec;    /* This stores one filter, 2D*/
  float (*output)[m][n]= (float(*)[m][n]) output_vec; /* This stores many outputs, arrays*/

  int x,y,z,v,u;

  int centre = (int) ceil(k/2.0) - 1; /* initial centre start point of a filter*/

  int bLowerRow = (int) floor((k-1)/2.0);
  int bUpperRow = (int) ceil((k-1)/2.0);
  int upperDim = m - bUpperRow;
  int upperDin = n - bUpperRow;

  /* private variable for each for that is value*/
  float value = 0;

  for(x = 0; x < b; x++){
    #pragma omp parallel for default(none) shared(input, output, filter, centre, bLowerRow, upperDim, upperDin, m, n, b, k, x) private(y,z,v,u) private(value) schedule(static)
    for(y = 0; y < m; y++){
      for(z = 0; z < n; z++){
        if((y < bLowerRow) || (y >= upperDim) || (z < bLowerRow) || (z >= upperDin)){
          output[x][y][z] = input[x][y][z];
        }
        else{
          value = 0;
          for(v = 0; v < k; v++){
            for(u = 0; u < k; u++){
              value += input[x][y + v - centre][z + u - centre] * filter[v][u];
            }
  	      }
          output[x][y][z] = value;
	      }
      }
    }
  }
}
