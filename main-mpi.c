#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<stdbool.h>
#include<mpi.h>

void stencil(float *input_vec, int m, int n, float *filter_vec, int k, float *output_vec, int b);
int *read_dims(char *filename);
float *read_array(char *filename, int *dims, int num_dims);
void *write_to_output_file(char *filename, float *output, int *dims, int num_dims);
long int product(int *array, int n);


int main(int argc, char *argv[]){

    /* Read Input File */
    int * inputDim = read_dims(argv[1]);
    int arr[3]     = {inputDim[1], inputDim[2], inputDim[3]};
    int * ptrArr   = arr;
    float * input  = read_array(argv[1], ptrArr, inputDim[0]);

    /* Read Filter File */
    int * filterDim = read_dims(argv[2]);
    int arra[2]     = {filterDim[1],filterDim[2]};
    int * ptrArra   = arra;
    float * filter  = read_array(argv[2], ptrArra, filterDim[0]);

    /* MPI number of matrices to divide the work to */
    int my_rank, comm_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // get the rank of the current processor
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size); // get the total number of processors

    /* Specify how many matrices will each process handle */
    int part_size = inputDim[1]/comm_size;
    int reminder = inputDim[1] - comm_size*part_size;
    int array_size[comm_size];
    int start_point[comm_size];
    int a,b;

    /* Assign some number of matrices to each of the processes */
    for(b = 0; b < comm_size; b++){
      if(b < reminder){
        array_size[b] = (part_size + 1)*inputDim[2]*inputDim[3];
      }
      else{
        array_size[b] = part_size*inputDim[2]*inputDim[3];
      }
    }

    /* Set up the correct start points in input for each of the processes */
    start_point[0] = 0;
    for(a=1; a < comm_size; a++){
       start_point[a] = start_point[a-1] + array_size[a-1];
    }

    /* input and output for each of the processes */
    float *partial_input = (float*) malloc(inputDim[1]*inputDim[2]*inputDim[3] * sizeof(float));
    float *partial_output = (float*) malloc(inputDim[1]*inputDim[2]*inputDim[3] * sizeof(float));

    /* Assign the portion of input to each of the processes */
    MPI_Scatterv(input, array_size, start_point, MPI_FLOAT, partial_input, array_size[my_rank], MPI_FLOAT, 0, MPI_COMM_WORLD);

    /* Apply the Stencil if a process got assigned any part of the input */
    if(array_size[my_rank] > 0){
	     stencil(partial_input, inputDim[2], inputDim[3], filter, filterDim[1], partial_output, array_size[my_rank]/(inputDim[2]*inputDim[3]));
    }

    /* Create output and gather all the partial_outputs to create a final solution */
    float *output = malloc(inputDim[1]*inputDim[2]*inputDim[3]*sizeof(float));
    MPI_Gatherv(partial_output, array_size[my_rank], MPI_FLOAT, output, array_size, start_point, MPI_FLOAT, 0, MPI_COMM_WORLD);

    /* Write to output, free the memory, finish MPI */
    if(my_rank == 0){
      write_to_output_file(argv[3], output, ptrArr, inputDim[0]);
    }

    free(partial_input);
    free(partial_output);
    free(output);
    MPI_Finalize();

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
