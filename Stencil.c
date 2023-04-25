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
  /* Not sure if should be -1 or not, double check*/
  int bLowerRow = (int) floor((k-1)/2.0);
  int bUpperRow = (int) ceil((k-1)/2.0);

  /* private variable for each for that is value*/
  float value = 0;

  #pragma omp parallel for default(none) shared(input, output, filter, centre, bLowerRow, bUpperRow, m, n) private(x,y,z,v,u) private(value) schedule(static)
  for(x = 0; x < b; x++){
    /* y = row of an input, z = column of an input*/
    for(y = bLowerRow; y < (m - bUpperRow); y++){
      for(z = bLowerRow; z < (n - bUpperRow); z++){
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

  /* Coppying all the values that weren't changed*/
  for(x = 0; x < b; x++){
    for(z = 0; z < n; z++){
      for(y = 0; y < bLowerRow; y++){
        output[x][y][z] = input[x][y][z];
      }
      for(y = (m - bUpperRow); y < m; y++){
        output[x][y][z] = input[x][y][z];
      }
    }
    for(y = 0; y < m; y++){
      for(z = 0; z < bLowerRow; z++){
        output[x][y][z] = input[x][y][z];
      }
      for(z = (n - bUpperRow); z < n; z++){
        output[x][y][z] = input[x][y][z];
      }
    }
  }

  /* 2nd way to solve it: First sove the square, then the rest*/
}
