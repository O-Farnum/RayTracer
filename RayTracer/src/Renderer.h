#pragma once
#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>

using namespace std;

class Renderer
{
public:
	Renderer() = default;
	void Resize(uint32_t width, uint32_t height);
	void Render();
	shared_ptr<Walnut::Image> GetFinal() const { return final_image; };
private:
	glm::vec4 ColorPixel(glm::vec2 xy_loc);
private:
	shared_ptr<Walnut::Image> final_image;
	uint32_t* imageData = nullptr;
};