//	Copyright Manea Dorin-Mihai 313CAb 2021-2022

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

//	constants
#define LMAX 100
#define MAGIC 3
#define RGB 3

//	outputs to stdout
#define INVALID puts("Invalid command")
#define NO_LOAD puts("No image loaded")
#define INVAL_COORD puts("Invalid set of coordinates")
#define CHARLIE puts("Easy, Charlie Chaplin")
#define INVAL_APPLY puts("APPLY parameter invalid")
#define CROP puts("Image cropped")
#define NOT_SQ puts("The selection must be square")
#define BAD_ANGLE puts("Unsupported rotation angle")
#define FAIL(p) printf("Failed to load %s\n", p)
#define LOAD(p) printf("Loaded %s\n", p)
#define SELECT_A printf("Selected ALL\n")
#define SELECT(x1, y1, x2, y2) printf("Selected %d %d %d %d\n", x1, y1, x2, y2)
#define ROTATE(ang) printf("Rotated %s\n", ang)
#define APPLY(effect) printf("APPLY %s done\n", effect)
#define SAVE(p) printf("Saved %s\n", p)

//	image structure
typedef struct __attribute__((__packed__)) {
	//	magic-word, width, height, maximum pixel intensity
	int magic, col, lin, px_max;
	//	grayscale pixel array, RGB pixel array
	//	double needed for APPLY
	double **px_gray, ***px_rgb;
	//	current working zone
	int x1, x2, y1, y2;
	//	whether any image is loaded or not
	int set;
} image_t;

//	functions
void end(image_t *img);

double **alloc_gray(image_t *img);
double ***aloc_rgb(image_t *img);

double **load_gray(image_t *img, FILE *in);
double ***load_rgb(image_t *img, FILE *in);
void check_comment(FILE *in);
void load(image_t *img);

int check_coord(char *p);
void zone(image_t *img);

void rotate_90_right(image_t *img);
void rotate_180(image_t *img);
void rotate_90_left(image_t *img);
void rotate(image_t *img);

void crop(image_t *img);

double clamp(double x, double min, double max);
void apply_edge(image_t *img);
void apply_sharpen(image_t *img);
void apply_blur(image_t *img);
void apply_gaussian(image_t *img);
void apply(image_t *img);

void save(image_t *img);

#endif	//	__COMMANDS_H
