#pragma once
#include <stdio.h>
#include <stddef.h>

void load_training_data(FILE *file, float** *inputs_p, float** *outputs_p, size_t data_number, size_t input_size);
