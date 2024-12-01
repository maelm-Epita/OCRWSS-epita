
// converts the image at the path to a set of input floats
// each float corresponds to a pixel's greyscale value
float *image_to_input(char *path);

// test the dataset
void input_to_image(float* input);

// converts the network's prediction, which is an array of 26 floats representing the guess of the network
// to a character
// to do so, it simply takes the index of the max of the network
char output_to_prediction(float* output);
