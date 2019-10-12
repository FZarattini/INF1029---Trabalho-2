#include <immintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <errno.h>
#include "matrix_lib.h"

float scalar_value = 0.0f;

struct matrix matrixA, matrixB, matrixC;

int store_matrix(struct matrix *matrix, char *filename) {
  unsigned long int i = 0;
  unsigned long int N = 0;
  FILE *fd = NULL;

  /* Check the numbers of the elements of the matrix */
  N = matrix->height * matrix->width;

  /* Check the integrity of the matrix */
  if (N == 0 || matrix->rows == NULL) return 0;

  /* Try to open file of floats */
  if ((fd = fopen(filename, "wb")) == NULL) {
    printf("Unable to open file %s\n", filename);
    return 0;
  }

  float *nxt_a = matrix->rows; 

  for ( i = 0;
	i < N; 
	i += 8, nxt_a += 8) {

	if (fwrite(nxt_a, sizeof(float), 8, fd) != 8) {
            printf("Error writing to file %s: short write (less than 8 floats)\n", filename);
            return 0;
	}
  }

  if (fd != NULL) fclose(fd);

  return 1;
}

int load_matrix(struct matrix *matrix, char *filename) {
  unsigned long int i = 0;
  unsigned long int N = 0;
  FILE *fd = NULL;

  /* Check the numbers of the elements of the matrix */
  N = matrix->height * matrix->width;

  /* Check the integrity of the matrix */
  if (N == 0 || matrix->rows == NULL) return 0;

  /* Try to open file of floats */
  if ((fd = fopen(filename, "rb")) == NULL) {
    printf("Unable to open file %s\n", filename);
    return 0;
  }

  float *nxt_a = matrix->rows; 

  for ( i = 0;
	i < N; 
	i += 8, nxt_a += 8) {

	if (fread(nxt_a, sizeof(float), 8, fd) != 8) {
            printf("Error reading from file %s: short read (less than 8 floats)\n", filename);
            return 0;
	}
  }

  if (fd != NULL) fclose(fd);

  return 1;
}

/* Thread to initialize arrays */
void *init_arrays(void *threadarg) {
  struct thread_data *my_data;

  my_data = (struct thread_data *) threadarg;

  float *nxt_matrix = my_data.mem + my_data->buffer_begin;

  /* Initialize the two argument arrays */
  __m256 vec_matrix= _mm256_broadcast_ss(my_data->m_value);

  for (long unsigned int i = my_data->buffer_begin;
	i < my_data->buffer_end; 
	i += my_data->stride, nxt_evens += my_data->stride, nxt_odds += my_data->stride) {

	/* Store the elements of the result array */
        _mm256_store_ps(nxt_matrix, vec_matrix);
 }

 pthread_exit(NULL);
}


int print_matrix(struct matrix *matrix) {
  unsigned long int i;
  unsigned long int N;
  unsigned long int nxt_newLine;

  /* Check the numbers of the elements of the matrix */
  N = matrix->height * matrix->width;

  /* Check the integrity of the matrix */
  if (N == 0 || matrix->rows == NULL) return 0;

  /* Initialize new line controol */
  nxt_newLine = matrix->width - 1;

  /* Print matrix elements */
  for (i = 0; i < N; i++) {
     printf("%5.1f ", matrix->rows[i]);
     if (i == nxt_newLine) {
	printf("\n");
	nxt_newLine += matrix->width;
     }
  }

  return 1;
}

int check_errors(struct matrix *matrix, float scalar_value) {
  unsigned long int i;
  unsigned long int N;

  /* Check the numbers of the elements of the matrix */
  N = matrix->height * matrix->width;

  /* Check the integrity of the matrix */
  if (N == 0 || matrix->rows == NULL) return 0;

  /* Check for errors (all values should be equal to scalar_value) */
  float maxError = 0.0f;
  float diffError = 0.0f;
  for (i = 0; i < N; i++)
    maxError = (maxError > (diffError=fabs(matrix->rows[i]-scalar_value)))? maxError : diffError;
  printf("Max error: %f\n", maxError);

  return 1;
}

