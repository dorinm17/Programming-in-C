//	Copyright Manea Dorin-Mihai 313CAb 2021-2022

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "commands.h"

//	frees the dynamically allocated memory for the current image
void end(image_t *img)
{
	if (!img->set) {
		NO_LOAD; return;
	}

	//	grayscale
	if (img->magic == 2 || img->magic == 5) {
		for (int i = 0; i < img->lin; i++)
			free(img->px_gray[i]);
		free(img->px_gray);
		return;
	}

	//	RGB
	for (int i = 0; i < img->lin; i++) {
		for (int j = 0; j < img->col; j++)
			free(img->px_rgb[i][j]);
		free(img->px_rgb[i]);
	}
	free(img->px_rgb);
}

//	dynamically allocates a grayscale pixel array fitted for the
// current working zone
double **alloc_gray(image_t *img)
{
	double **a;
	a = (double **)malloc((img->y2 - img->y1) * sizeof(double *));
	if (!a) {
		free(a); return NULL;
	}
	for (int i = 0; i < img->y2 - img->y1; i++) {
		a[i] = (double *)malloc((img->x2 - img->x1) * sizeof(double));
		if (!a[i]) {
			for (int j = 0; j < i; j++)
				free(a[j]);
			free(a);
			return NULL;
		}
	}
	return a;
}

//	dynamically allocates a RGB pixel array fitted for the
// current working zone
double ***alloc_rgb(image_t *img)
{
	double ***a;
	a = (double ***)malloc((img->y2 - img->y1) * sizeof(double **));
	if (!a) {
		free(a); return NULL;
	}
	for (int i = 0; i < img->y2 - img->y1; i++) {
		a[i] = (double **)malloc((img->x2 - img->x1) * sizeof(double *));
		if (!a[i]) {
			for (int j = 0; j < i; j++)
				free(a[j]);
			free(a);
			return NULL;
		}
	}
	for (int i = 0; i < img->y2 - img->y1; i++)
		for (int j = 0; j < img->x2 - img->x1; j++) {
			a[i][j] = (double *)malloc(RGB * sizeof(double));
			if (!a[i][j]) {
				for (int k = 0; k < j; k++)
					free(a[i][k]);
				for (int k = 0; k < img->lin; k++)
					free(a[k]);
				free(a);
				return NULL;
			}
		}
	return a;
}

double **load_gray(image_t *img, FILE *in)
{
	double **a = alloc_gray(img);
	if (!a) {
		fclose(in); exit(0);
	}

	if (img->magic == 2)	//	plain
		for (int i = 0; i < img->lin; i++)
			for (int j = 0; j < img->col; j++) {
				double x;
				fscanf(in, "%lf", &x);
				a[i][j] = x;
			}
	else	//	binary
		for (int i = 0; i < img->lin; i++)
			for (int j = 0; j < img->col; j++) {
				unsigned char c;	//	!!!
				fread(&c, sizeof(unsigned char), 1, in);
				a[i][j] = (double)c;
			}
	return a;
}

double ***load_rgb(image_t *img, FILE *in)
{
	double ***a = alloc_rgb(img);
	if (!a) {
		fclose(in); exit(0);
	}

	if (img->magic == 3)	//	plain
		for (int i = 0; i < img->lin; i++)
			for (int j = 0; j < img->col; j++)
				for (int k = 0; k < RGB; k++) {
					double x;
					fscanf(in, "%lf", &x);
					a[i][j][k] = x;
				}
	else	//	binary
		for (int i = 0; i < img->lin; i++)
			for (int j = 0; j < img->col; j++)
				for (int k = 0; k < RGB; k++) {
					unsigned char c;	//	!!!
					fread(&c, sizeof(unsigned char), 1, in);
					a[i][j][k] = (double)c;
				}
	return a;
}

//	checks for comments in the image
//	a line that contains a comment does not contain relevant data
void check_comment(FILE *in)
{
	//	all comments start with "#"
	if (fgetc(in) == '#') {
		char c = fgetc(in);
		while (c != '\n')
			c = fgetc(in);
		//	checks if the next line is a comment
		check_comment(in);
	} else {	//	not a comment
		//	if not a comment, moves the cursor one byte behind
		// (at the line's starting point)
		fseek(in, -1L, SEEK_CUR);
	}
}

