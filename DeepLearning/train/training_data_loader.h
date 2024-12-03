#pragma once
#include <stdio.h>
#include <stddef.h>

void load_training_data(FILE *file, double** *inputs_p, double** *outputs_p, size_t data_number, size_t input_size);
