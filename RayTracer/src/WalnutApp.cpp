#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include "Walnut/Image.h"
#include "Walnut/Timer.h"
#include "Renderer.h"

using namespace std;

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
			ImGui::Text("Time to render scene: %.3fms", renderTime);
			ImGui::Text("%.3ffps", fps);

		ImGui::End();

		//Window: Displays rendered scene
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Scene");
		
			sceneWidth = ImGui::GetContentRegionAvail().y;//x and y are flipped?
			sceneHeight = ImGui::GetContentRegionAvail().x;

			auto image = renderer.GetFinal();

			if (image) 
			{
				ImGui::Image(image->GetDescriptorSet(), { (float)image->GetHeight(), (float)image->GetWidth() });
			}

			Render();
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void Render()
	{
		Walnut::Timer timer;

		renderer.Resize(sceneWidth, sceneHeight);
		renderer.Render();

		 renderTime = timer.ElapsedMillis();
		 fps = 1 / ( renderTime / 1000);
	}

private: 
	Renderer renderer;
	uint32_t sceneWidth, sceneHeight;
	float renderTime, fps;
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