void load(image_t *img)
{
	//	if there is another loaded image in the program,
	// its dynamically allocated memory needs to be freed
	if (img->set)
		end(img);

	img->x1 = 0; img->y1 = 0;
	img->set = 0;

	char *p = strtok(NULL, "\n ");
	FILE *in = fopen(p, "r");
	//	non-existent file; there is no loaded image in the program
	if (!in) {
		FAIL(p);
		return;
	}

	//	reads magic-word
	char magic_word[MAGIC];
	fgets(magic_word, MAGIC, in);
	//	converts digit in char to int
	img->magic = (int)(magic_word[1] - '0');

	//	reads width, height, maximum pixel intensity
	//	checks for comments in between each line with relevant data
	int width, height, intensity;
	check_comment(in);
	fscanf(in, "%d%d ", &width, &height);
	check_comment(in);
	fscanf(in, "%d ", &intensity);
	check_comment(in);
	img->col = width; img->lin = height;
	img->x2 = width; img->y2 = height;
	img->px_max = intensity;

	if (img->magic == 2 || img->magic == 5)	//	grayscale
		img->px_gray = load_gray(img, in);
	else	//	RGB
		img->px_rgb = load_rgb(img, in);

	//	the image was successfully loaded
	img->set = 1;
	LOAD(p);
	fclose(in);
}

//	checks if the given coordinate is valid —
// integer that lies within the given domain
int check_coord(char *p)
{
	int size = strlen(p);
	for (int i = 0; i < size; i++)
		if ((p[i] >= '0' && p[i] <= '9') || (p[0] == '-' && i == 0)) {
			continue;
		} else {
			INVALID;
			return 0;
		}
	return 1;
}

void zone(image_t *img)
{
	if (!img->set) {
		NO_LOAD; return;
	}

	int x1, x2, y1, y2;
	char *p = strtok(NULL, "\n ");
	//	if argument exists
	if (p) {
		if (strcmp(p, "ALL") == 0) {
			img->x1 = 0; img->x2 = img->col;
			img->y1 = 0; img->y2 = img->lin;
			SELECT_A;
			return;
		}

		if (!check_coord(p))
			return;
		x1 = atoi(p);
		if (x1 < 0 || x1 > img->col) {
			INVAL_COORD; return;
		}
	} else {
		INVALID; return;
	}

	p = strtok(NULL, "\n ");
	//	if argument exists
	if (p) {
		if (!check_coord(p))
			return;
		y1 = atoi(p);
		if (y1 < 0 || y1 > img->lin) {
			INVAL_COORD; return;
		}
	} else {
		INVALID; return;
	}

	p = strtok(NULL, "\n ");
	//	if argument exists
	if (p) {
		if (!check_coord(p))
			return;
		x2 = atoi(p);
		if (x2 < 0 || x2 > img->col || x1 == x2) {
			INVAL_COORD; return;
		}
	} else {
		INVALID; return;
	}

	p = strtok(NULL, "\n ");
	//	if argument exists
	if (p) {
		if (!check_coord(p))
			return;
		y2 = atoi(p);
		if (y2 < 0 || y2 > img->lin || y1 == y2) {
			INVAL_COORD; return;
		}
	} else {
		INVALID; return;
	}

	//	arguments could be inputed in the wrong order
	if (x1 < x2) {
		img->x1 = x1; img->x2 = x2;
	} else {
		img->x1 = x2; img->x2 = x1;
	}

	if (y1 < y2) {
		img->y1 = y1; img->y2 = y2;
	} else {
		img->y1 = y2; img->y2 = y1;
	}

	SELECT(img->x1, img->y1, img->x2, img->y2);
}

