struct matrix {
	unsigned long int height;
	unsigned long int width;
	float *rows;
};

typedef struct thread_data{
  int thread_id;
  long unsigned int buffer_begin;
  long unsigned int buffer_end;
  long unsigned int buffer_size;
  long unsigned int stride;
  int mat_size;
  float mat_value;
  float* m;
}THREAD_DATA;

int scalar_matrix_mult(float scalar_value, struct matrix *matrix);

int matrix_matrix_mult(struct matrix *a, struct matrix *b, struct matrix *c);
