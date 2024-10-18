#pragma once
static const float EPS = 1e-8;

float Cost(struct Network net, struct training_data data);
float av_Cost(float* costs, size_t size);