void rotate_90_right(image_t *img)
{
	//	if the whole image is rotated, this coordinates need to be swapped.
	// its new dimensions are the swapped previous ones, ergo the temporary
	// dynamically allocated pixel array should have this new dimensions
	if (img->x1 == 0 && img->y1 == 0 &&
		img->x2 == img->col && img->y2 == img->lin) {
		int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
	}

	if (img->magic == 2 || img->magic == 5) {	//grayscale
		double **a = alloc_gray(img);
		if (!a) {
			end(img); exit(0);
		}
		//	if the whole image is being rotated, this coordinates are once
		// again swapped, because the image has not been rotated yet
		if (img->x1 == 0 && img->y1 == 0 &&
			img->x2 == img->lin && img->y2 == img->col) {
			int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
		}
		//	rotating the image
		int a_i = 0;
		for (int j = 0; j < img->x2 - img->x1; j++) {
			int a_j = 0;
			for (int i = 0; i < img->y2 - img->y1; i++)
				a[a_i][a_j++] = img->px_gray[img->y2 - i - 1][img->x1 + j];
			a_i++;
		}

		if (img->x1 == 0 && img->y1 == 0 &&	//	whole image
			img->x2 == img->col && img->y2 == img->lin) {
			end(img); img->px_gray = a;
			//	image rotated, dimensions have changed
			int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
			aux = img->lin; img->lin = img->col; img->col = aux;
		} else {	//	selected zone
			for (int i = 0; i < img->y2 - img->y1; i++) {
				for (int j = 0; j < img->x2 - img->x1; j++)
					img->px_gray[img->y1 + i][img->x1 + j] = a[i][j];
				free(a[i]);
			} free(a);
		} return;
	}
	//	RGB
	double ***a = alloc_rgb(img);
	if (!a) {
		end(img); exit(0);
	}
	//	if the whole image is rotated, this coordinates are once
	// again swapped, because the image has not been rotated yet
	if (img->x1 == 0 && img->y1 == 0 &&
		img->x2 == img->lin && img->y2 == img->col) {
		int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
	}
	//	rotating the image
	int a_i = 0;
	for (int j = 0; j < img->x2 - img->x1; j++) {
		int a_j = 0;
		for (int i = 0; i < img->y2 - img->y1; i++) {
			for (int k = 0; k < RGB; k++)
				a[a_i][a_j][k] = img->px_rgb[img->y2 - i - 1][img->x1 + j][k];
			a_j++;
		} a_i++;
	}

	if (img->x1 == 0 && img->y1 == 0 &&	//	whole image
		img->x2 == img->col && img->y2 == img->lin) {
		end(img); img->px_rgb = a;
		//	image rotated, dimensions have changed
		int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
		aux = img->lin; img->lin = img->col; img->col = aux;
	} else {	//	selected zone
		for (int i = 0; i < img->y2 - img->y1; i++) {
			for (int j = 0; j < img->x2 - img->x1; j++) {
				for (int k = 0; k < RGB; k++)
					img->px_rgb[img->y1 + i][img->x1 + j][k] = a[i][j][k];
				free(a[i][j]);
			} free(a[i]);
		} free(a);
	}
}

void rotate_180(image_t *img)
{
	//	grayscale
	if (img->magic == 2 || img->magic == 5) {
		double **a = alloc_gray(img);
		if (!a) {
			end(img); exit(0);
		}

		//	rotating the image
		for (int i = 0; i < img->y2 - img->y1; i++)
			for (int j = 0; j < img->x2 - img->x1; j++)
				a[i][j] = img->px_gray[img->y2 - i - 1][img->x2 - j - 1];

		for (int i = 0; i < img->y2 - img->y1; i++) {
			for (int j = 0; j < img->x2 - img->x1; j++)
				img->px_gray[img->y1 + i][img->x1 + j] = a[i][j];
			free(a[i]);
		}
		free(a);
		return;
	}

	//	RGB
	double ***a = alloc_rgb(img);
	if (!a) {
		end(img); exit(0);
	}

	//	rotating the image
	for (int i = 0; i < img->y2 - img->y1; i++)
		for (int j = 0; j < img->x2 - img->x1; j++)
			for (int k = 0; k < RGB; k++)
				a[i][j][k] = img->px_rgb[img->y2 - i - 1][img->x2 - j - 1][k];

	for (int i = 0; i < img->y2 - img->y1; i++) {
		for (int j = 0; j < img->x2 - img->x1; j++) {
			for (int k = 0; k < RGB; k++)
				img->px_rgb[img->y1 + i][img->x1 + j][k] = a[i][j][k];
			free(a[i][j]);
		}
		free(a[i]);
	}
	free(a);
}

