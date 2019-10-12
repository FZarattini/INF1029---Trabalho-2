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
	long unsigned int m_value;
};

float *a = NULL;
float *b = NULL;
float *c = NULL;
float *result = NULL;
float *scalar = NULL;

#define VECTOR_SIZE 8

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

int matrix_matrix_mult(struct matrix *a, struct matrix *b, struct matrix *c);
