#include "image_processing-intrinsics.h"
#include <arm_neon.h>
#include <android/bitmap.h>
/*
typedef struct
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
}  rgba;

int b(int pa, int pb) {
	if (pb == 0) return pa;
	int p = 255 - ((255 - pa) * 255 / pb);
	return p < 0 ? 0 : p;
}
*/
void burn_neon_intrinsics(AndroidBitmapInfo* info, void* pixels, int width, int height)
{
	/*
	int x, y;

	for (y = 0; y < height; y++) {
		rgba* line = (rgba*) pixels;
		for (x = 0; x < width; x++) {
			rgba pixel;
			pixel.red = b(line[x].red, line[x].red);
			pixel.green = b(line[x].green, line[x].green);
			pixel.blue = b(line[x].blue, line[x].blue);

			pixel.alpha = line[x].alpha;

			line[x] = pixel;
		}

		pixels = (char*) pixels + info->stride;
	}
	*/
}
