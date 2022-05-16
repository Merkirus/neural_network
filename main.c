#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define WIDTH 51
#define HEIGHT 51
#define SCALE 25
#define SAMPLE_SIZE 10

#define randnum(min, max) \
	((rand() % (int) (((max) + 1) - (min))) + (min))

typedef float Layer[WIDTH][HEIGHT];

int adjust_points(int x, int low, int high)
{
	if (x < low) x = low;
	if (x > high) x = high;
	return x;
}

void fill_rect(Layer layer, int x, int y, int w, int h, float value)
{
	assert(w > 0);
	assert(h > 0);
	int x0 = adjust_points(x, 0, HEIGHT-1);
	int y0 = adjust_points(y, 0, WIDTH-1);
	int x1 = adjust_points(x0 + h - 1, 0, HEIGHT-1);
	int y1 = adjust_points(y0 + w - 1, 0, WIDTH-1);

	int i, j;
	for (i = x0; i <= x1; ++i) {
		for (j = y0; j <= y1; ++j) {
			layer[i][j] = value;
		}
	}
}

void fill_circle(Layer layer, int x, int y, int r, int value)
{
	assert(r > 0);
	int x0 = adjust_points(x - r, 0, HEIGHT-1);
	int y0 = adjust_points(y - r, 0, WIDTH-1);
	int x1 = adjust_points(x + r, 0, HEIGHT-1);
	int y1 = adjust_points(y + r, 0, WIDTH-1);

	int i, j;
	for (i = x0; i <= x1; ++i) {
		for (j = y0; j <= y1; ++j) {
			int dx = x - i;
			int dy = y - j;
			if (dx*dx + dy*dy <= r*r)
				layer[i][j] = value;
		}
	}
}

void layer_to_ppm(Layer layer, const char *file_path)
{
	FILE *f = fopen(file_path, "wb");
	if (f == NULL) {
		fprintf(stderr, "ERROR: could not open the file %s: %m\n", file_path);
		exit(1);
	}

	fprintf(f, "P6\n%d %d 255\n", WIDTH*SCALE, HEIGHT*SCALE);

	int i, j;
	for (i = 0; i < HEIGHT*SCALE; ++i) {
		for (j = 0; j < WIDTH*SCALE; ++j) {
			float s = layer[i/SCALE][j/SCALE];
			char pixel[3] = {
				(char) floorf(255*s),
				0,
				0
			};

			fwrite(pixel, sizeof(pixel), 1, f);
		}
	}

	fclose(f);
}

void save_layer(Layer layer, const char *file_path)
{
	FILE *f = fopen(file_path, "wb");
	if (f == NULL) {
		fprintf(stderr, "ERROR: could not open the file%s\n", file_path);
		exit(1);
	}

	fwrite(layer, sizeof(Layer), 1, f);

	fclose(f);
}

void load_layer(Layer layer, const char *file_path)
{
	assert(0 && "TODO");	
}

float forward(Layer input,Layer weight)
{

	float output = 0;

	int i, j;
	for (i = 0; i < HEIGHT; ++i) {
		for (j = 0; j < WIDTH; ++j) {
			output += input[i][j] * weight[i][j];
		}
	}

	return output;
}

void func(Layer input, Layer weight)
{
	
}

void random_rect(Layer layer)
{
	fill_rect(layer, 0, 0, WIDTH, HEIGHT, 0);
	int x = randnum(0, HEIGHT-1);
	int y = randnum(0, WIDTH-1);
	int w = randnum(1, WIDTH);
	if (w+y > WIDTH)
		w = WIDTH-y;
	int h = randnum(1, HEIGHT);
	if (h+x > HEIGHT)
		h = HEIGHT-x;
	fill_rect(layer, x, y, w, h, 1);
}

void random_circle(Layer layer)
{
	fill_rect(layer, 0, 0, WIDTH, HEIGHT, 0);
	int x = randnum(1,HEIGHT-1);
	int y = randnum(1, WIDTH-1);
	int r = randnum(1, WIDTH/2);
	int shorter_x = x < HEIGHT/2 ? x : HEIGHT - x;
	int shorter_y = y < WIDTH/2 ? y : HEIGHT - y;
	if (r > shorter_x || r > shorter_y)
		r = shorter_x < shorter_y ? shorter_x : shorter_y;
	fill_circle(layer, x, y, r, 1);
}

Layer input;
Layer weight;

#define PREFIX "rect"

int main(int argc, char const *argv[])
{
	srand(time(NULL));

	char file_path[256];

	int i;
	for (i = 0; i < SAMPLE_SIZE; ++i) {
		printf("[INFO] generating "PREFIX" %d\n", i);


		random_rect(input);


		snprintf(file_path, sizeof(file_path), PREFIX"-%02d.bin", i);
		save_layer(input, file_path);
		snprintf(file_path, sizeof(file_path), PREFIX"-%02d.ppm", i);
		layer_to_ppm(input, file_path);
	}

	return 0;
}