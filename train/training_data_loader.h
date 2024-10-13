float* image_to_input(char* path);
float get_inputs_size();
float get_outputs_size();
void load_training_data(FILE *file, float** *inputs_p, float** *outputs_p, size_t data_number, size_t input_size);
