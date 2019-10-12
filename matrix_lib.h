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
	float m_value;
};

float *a;
float *b;
float *c;
float *result;
float *scalar;

float scalar__value;
int NUM__THREADS;

#define VECTOR_SIZE 8

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

int matrix_matrix_mult(struct matrix *a, struct matrix *b, struct matrix *c);

void set_number_threads(int num_threads);
