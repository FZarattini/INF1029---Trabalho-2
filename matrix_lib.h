struct matrix {
	unsigned long int height;
	unsigned long int width;
	float *rows;
};

struct thread_data {
	int thread_id;
	long unsigned int buffer_begin;
	long unsigned int buffer_end;
	long unsigned int buffer_size;
	long unsigned int stride;
	struct matrix *matrixA;
	struct matrix *matrixB;
	struct matrix *matrixC;
	float scalar;

};

float *mem_a;
float *mem_b;
float *mem_c;
float *mem_result;
float *mem_scalar;

int NUM__THREADS;

#define VECTOR_SIZE 8

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

int matrix_matrix_mult(struct matrix *a, struct matrix *b, struct matrix *c);

void set_number_threads(int num_threads);
