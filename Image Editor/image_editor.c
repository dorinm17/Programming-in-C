//	Copyright Manea Dorin-Mihai 313CAb 2021-2022

#include <stdio.h>
#include <string.h>
#include "commands.h"

int main(void)
{
	image_t img;
	//	initalising the image data
	img.set = 0; img.px_max = 255;
	img.x1 = 0; img.x2 = 0; img.y1 = 0; img.y2 = 0;
	img.lin = 0; img.col = 0;

	//	reading commands from stdin until "EXIT"
	char cmd[LMAX] = {0}, *p = NULL;
	while (strcmp(cmd, "EXIT") != 0) {
		memset(cmd, 0, LMAX);
		fgets(cmd, LMAX, stdin);
		p = strtok(cmd, "\n ");

		if (strcmp(p, "LOAD") == 0)
			load(&img);
		else if (strcmp(p, "SELECT") == 0)
			zone(&img);
		else if (strcmp(p, "ROTATE") == 0)
			rotate(&img);
		else if (strcmp(p, "CROP") == 0)
			crop(&img);
		else if (strcmp(p, "APPLY") == 0)
			apply(&img);
		else if (strcmp(p, "SAVE") == 0)
			save(&img);
		else if (strcmp(p, "EXIT") == 0)
			end(&img);
		else
			INVALID;
	}
	return 0;
}
