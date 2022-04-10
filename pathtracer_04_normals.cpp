#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "png.h"

// 3D vector (or color, or whatever has 3 floats)
struct Vec3
{
	float x, y, z;
};

// subtract (element-wise) vector a from b
Vec3 sub(Vec3 a, Vec3 b)
{
	return {a.x - b.x, a.y - b.y, a.z - b.z};
}

// add (element-wise) vector a to vector b
Vec3 add(Vec3 a, Vec3 b)
{
	return {a.x + b.x, a.y + b.y, a.z + b.z};
}

// multiply (element-wise) vector a by vector b
Vec3 mul(Vec3 a, Vec3 b)
{
	return {a.x * b.x, a.y * b.y, a.z * b.z};
}

// multiply 3D vector a by scalar s (scale the vector)
Vec3 mul(Vec3 a, float s)
{
	return {a.x * s, a.y * s, a.z * s};
}

// cross product of two 3D vectors
Vec3 cross(Vec3 a, Vec3 b)
{
	return {a.y * b.z - a.z * b.y, a.x * b.z - a.z * b.x, a.x * b.y - a.y * b.x};
}

// dot product of two 3D vectors
float dot(Vec3 a, Vec3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

// magnitude - length of vector
float mag(Vec3 a)
{
	return sqrt(dot(a, a));	// because dot(a,a) is a.x^2 + a.y^2 + a.z^2, which is what we need
}

float saturate(float a)
{
	if(a < 0.0f)
		return 0.0f;
	if(a > 1.0f)
		return 1.0f;
	return a;
}

Vec3 saturate(Vec3 a)
{
	return {saturate(a.x), saturate(a.x), saturate(a.x)};
}

// normalise vector a (scale the vector so its length is equal to 1)
Vec3 norm(Vec3 a)
{
	return mul(a, 1.0f / mag(a));
}

struct Sphere
{
	Vec3 pos;		// position, center of the sphere
	float radius;	// half-size of the sphere
};

struct Ray
{
	Vec3 pos;	// position, where the ray starts
	Vec3 dir;	// direction, where the ray flies, what it looks at
};

void adjust(Ray& r)
{
	r.pos = add(r.pos, mul(r.dir, 0.0001f));
}

struct Hit
{
	Vec3 pos;		// where the ray hit the object
	float distance;	// distance along ray to the hit position, used for comparing two intersections (we need to know which one is closer)
	Vec3 normal;	// normal of the surface hit
};

// test intersection (collision) between ray and sphere
bool intersect(Ray ray, Sphere sphere, Hit& hit)
{
	Vec3 c = sub(sphere.pos, ray.pos);
	float d = mag(cross(ray.dir, c));
	
	// if distance between sphere center and ray is less than or equal radius, then we have a hit!
	if(d <= sphere.radius)
	{
		float t1 = dot(ray.dir, c);
		float t2 = sqrt(sphere.radius * sphere.radius - d * d);
		
		hit.distance = t1 - t2;
		hit.pos = add(ray.pos, mul(ray.dir, hit.distance));
		hit.normal = norm(sub(hit.pos, sphere.pos));
		
		if(dot(ray.dir, hit.normal) > 0.0f)
		{
			hit.normal = mul(hit.normal, -1.0f);
		}
		
		return true;
	}
	
	return false;
}

Vec3 render(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	// camera
	Vec3 camera_pos = {0.0f, 0.0f, -3.0f};
	float camera_near = 0.5f;	// distance from camera position to the near plane of the camera
	
	// pixel position is on the camera near plane
	float aspect_ratio = width / (float)height;
	Vec3 pixel_pos = {
		aspect_ratio * (float)x / (float)width - (aspect_ratio - 1.0f) * 0.5f - 0.5f,
		(float)y / (float)height - 0.5f,
		camera_pos.z + camera_near
	};
	
	// ray starting at pixel position
	Ray ray;
	ray.pos = pixel_pos;
	ray.dir = norm(sub(pixel_pos, camera_pos));
	
	Sphere sphere = {{0.0f, 0.0f, 0.0f}, 1.0f};	// sphere at position 0,0,0 with radius 0.2
	
	// if ray doesn't hit anything, return background color
	Hit hit = {};
	if(!intersect(ray, sphere, hit))
	{
		// return white-blue gradient
		Vec3 white = {1.0f, 1.0f, 1.0f};
		Vec3 blue = {0.5f, 0.7f, 1.0f};
		float t = saturate(0.5f * (ray.dir.y + 1.0f));
		return add(mul(white, t), mul(blue, 1.0f - t));
	}
	
	return mul(add(hit.normal, {1.0f, 1.0f, 1.0f}), 0.5f);
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