void rotate_90_left(image_t *img)
{
	//	if the whole image is rotated, this coordinates need to be swapped.
	// its new dimensions are the swapped previous ones, ergo the temporary
	// dynamically allocated pixel array should have this new dimensions
	if (img->x1 == 0 && img->y1 == 0 &&
		img->x2 == img->col && img->y2 == img->lin) {
		int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
	}

	if (img->magic == 2 || img->magic == 5) {	// grayscale
		double **a = alloc_gray(img);
		if (!a) {
			end(img); exit(0);
		}
		//	if the whole image is being rotated, this coordinates are once
		// again swapped, because the image has not been rotated yet
		if (img->x1 == 0 && img->y1 == 0 &&
			img->x2 == img->lin && img->y2 == img->col) {
			int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
		}
		//	rotating the image
		int a_i = 0;
		for (int j = 0; j < img->x2 - img->x1; j++) {
			int a_j = 0;
			for (int i = 0; i < img->y2 - img->y1; i++)
				a[a_i][a_j++] = img->px_gray[img->y1 + i][img->x2 - j - 1];
			a_i++;
		}

		if (img->x1 == 0 && img->y1 == 0 &&	//	whole image
			img->x2 == img->col && img->y2 == img->lin) {
			end(img); img->px_gray = a;
			//	image rotated, dimensions have changed
			int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
			aux = img->lin; img->lin = img->col; img->col = aux;
		} else {	// selected zone
			for (int i = 0; i < img->y2 - img->y1; i++) {
				for (int j = 0; j < img->x2 - img->x1; j++)
					img->px_gray[img->y1 + i][img->x1 + j] = a[i][j];
				free(a[i]);
			} free(a);
		} return;
	}
	//	RGB
	double ***a = alloc_rgb(img);
	if (!a) {
		end(img); exit(0);
	}
	//	if the whole image is being rotated, this coordinates are once
	// again swapped, because the image has not been rotated yet
	if (img->x1 == 0 && img->y1 == 0 &&
		img->x2 == img->lin && img->y2 == img->col) {
		int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
	}
	//	rotating the image
	int a_i = 0;
	for (int j = 0; j < img->x2 - img->x1; j++) {
		int a_j = 0;
		for (int i = 0; i < img->y2 - img->y1; i++) {
			for (int k = 0; k < RGB; k++)
				a[a_i][a_j][k] = img->px_rgb[img->y1 + i][img->x2 - j - 1][k];
			a_j++;
		} a_i++;
	}

	if (img->x1 == 0 && img->y1 == 0 &&	//	whole image
		img->x2 == img->col && img->y2 == img->lin) {
		end(img); img->px_rgb = a;
		//	image rotated, dimensions have changed
		int aux = img->x2; img->x2 = img->y2; img->y2 = aux;
		aux = img->lin; img->lin = img->col; img->col = aux;
	} else {	// selected zone
		for (int i = 0; i < img->y2 - img->y1; i++) {
			for (int j = 0; j < img->x2 - img->x1; j++) {
				for (int k = 0; k < RGB; k++)
					img->px_rgb[img->y1 + i][img->x1 + j][k] = a[i][j][k];
				free(a[i][j]);
			} free(a[i]);
		} free(a);
	}
}

