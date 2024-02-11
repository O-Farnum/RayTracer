#include "Renderer.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"

using namespace std;

void Renderer::Resize(uint32_t width, uint32_t height)
{
	if (final_image)
	{
		if (final_image->GetWidth() == width && final_image->GetHeight() == height) { return; } //Resizing the image isn't necessary 

		final_image->Resize(width, height);
	}
	else
	{
		final_image = make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA); //Make new image with viewport size

	}
	delete[] imageData; //Delete any old data
	imageData = new uint32_t[width * height]; //Create an image buffer
}

void Renderer::Render()
{

	//Fill pixels
	for (uint32_t y = 0; y < final_image->GetHeight(); y++)
	{
		for (uint32_t x = 0; x < final_image->GetWidth(); x++)
		{
			glm::vec2 xy_loc((float)x / final_image->GetWidth(), (float)y / final_image->GetHeight());//Determine pixel's location within the scene 
																									  //TODO:Make copies of x and y that are floats so CPU doesn't have to convert every call
			xy_loc = (xy_loc * 2.0f) - 1.0f; //Remap the coordinate from 0 -> 1 to -1 -> 1
			imageData[x + y * final_image -> GetWidth()] = ColorPixel(xy_loc); //Generate a color for the pixel
		}
	}
	final_image->SetData(imageData);
}

//Function to return the color a pixel should be
uint32_t Renderer::ColorPixel(glm::vec2 xy_loc)
{
	glm::vec3 ray_origin(0.0f, 0.0f, 2.0f); //Camera Location x: horizontal, y: vertical, z: depth (forward is neg, back is pos)
	glm::vec3 ray_direct( xy_loc.x, xy_loc.y, -1.0f );
	float radius = 0.5f;

	//(bx^2 + by^2)t^2 + (2(axbx +ayby))t + (ax^2 + ay^2 - r^2) = 0
	//a: orgin of ray b: direction of ray r: radius t: distance of hit

	float first_coeff  = glm::dot(ray_direct, ray_direct); //dot product of the ray's direction against itself
	float second_coeff = 2.0f * glm::dot(ray_origin, ray_direct); // dot product of the ray's origin against its direction
	float third_coeff = glm::dot(ray_origin, ray_origin) - (radius * radius); //dot product of the ray's origin against itself, minus the radius squared

	//b^2 - 4ac :Discriminate of the Quadratic Formula
	float discriminate = (second_coeff * second_coeff) - (4.0f * first_coeff * third_coeff);// >0: 2 Solutions, ==0: 1 Solution, <0: 0 Solutions

	if (discriminate >= 0.0f) { return 0xff000000 | Walnut::Random::UInt(); } //Ray hits

	return 0xff000000;
}