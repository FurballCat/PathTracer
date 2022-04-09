#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "png.h"

// 3D vector (or color, or whatever has 3 floats)
struct Vec3
{
	float x, y, z;
};

Vec3 render(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	// draw rectangle in the center
	if(100 < x && x < 924 && 100 < y && y < 668)
		return {1.0f, 0.0f, 0.0f};
	
	// return black
	return {0.0f, 0.0f, 0.0f};
}

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
	
	uint8_t* pixel = (uint8_t*)image;
	
	// render pixels
	for(uint32_t y=0; y<height; ++y)
	{
		for(uint32_t x=0; x<width; ++x)
		{
			// render single pixel
			Vec3 color = render(x, y, width, height);
			
			// translate from Vec3 color to bytes color
			pixel[0] = color.x * 255.0f;
			pixel[1] = color.y * 255.0f;
			pixel[2] = color.z * 255.0f;
			
			// move to next pixel
			pixel += stride;
		}
	}
	
	// save image to 'render.png'
	int32_t res = stbi_write_png("render.png", width, height, 3, image, stride * width);
	
	if(res)
		printf("Saved to render.png\n");
	else
		printf("Cannot save to render.png\n");
	
	// release image memory
	free(image);
	
	return res;
}
