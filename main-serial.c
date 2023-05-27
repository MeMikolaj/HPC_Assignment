#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>

void stencil(float *input_vec, int m, int n, float *filter_vec, int k, float *output_vec, int b);
int *read_dims(char *filename);
float *read_array(char *filename, int *dims, int num_dims);
void *write_to_output_file(char *filename, float *output, int *dims, int num_dims);
long int product(int *array, int n);


int main(int argc, char *argv[]){

    /* Read Input File */
    int * inputDim = read_dims(argv[1]);
    int arr[3] = {inputDim[1], inputDim[2], inputDim[3]};
    int * ptrArr = arr;
    float * input = read_array(argv[1], ptrArr, inputDim[0]);

    /* Read Filter File */
    int * filterDim = read_dims(argv[2]);
    int arra[2] = {filterDim[1],filterDim[2]};
    int * ptrArra = arra;
    float * filter = read_array(argv[2], ptrArra, filterDim[0]);

    /* Use the Stencil and write to the Output File */
    int dimensions = inputDim[1]*inputDim[2]*inputDim[3];
    float *output = malloc(dimensions*sizeof(float));
    stencil(input, inputDim[2], inputDim[3], filter, filterDim[1], output, inputDim[1]);
    write_to_output_file(argv[3], output, ptrArr, inputDim[0]);
    free(output);

}

/*Code for reading and writing to the files*/

/*Gets the dimensions of the matrices. This will return a 1d array of 4 elements [0] = number of dimensions [1] = batch [2] = m [3] = n*/
int *read_dims(char *filename) {
    FILE *file = fopen(filename,"r");
    int i;

    if(file == NULL) {
        printf("Unable to open file: %s", filename);
        return NULL;
    }

    char firstline[500];
    fgets(firstline, 500, file);

    int line_length = strlen(firstline);

    int num_dims = 0;

    for(i=0; i<line_length; i++) {
        if(firstline[i] == ' ') {
            num_dims++;
        }
    }

    int *dims = malloc((num_dims+1)*sizeof(int));
    dims[0] = num_dims;
    const char s[2] = " ";
    char *token;
    token = strtok(firstline, s);
    i = 0;
    while( token != NULL ) {
        dims[i+1] = atoi(token);
        i++;
        token = strtok(NULL, s);
    }
    fclose(file);
    return dims;
}

/*Gets the data from the file and returns it as a 1 dimensional array*/
float * read_array(char *filename, int *dims, int num_dims) {
    FILE *file = fopen(filename,"r");
    int i;

    if(file == NULL) {
        printf("Unable to open file: %s", filename);
        return NULL;
    }

    char firstline[500];
    fgets(firstline, 500, file);

    //Ignore first line and move on since first line contains
    //header information and we already have that.

    long int total_elements = product(dims, num_dims);

    float *one_d = malloc(sizeof(float) * total_elements);

    for(i=0; i<total_elements; i++) {
        fscanf(file, "%f", &one_d[i]);
    }
    fclose(file);
    return one_d;
}

/*Writes to the output file*/
void *write_to_output_file(char *filename, float *output, int *dims, int num_dims){
    FILE *file = fopen(filename,"w");
    int i;

    if(file == NULL) {
        printf("Unable to open file: %s", filename);
        return NULL;
    }
    printf("File opened, writing dims");
    if (file != NULL) {
        for(i=0; i<num_dims; i++) {
            fprintf(file, "%d ", dims[i]);
        }
        fprintf(file, "\n");
    }

    long int total_elements = product(dims, num_dims);

    printf("Writing output data");
    for(i=0; i<total_elements; i++) {
        fprintf(file, "%.7f ", output[i]);
    }
}

/*Returns the number of elements by multiplying the dimensions*/
long int product(int *array, int n) {
    long int product = 1;
    int i;
    for(i=0; i<n; i++) {
        product *= array[i];
    }
    return product;
}
