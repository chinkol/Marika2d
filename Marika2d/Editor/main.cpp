#include <iostream>

#include "Core/Application/Application.h"

void EditorLoop()
{
	ImGui::DockSpaceOverViewport();

	//viewport
	ImGui::Begin("main viewport");
	ImGui::End();

	ImGui::Begin("edit viewport");
	ImGui::End();

	ImGui::Begin("debug logger");
	ImGui::End();

	ImGui::Begin("component watcher");
	ImGui::End();

	ImGui::Begin("project manager");
	ImGui::End();

	ImGui::Begin("scene tree");
	ImGui::End();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
			}
			if (ImGui::MenuItem("Save"))
			{
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit"))
			{
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Undo"))
			{
			}
			if (ImGui::MenuItem("Redo"))
			{
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

int main()
{
	auto context = Mrk::Application::GetAppContext();
	context.mainwndTitle = "Marika Engine Editor";
	context.mainwndBox = { 1280, 800 };
	context.loopCallBacks.push_back([]() {
		EditorLoop();
		});
	Mrk::Application::SetAppContext(context);
	Mrk::Application::Run();
}
