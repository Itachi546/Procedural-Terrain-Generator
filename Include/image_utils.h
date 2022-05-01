#ifndef IMAGE_UTILS_H
#define IMAGE_UTILS_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

struct ImageHeader {
	int width;
	int height;
	int nChannel;
};

class ImageUtils
{
public:
	static float* LoadImageFloat(const char* filename, ImageHeader& header)
	{
		float* data = stbi_loadf(filename, &header.width, &header.height, &header.nChannel, 0);
		if (data == nullptr)
		{
			fprintf(stderr, "Failed to load image: %s\n", filename);
			return nullptr;
		}
		return data;
	}

	static unsigned char* LoadImage(const char* filename, ImageHeader& header)
	{
		unsigned char* data = stbi_load(filename, &header.width, &header.height, &header.nChannel, 0);
		if (data == nullptr)
		{
			fprintf(stderr, "Failed to load image: %s\n", filename);
			return nullptr;
		}
		return data;
	}

	static void FreeImage(void* data)
	{
		stbi_image_free(data);
	}
};

#endif