int main_func(int argc, char *argv[]) {
  unsigned long int DimA_M, DimA_N, DimB_M, DimB_N;
  int NUM_THREADS;
  char *matrixA_filename, *matrixB_filename, *result1_filename, *result2_filename;
  char *eptr = NULL;

  // Disable buffering entirely
  setbuf(stdout, NULL);

  // Check arguments
  if (argc != 10) {
        printf("Usage: %s <scalar_value> <DimA_M> <DimA_N> <DimB_M> <DimB_N> <matrixA_filename> <matrixB_filename> <result1_filename> <result2_filename>\n", argv[0]);
        return 0;
  } else {
        //printf("Number of args: %d\n", argc);
        //for (int i=0; i<argc; ++i)
         //       printf("argv[%d] = %s\n", i, argv[i]);
  }

  // Convert arguments
  scalar_value = strtof(argv[1], NULL);
  DimA_M = strtol(argv[2], &eptr, 10);
  DimA_N = strtol(argv[3], &eptr, 10);
  DimB_M = strtol(argv[4], &eptr, 10);
  DimB_N = strtol(argv[5], &eptr, 10);

  NUM_THREADS = strtol(argv[6], &eptr, 10);

  matrixA_filename = argv[7];
  matrixB_filename = argv[8];
  result1_filename = argv[9];
  result2_filename = argv[10];

  if ((scalar_value == 0.0f) || (DimA_M == 0) || (DimA_N == 0) || (DimB_M == 0) || (DimB_N == 0)) {
        printf("%s: erro na conversao do argumento: errno = %d\n", argv[0], errno);

        /* If a conversion error occurred, display a message and exit */
        if (errno == EINVAL)
        {
            printf("Conversion error occurred: %d\n", errno);
            return 1;
        }

        /* If the value provided was out of range, display a warning message */
        if (errno == ERANGE) {
            printf("The value provided was out of rangei: %d\n", errno);
            return 1;
	}
  }

 if(NUM_THREADS == 0)
 {
	NUM_THREADS = 1;
 }

  /* Allocate the arrays of the four matrixes */
  a=  (float*)aligned_alloc(32, DimA_M*DimA_N*sizeof(float));
  scalar = (float*)aligned_alloc(32, DimA_M*DimA_N*sizeof(float));
  b = (float*)aligned_alloc(32, DimB_M*DimB_N*sizeof(float));
  c = (float*)aligned_alloc(32, DimA_M*DimB_N*sizeof(float));
  

  if ((a == NULL) || (b == NULL) || (c == NULL)) {
	printf("%s: array allocation problem.", argv[0]);
	return 1;
  }

  /* Initialize the three matrixes */
  matrixA.height = DimA_M;
  matrixA.width = DimA_N;
  matrixA.rows = a;
  //if (!initialize_matrix(&matrixA, 5.0f, 0.0f)) {
  if (!load_matrix(&matrixA, matrixA_filename)) {
	printf("%s: matrixA initialization problem.", argv[0]);
	return 1;
  }

  /* Print matrix */
  printf("---------- Matrix A ----------\n");
  print_matrix(&matrixA);

//================================ INIT THREAD MATRIX A =========================================//

/* Define auxiliary variables to work with threads */
  struct thread_data thread_data_array[NUM_THREADS];
  pthread_t thread[NUM_THREADS];
  pthread_attr_t attr;
  int rc;
  long t;
  void *status;
  long unsigned int buffer_chunk = (DimA_M * DimA_N) / NUM_THREADS;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  /* Create threads to initialize arrays */
  for(t=0; t<NUM_THREADS; t++){
	thread_data_array[t].thread_id = t;
	thread_data_array[t].buffer_begin = t * buffer_chunk;
	thread_data_array[t].buffer_end = t * buffer_chunk + buffer_chunk;
	thread_data_array[t].buffer_size = (DimA_M * DimA_N);
	thread_data_array[t].stride = VECTOR_SIZE;
	thread_data_array[t].m_value = matrixA->rows[0];

	if (rc = pthread_create(&thread[t], &attr, init_arrays, (void *) &thread_data_array[t])) {
	  printf("ERROR; return code from pthread_create() is %d\n", rc);
	  exit(-1);
	}
  }

  /* Free attribute and wait for the other threads */
  pthread_attr_destroy(&attr);
  for(t=0; t<NUM_THREADS; t++) {
	if (rc = pthread_join(thread[t], &status)) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
	}
  }

