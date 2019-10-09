#include <immintrin.h>
#include <pthread.h>

#define VECTOR_SIZE 8

struct matrix {
	unsigned long int height;
	unsigned long int width;
	float *rows;
};

int scalar_matrix_mult(float scalar_value, struct matrix *matrix) {
  unsigned long int i;
  unsigned long int N;

  /* Check the numbers of the elements of the matrix */
  N = matrix->height * matrix->width;

  /* Check the integrity of the matrix */
  if (N == 0 || matrix->rows == NULL) return 0;

  /* Initialize the scalar vector with the scalar value */
  __m256 vec_scalar_value = _mm256_set1_ps(scalar_value);

  /* Compute the product between the scalar value and the elements of the matrix */
  float *nxt_matrix_a = matrix->rows; 
  float *nxt_result = matrix->rows; 

  for ( i = 0; 
	i < N; 
	i += 8, nxt_matrix_a += 8, nxt_result += 8) {
	  /* Initialize the three argument vectors */
	  __m256 vec_matrix_a = _mm256_load_ps(nxt_matrix_a);

	  /* Compute the expression res = a * b + c between the three vectors */
	  __m256 vec_result = _mm256_mul_ps(vec_scalar_value, vec_matrix_a);

	  /* Store the elements of the result vector */
	  _mm256_store_ps(nxt_result, vec_result);
  }

  return 1;
}

int matrix_matrix_mult(struct matrix *a, struct matrix *b, struct matrix *c) {
  unsigned long int NA, NB, NC, i, j, k;
  float* nxt_a; 
  float* nxt_b; 
  float* nxt_c; 

  /* Check the numbers of the elements of the matrix */
  NA = a->height * a->width;
  NB = b->height * b->width;
  NC = c->height * c->width;

  /* Check the integrity of the matrix */
  if ( (NA == 0 || a->rows == NULL) ||
       (NB == 0 || b->rows == NULL) ||
       (NC == 0 || c->rows == NULL) ) return 0;

  /* Check if we can execute de product of matrix A and matrib B */
  if ( (a->width != b->height) ||
       (c->height != a->height) ||
       (c->width != b->width) ) return 0;

  for ( i = 0, nxt_a = a->rows; 
	i < a->height; 
	i += 1) {

	  /* Set nxt_b to the begining of matrixB */
	  nxt_b = b->rows;

      	  for ( j = 0; 
	    j < a->width; 
	    j += 1, nxt_a += 1) {
		/* Initialize the scalar vector with the next scalar value */
	  	__m256 vec_a = _mm256_set1_ps(*nxt_a);

		/* 
		 * Compute the product between the scalar vector and the elements of 
		 * a row of matrixB, 8 elements at a time, and add the result to the 
		 * respective elements of a row of matrixC, 8 elements at a time.
		 */
          	for (k = 0, nxt_c = c->rows + (c->width * i);
	    	     k < b->width;
	   	     k += VECTOR_SIZE, nxt_b += VECTOR_SIZE, nxt_c += VECTOR_SIZE) {

			/* Load part of b row (size of vector) */
	  		__m256 vec_b = _mm256_load_ps(nxt_b);

          		/* Initialize vector c with zero or load part of c row (size of vector) */
			__m256 vec_c;

			if (j == 0) { /* if vec_a is the first scalar vector, vec_c is set to zero */
	  			vec_c = _mm256_setzero_ps();
			} else { /* otherwise, load part of c row (size of vector) to vec_c */
	  			vec_c = _mm256_load_ps(nxt_c);
			}

	  		/* Compute the expression res = a * b + c between the three vectors */
	  		vec_c = _mm256_fmadd_ps(vec_a, vec_b, vec_c);

	  		/* Store the elements of the result vector */
	  		_mm256_store_ps(nxt_c, vec_c);
	  	}
      	  }
  }

  return 1;
}
