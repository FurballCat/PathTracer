#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "png.h"

int main(int argc, const char * argv[])
{
	// settings
	const uint32_t width = 1024;
	const uint32_t height = 768;
	
	// useful variables
	const uint32_t stride = 3;
	const uint32_t image_size = width * height * stride;
	
	// allocate and 'zero' (clear) image memory
	void* image = malloc(image_size);
	memset(image, 0, image_size);
	
	// save image to 'render.png'
	int32_t res = stbi_write_png("render.png", width, height, 3, image, width * stride);
	
	if(res)
		printf("Saved to render.png\n");
	else
		printf("Cannot save to render.png\n");
	
	// release image memory
	free(image);
	
	return res;
}
