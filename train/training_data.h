struct training_data{
  float* inputs;
  float* expected_output;
};

struct training_set{
  size_t input_number;
  size_t data_number;
  struct training_data* data;
};

struct training_set create_training_set(float** inputs, float** outputs, size_t data_number, size_t input_number);
void free_training_set(struct training_set set);
void print_training_set(struct training_set set);
