#include "Renderer.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"

using namespace std;

namespace Utils
{
	//Convert a vec4 of float colors to a uint32 value
	static uint32_t RGBA_to_AGBR(const glm::vec4& color) //Passing as a const reference to avoid making unnecessary copies
	{
		uint8_t red = (uint8_t)(color.r * 255.0f); //Convert floats to 8 bit unsigned ints
		uint8_t green = (uint8_t)(color.g * 255.0f);
		uint8_t blue = (uint8_t)(color.b * 255.0f);
		uint8_t alpha = (uint8_t)(color.a * 255.0f);

		return (alpha << 24) | (blue << 16) | (green << 8) | red;
	}
	//Solve the quadratic formula for x0 or x1
	static float quad_form(float a, float b, float sqr_discriminate)
	{
		return (-b + sqr_discriminate) / (2 * a); //x0,x1 = (-b +/- sqrt(4 * a * c)) / (2 * a)
	}
}

void Renderer::Resize(uint32_t width, uint32_t height)
{
	if (final_image)
	{
		if (final_image->GetWidth() == width && final_image->GetHeight() == height) { return; } //Resizing the image isn't necessary 

		final_image->Walnut::Image::Resize(width, height);
	}
	else
	{
		final_image = make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA); //Make new image with view-port size

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

			glm::vec4 color = glm::clamp(ColorPixel(xy_loc), glm::vec4(0.0f), glm::vec4(1.0f));
			imageData[x + y * final_image -> GetWidth()] = Utils::RGBA_to_AGBR(color); //Generate a color for the pixel
		}
	}
	final_image->SetData(imageData);
}

//Function to return the color a pixel should be
glm::vec4 Renderer::ColorPixel(glm::vec2 xy_loc)
{
	glm::vec3 ray_origin(0.0f, 0.0f, 2.0f);//Camera Location x: horizontal, y: vertical, z: depth (forward is neg, back is pos)
	glm::vec3 ray_direct( xy_loc.x, xy_loc.y, -1.0f );
	float radius = 0.50f;

	//(bx^2 + by^2)t^2 + (2(axbx +ayby))t + (ax^2 + ay^2 - r^2) = 0
	//a: orgin of ray b: direction of ray r: radius t: distance of hit

	float first_coeff  = glm::dot(ray_direct, ray_direct);//dot product of the ray's direction against itself <- a "squared" dot prod will always be positive
	float second_coeff = 2.0f * glm::dot(ray_origin, ray_direct);// dot product of the ray's origin against its direction
	float third_coeff = glm::dot(ray_origin, ray_origin) - (radius * radius); //dot product of the ray's origin against itself, minus the radius squared

	//b^2 - 4ac :Discriminate of the Quadratic Formula
	float discriminate = (second_coeff * second_coeff) - (4.0f * first_coeff * third_coeff);// >0: 2 Solutions, ==0: 1 Solution, <0: 0 Solutions

	if (discriminate < 0.0f) { return glm::vec4(0, 0, 0, 1); }//Ray hits (RGBA)

	//Finish solving Quad Form for both roots/intersections (hit points)
	//float root_0 = Utils::quad_form(first_coeff, second_coeff, discriminate); <- don't need to calculate right now
	//glm::vec3 hit_0 = ray_origin + ray_direct * root_0; <- don't need to calculate right now

	float root_1 = Utils::quad_form(first_coeff, second_coeff, (-1.0f * glm::sqrt(discriminate)));//Will always be the smallest as long as A is pos which it will be (Line 66)
	glm::vec3 hit_1 = ray_origin + ray_direct * root_1;//closest hit
	glm::vec3 normal = glm::normalize(hit_1 - 0.0f);//Calculate the normal of each hit by normalizing the hit point - origin of the sphere
	glm::vec3 color = (0.50f * normal + 0.50f);//Bring the normal from the -1:1 range to the 0:1 range

	glm::vec3 light_dir = glm::normalize(glm::vec3(-1, -1, -1));//Direction of the light source
	float light_intensity = glm::max((glm::dot(normal, -light_dir)), 0.0f);//Dot Prod of 2 lines = cos(angle)->angle>90 == -1, so max is needed

	//color = glm::vec3(1 * xy_loc.x, 1 * xy_loc.y, 0);//specified gradient
	color = glm::vec3(0, 1, 1);//specified color
	color *= light_intensity;

	return glm::vec4(color, 1);
}