void rotate(image_t *img)
{
	if (!img->set) {
		NO_LOAD; return;
	}

	//	if the selected zone is neither a square, neither the whole image
	if (img->x1 > 0 || img->y1 > 0 || img->x2 < img->col || img->y2 < img->lin)
		if (img->x2 - img->x1 != img->y2 - img->y1) {
			NOT_SQ; return;
		}

	char *p = strtok(NULL, "\n ");
	int angle = atoi(p);

	if (abs(angle) % 90 != 0 || abs(angle) > 360) {
		BAD_ANGLE; return;
	}

	switch (angle) {
	case 90:
	case -270:
		rotate_90_right(img);
		break;

	case 180:
	case -180:
		rotate_180(img);
		break;

	case 270:
	case -90:
		rotate_90_left(img);
		break;
	}
	ROTATE(p);
}

void crop(image_t *img)
{
	if (!img->set) {
		NO_LOAD; return;
	}

	if (img->magic == 2 || img->magic == 5) {	//	grayscale
		double **a = alloc_gray(img);
		if (!a) {
			end(img); exit(0);
		}

		for (int i = 0; i < img->y2 - img->y1; i++)
			for (int j = 0; j < img->x2 - img->x1; j++)
				a[i][j] = img->px_gray[img->y1 + i][img->x1 + j];

		end(img); img->px_gray = a;
	} else {	//	RGB
		double ***a = alloc_rgb(img);
		if (!a) {
			end(img); exit(0);
		}

		for (int i = 0; i < img->y2 - img->y1; i++)
			for (int j = 0; j < img->x2 - img->x1; j++)
				for (int k = 0; k < RGB; k++)
					a[i][j][k] = img->px_rgb[img->y1 + i][img->x1 + j][k];

		end(img); img->px_rgb = a;
	}

	//	the image's dimensions have changed
	img->lin = img->y2 - img->y1;
	img->col = img->x2 - img->x1;
	img->x1 = 0; img->x2 = img->col;
	img->y1 = 0; img->y2 = img->lin;
	CROP;
}

//	used for APPLY
//	if the given pixel is less then 0, it becomes 0
//	if the given pixel is greater than 255, it becomes 255
double clamp(double x, double min, double max)
{
	if (x < min)
		x = min;
	else if (x > max)
		x = max;
	return x;
}

void apply_edge(image_t *img)
{
	double ***a = alloc_rgb(img);
	if (!a) {
		end(img); exit(0);
	}

	double ker[3][3] = {{-1.0, -1.0, -1.0},
						{-1.0, 8.0, -1.0},
						{-1.0, -1.0, -1.0}};

	for (int i = img->y1; i < img->y2; i++)
		for (int j = img->x1; j < img->x2; j++)
			for (int k = 0; k < RGB; k++)
				if (i > 0 && j > 0 && i < img->lin - 1 && j < img->col - 1) {
					double sum = img->px_rgb[i - 1][j - 1][k] * ker[2][2] +
							  img->px_rgb[i - 1][j][k] * ker[2][1] +
							  img->px_rgb[i - 1][j + 1][k] * ker[2][0] +
							  img->px_rgb[i][j - 1][k] * ker[1][2] +
							  img->px_rgb[i][j][k] * ker[1][1] +
							  img->px_rgb[i][j + 1][k] * ker[1][0] +
							  img->px_rgb[i + 1][j - 1][k] * ker[0][2] +
							  img->px_rgb[i + 1][j][k] * ker[0][1] +
							  img->px_rgb[i + 1][j + 1][k] * ker[0][0];
					a[i - img->y1][j - img->x1][k] =
					clamp(sum, 0.0, (double)img->px_max);
				} else {
					a[i - img->y1][j - img->x1][k] = img->px_rgb[i][j][k];
				}

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			for (int k = 0; k < RGB; k++)
				img->px_rgb[i][j][k] = a[i - img->y1][j - img->x1][k];
			free(a[i - img->y1][j - img->x1]);
		}
		free(a[i - img->y1]);
	}
	free(a);
}

