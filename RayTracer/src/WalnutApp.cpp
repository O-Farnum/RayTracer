#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Random.h"
#include "Walnut/Timer.h"

using namespace std;
using namespace Walnut;

class ExampleLayer : public Walnut::Layer
{
public:
	virtual void OnUIRender() override
	{
		//Window: Contains settings controls for the scene 
		ImGui::Begin("Settings");
		
			if (ImGui::Button("Render"))
			{
				Render();
			}
			ImGui::Text("Time to render scene: %.3fms", m_renderTime);
			ImGui::Text("%.3ffps", m_fps);
			if (ImGui::Button("Gradient"))
			{
				RenderGradient();
			}
			ImGui::Text("Time to render gradient: %.3fms", m_gradientTime);

		ImGui::End();

		//Window: Displays rendered scene
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Scene");
		
			m_sceneWidth = ImGui::GetContentRegionAvail().y;//x and y are flipped?
			m_sceneHeight = ImGui::GetContentRegionAvail().x;

			if (m_image) 
			{
				ImGui::Image(m_image->GetDescriptorSet(), { (float)m_image->GetHeight(), (float)m_image->GetWidth() });
			}

			Render();
		ImGui::End();

		//Gradient Scene
		ImGui::Begin("Gradient");

			m_gradientWidth = ImGui::GetContentRegionAvail().y;//x and y are flipped?
			m_gradientHeight = ImGui::GetContentRegionAvail().x;

			if (m_gradient)
			{
				ImGui::Image(m_gradient->GetDescriptorSet(), { (float)m_gradient->GetHeight(), (float)m_gradient->GetWidth() });
			}

			//RenderGradient();
		ImGui::End();
		ImGui::PopStyleVar();
		//ImGui::ShowDemoWindow();
		counter += 1;
	}

	void Render()
	{
		Timer timer;
		//Check if there is no image or if the window has been resized
		if (!m_image || m_sceneHeight != m_image ->GetHeight() || m_sceneWidth != m_image -> GetWidth())
		{
			m_image = make_shared<Image>(m_sceneWidth, m_sceneHeight, ImageFormat::RGBA); //Make new image with viewport size
			delete[] m_imageData; //Delete any old data
			m_imageData = new uint32_t[m_sceneWidth * m_sceneHeight]; //Create an image buffer
		}
		//Fill each pixel
		for (uint32_t i = 0; i < m_sceneWidth * m_sceneHeight; i++)
		{
			m_imageData[i] = Random::UInt();
			m_imageData[i] |= 0xff000000;
		}

		m_image->SetData(m_imageData);
		m_renderTime = timer.ElapsedMillis();
		m_fps = 1 / (m_renderTime / 1000);
	}

	void RenderGradient()
	{
		Timer gradTimer;
		//Check if there is no image or if the window has been resized
		if (!m_gradient || m_gradientHeight != m_gradient->GetHeight() || m_gradientWidth != m_gradient->GetWidth())
		{
			m_gradient = make_shared<Image>(m_gradientWidth, m_gradientHeight, ImageFormat::RGBA); //Make new image with viewport size
			delete[] m_gradientData; //Delete any old data
			m_gradientData = new uint32_t[m_gradientWidth * m_gradientHeight]; //Create an image buffer
		}

		float x_deviation = (static_cast<float>(1) / m_gradientWidth);
		float y_deviation = (static_cast<float>(1) / m_gradientHeight);

		//Fill each pixel
		int j = 0, k = 0;
		float red = 0.00, green = 0.00;
		for (uint32_t i = 0; i < m_gradientWidth * m_gradientHeight; i++)
		{
			//set red value
			if (j != m_gradientWidth) 
			{
				red = ((float)j + (counter * x_deviation)) / (m_gradientWidth - 1);
				if (red >= 255) { red -= 255; }
				j += 1;
			}
			else
			{
				red = 0;
				j = 1; k += 1;
			}
			//set green value
			if (k != m_gradientHeight)
			{
				green = ((float)k + (counter * y_deviation)) / (m_gradientHeight - 1);
				if (green >= 255) { green -= 255; }
			}
			else
			{
				green = 0;
				k = 1;
			}

			if (red == 0 && green == 0 && counter != 0) { counter = 0; }
			red = (red * 255); green = (green * 255); blue = 0;

			string hex_red = (rgb_to_hex(red));
			string hex_green = (rgb_to_hex(green));
			if (hex_red.length() < 2) { hex_red = "0" + hex_red; };
			if (hex_green.length() < 2) { hex_green = "0" + hex_green;};

			string ans = "0xff00" + hex_green + hex_red;
			m_gradientData[i] = stoul(ans, nullptr, 16);
		}

		m_gradient->SetData(m_gradientData);
		m_gradientTime = gradTimer.ElapsedMillis();
	}

	string rgb_to_hex(int num)
	{
		if (num == 0) return "00";

		string res = "";
		while (num != 0)
		{
			int rem = 0; char temp;
			rem = num % 16;
			if (rem < 10)
			{
				temp = rem + 48;
			}
			else
			{
				temp = rem + 55;
			}

			res += temp;
			num = num / 16;
		}

		// reversing the ans string to get the final result
		int i = 0, j = res.size() - 1;
		while (i <= j)
		{
			swap(res[i], res[j]);
			i++;
			j--;
		}

		return res;
	}

private: 
	uint32_t* m_imageData = nullptr;
	uint32_t* m_gradientData = nullptr;
	shared_ptr<Image> m_image;
	shared_ptr<Image> m_gradient;
	uint32_t m_sceneWidth, m_sceneHeight;
	uint32_t m_gradientWidth, m_gradientHeight;
	uint32_t red, green, blue;
	float m_renderTime, m_fps;
	float m_gradientTime;
	int counter = 0;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "Ray Tracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<ExampleLayer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}