//====================================================================================

//============================= INIT THREAD SCALAR ===================================
 /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  /* Create threads to calculate product of arrays */
  for(t=0; t<NUM_THREADS; t++){
	thread_data_array[t].thread_id = t;
	thread_data_array[t].buffer_begin = t * buffer_chunk;
	thread_data_array[t].buffer_end = t * buffer_chunk + buffer_chunk;
	thread_data_array[t].buffer_size = (DimA_M * DimA_N);
	thread_data_array[t].stride = VECTOR_SIZE;
	thread_data_array[t].m_value = scalar_value;

	if (rc = pthread_create(&thread[t], &attr, init_arrays, (void *) &thread_data_array[t])) {
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
       }
  }


  /* Free attribute and wait for the other threads */
  pthread_attr_destroy(&attr);
  for(t=0; t<NUM_THREADS; t++) {
	if (rc = pthread_join(thread[t], &status)) {
		printf("ERROR; return code from pthread_join() is %d\n", rc);
		exit(-1);
	}
  }

//====================================================================================
//============================ MULT POR SCALAR THREAD  ===============================
  /* Scalar product of matrix A */
  printf("Executing mult_arrays(%5.1f, matrixA)...\n",scalar_value);


  if (!scalar_matrix_mult(scalar_value, &matrixA)) {
	printf("%s: scalar_matrix_mult problem.", argv[0]);
	return 1;
  }

  /* Print matrix */
  printf("---------- Matrix A ----------\n");
  print_matrix(&matrixA);

  /* Write first result */
  if (!store_matrix(&matrixA, result1_filename)) {
	printf("%s: failed to write first result to file.", argv[0]);
	return 1;
  }

  /* Check for errors */
  //check_errors(&matrixA, 10.0f);



















  matrixB.height = DimB_M;
  matrixB.width = DimB_N;
  matrixB.rows = b;
  //if (!initialize_matrix(&matrixB, 1.0f, 0.0f)) {
  if (!load_matrix(&matrixB, matrixB_filename)) {
	printf("%s: matrixB initialization problem.", argv[0]);
	return 1;
  }

  /* Print matrix */
  printf("---------- Matrix B ----------\n");
  print_matrix(&matrixB);

  matrixC.height = DimA_M;
  matrixC.width = DimB_N;
  matrixC.rows = c;
//  if (!initialize_matrix(&matrixC, 0.0f, 0.0f)) {
//	printf("%s: matrixC initialization problem.", argv[0]);
//	return 1;
//  }

  /* Print matrix */
  printf("---------- Matrix C ----------\n");
  print_matrix(&matrixC);

/*
  // Calculate the product between matrix A and matrix B//
  printf("Executing matrix_matrix_mult(matrixA, mattrixB, matrixC)...\n");
  if (!matrix_matrix_mult(&matrixA, &matrixB, &matrixC)) {
	printf("%s: matrix_matrix_mult problem.", argv[0]);
	return 1;
  }

  // Print matrix //
  printf("---------- Matrix C ----------\n");
  print_matrix(&matrixC);

  // Write second result //
  if (!store_matrix(&matrixC, result2_filename)) {
	printf("%s: failed to write second result to file.", argv[0]);
	return 1;
  }*/

  /* Check foor errors */
  //check_errors(&matrixC, 160.0f);

  return 0;
}