void apply_sharpen(image_t *img)
{
	double ***a = alloc_rgb(img);
	if (!a) {
		end(img); exit(0);
	}

	double ker[3][3] = {{0.0, -1.0, 0.0},
						{-1.0, 5.0, -1.0},
						{0.0, -1.0, 0.0}};

	for (int i = img->y1; i < img->y2; i++)
		for (int j = img->x1; j < img->x2; j++)
			for (int k = 0; k < RGB; k++)
				if (i > 0 && j > 0 && i < img->lin - 1 && j < img->col - 1) {
					double sum = img->px_rgb[i - 1][j - 1][k] * ker[2][2] +
							  img->px_rgb[i - 1][j][k] * ker[2][1] +
							  img->px_rgb[i - 1][j + 1][k] * ker[2][0] +
							  img->px_rgb[i][j - 1][k] * ker[1][2] +
							  img->px_rgb[i][j][k] * ker[1][1] +
							  img->px_rgb[i][j + 1][k] * ker[1][0] +
							  img->px_rgb[i + 1][j - 1][k] * ker[0][2] +
							  img->px_rgb[i + 1][j][k] * ker[0][1] +
							  img->px_rgb[i + 1][j + 1][k] * ker[0][0];
					a[i - img->y1][j - img->x1][k] =
					clamp(sum, 0.0, (double)img->px_max);
				} else {
					a[i - img->y1][j - img->x1][k] = img->px_rgb[i][j][k];
				}

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			for (int k = 0; k < RGB; k++)
				img->px_rgb[i][j][k] = a[i - img->y1][j - img->x1][k];
			free(a[i - img->y1][j - img->x1]);
		}
		free(a[i - img->y1]);
	}
	free(a);
}

void apply_blur(image_t *img)
{
	double ***a = alloc_rgb(img);
	if (!a) {
		end(img); exit(0);
	}

	double ker[3][3] = {{0.1111111111, 0.1111111111, 0.1111111111},
						{0.1111111111, 0.1111111111, 0.1111111111},
						{0.1111111111, 0.1111111111, 0.1111111111}};

	for (int i = img->y1; i < img->y2; i++)
		for (int j = img->x1; j < img->x2; j++)
			for (int k = 0; k < RGB; k++)
				if (i > 0 && j > 0 && i < img->lin - 1 && j < img->col - 1) {
					double sum = img->px_rgb[i - 1][j - 1][k] * ker[2][2] +
							  img->px_rgb[i - 1][j][k] * ker[2][1] +
							  img->px_rgb[i - 1][j + 1][k] * ker[2][0] +
							  img->px_rgb[i][j - 1][k] * ker[1][2] +
							  img->px_rgb[i][j][k] * ker[1][1] +
							  img->px_rgb[i][j + 1][k] * ker[1][0] +
							  img->px_rgb[i + 1][j - 1][k] * ker[0][2] +
							  img->px_rgb[i + 1][j][k] * ker[0][1] +
							  img->px_rgb[i + 1][j + 1][k] * ker[0][0];
					a[i - img->y1][j - img->x1][k] =
					clamp(sum, 0.0, (double)img->px_max);
				} else {
					a[i - img->y1][j - img->x1][k] = img->px_rgb[i][j][k];
				}

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			for (int k = 0; k < RGB; k++)
				img->px_rgb[i][j][k] = a[i - img->y1][j - img->x1][k];
			free(a[i - img->y1][j - img->x1]);
		}
		free(a[i - img->y1]);
	}
	free(a);
}

void apply_gaussian(image_t *img)
{
	double ***a = alloc_rgb(img);
	if (!a) {
		end(img); exit(0);
	}

	double ker[3][3] = {{0.0625, 0.125, 0.0625},
						{0.125, 0.25, 0.125},
						{0.0625, 0.125, 0.0625}};

	for (int i = img->y1; i < img->y2; i++)
		for (int j = img->x1; j < img->x2; j++)
			for (int k = 0; k < RGB; k++)
				if (i > 0 && j > 0 && i < img->lin - 1 && j < img->col - 1) {
					double sum = img->px_rgb[i - 1][j - 1][k] * ker[2][2] +
							  img->px_rgb[i - 1][j][k] * ker[2][1] +
							  img->px_rgb[i - 1][j + 1][k] * ker[2][0] +
							  img->px_rgb[i][j - 1][k] * ker[1][2] +
							  img->px_rgb[i][j][k] * ker[1][1] +
							  img->px_rgb[i][j + 1][k] * ker[1][0] +
							  img->px_rgb[i + 1][j - 1][k] * ker[0][2] +
							  img->px_rgb[i + 1][j][k] * ker[0][1] +
							  img->px_rgb[i + 1][j + 1][k] * ker[0][0];
					a[i - img->y1][j - img->x1][k] =
					clamp(sum, 0.0, (double)img->px_max);
				} else {
					a[i - img->y1][j - img->x1][k] = img->px_rgb[i][j][k];
				}

	for (int i = img->y1; i < img->y2; i++) {
		for (int j = img->x1; j < img->x2; j++) {
			for (int k = 0; k < RGB; k++)
				img->px_rgb[i][j][k] = a[i - img->y1][j - img->x1][k];
			free(a[i - img->y1][j - img->x1]);
		}
		free(a[i - img->y1]);
	}
	free(a);
}

