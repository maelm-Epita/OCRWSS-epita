
// converts the image at the path to a set of input doubles
// each double corresponds to a pixel's greyscale value
double *image_to_input(char *path);

// test the dataset
void input_to_image(double* input);

// converts the network's prediction, which is an array of 26 doubles representing the guess of the network
// to a character
// to do so, it simply takes the index of the max of the network
char output_to_prediction(double* output);
