#include "Renderer.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"

using namespace std;

void Renderer::Render()
{

	//Fill pixels
	for (uint32_t i = 0; i < final_image->GetWidth() * final_image->GetHeight(); i++)
	{
		imageData[i] = Walnut::Random::UInt();
		imageData[i] |= 0xff000000;
	}
	final_image->SetData(imageData);
}

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