void apply(image_t *img)
{
	if (!img->set) {
		NO_LOAD; return;
	}

	char *p = strtok(NULL, "\n ");
	//	checks if argument was passed
	if (!p) {
		INVALID; return;
	}

	//	cannot perform APPLY on grayscale image
	if (img->magic == 2 || img->magic == 5) {
		CHARLIE; return;
	}

	if (strcmp(p, "EDGE") == 0) {
		apply_edge(img);
	} else if (strcmp(p, "SHARPEN") == 0) {
		apply_sharpen(img);
	} else if (strcmp(p, "BLUR") == 0) {
		apply_blur(img);
	} else if (strcmp(p, "GAUSSIAN_BLUR") == 0) {
		apply_gaussian(img);
	} else {
		INVAL_APPLY; return;
	}
	APPLY(p);
}

void save(image_t *img)
{
	if (!img->set) {
		NO_LOAD; return;
	}

	char *p = strtok(NULL, "\n ");
	FILE *out = fopen(p, "w");
	if (!out) {
		end(img); exit(0);
	}
	SAVE(p);

	p = strtok(NULL, "\n ");
	int magic;
	if (p) {	//	if the optional argument "ascii" is inputed
		if (img->magic == 2 || img->magic == 5)	//	grayscale
			magic = 2;
		else	//	RGB
			magic = 3;
	} else {	//	if the optional argument "ascii" is NOT inputed
		if (img->magic == 2 || img->magic == 5)	//	grayscale
			magic = 5;
		else	//	RGB
			magic = 6;
	}

	fprintf(out, "P%d\n", magic);
	fprintf(out, "%d %d\n", img->col, img->lin);
	fprintf(out, "%d\n", img->px_max);

	//	the saved image format is based on whether the user has inputed the
	// optional argument "ascii" or not regardless of the previous format
	switch (magic) {
	case 2:	//	plain grayscale
		for (int i = 0; i < img->lin; i++) {
			for (int j = 0; j < img->col; j++) {
				int x = (int)round(img->px_gray[i][j]);
				fprintf(out, "%d ", x);
			}
			fprintf(out, "\n");
		}
		break;

	case 3:	//	plain RGB
		for (int i = 0; i < img->lin; i++) {
			for (int j = 0; j < img->col; j++)
				for (int k = 0; k < RGB; k++) {
					int x = (int)round(img->px_rgb[i][j][k]);
					fprintf(out, "%d ", x);
			}
			fprintf(out, "\n");
		}
		break;

	case 5:	// binary grayscale
		for (int i = 0; i < img->lin; i++)
			for (int j = 0; j < img->col; j++) {
				unsigned char c = (unsigned char)round(img->px_gray[i][j]);
				fwrite(&c, sizeof(unsigned char), 1, out);	//	!!!
			}
		break;

	case 6:	// binary RGB
		for (int i = 0; i < img->lin; i++)
			for (int j = 0; j < img->col; j++)
				for (int k = 0; k < RGB; k++) {
					unsigned char c = (unsigned char)
									   round(img->px_rgb[i][j][k]);
					fwrite(&c, sizeof(unsigned char), 1, out);	//	!!!
			}
		break;
	}

	fclose(out);
}
