#include "PluginMenuBarUI.h"

#include "Common/Utility/Utility.h"

#include "Core/GameObject/GameObject.h"
#include "Core/Scene/Scene.h"
#include "Core/Render/Material/Material.h"
#include "Core/Mesh/MeshRenderer.h"

#include "Editor/Plugin/PluginPathSelectDlg/PluginPathSelectDlg.h"
#include "Editor/Plugin/PluginMessageBoxDlg/PluginMessageBoxDlg.h"

void Mrk::PluginMenuBarUI::Draw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Create"))
			{
				Mrk::PluginProjectCreater::GetInstance()->SelectFile();
			}
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

		if (ImGui::BeginMenu("Assert"))
		{
			if (ImGui::MenuItem("Import"))
			{
				Mrk::PluginAssetImport::GetInstance()->SelectFiles();
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Scene"))
		{
			if (ImGui::MenuItem("Load(.msce)"))
			{
				Mrk::PluginSceneLoader::GetInstance()->SelectFile();
			}

			if (ImGui::MenuItem("Save(.msce)"))
			{
				Mrk::PluginSceneSaver::GetInstance()->SelectFile();
			}

			if (ImGui::MenuItem("New"))
			{
				Mrk::SceneHut::CreateNew("Scene", 10000);
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("GameObject"))
		{
			if (ImGui::MenuItem("Load(.mpre)"))
			{
				Mrk::Editor::PluginPathSelectDlg::GetInstance()->SelectFile([](const std::filesystem::path& path) {
					if (auto obj = GameObjectFactory::CreateNew(Mrk::Utility::ReadJson(path)))
					{
						if (auto parent = PluginObjectSelecter::GetInstance()->GetSelection())
						{
							parent->AddChild(obj);
						}
						else if (auto scene = SceneHut::GetCurrScene())
						{
							scene->GetRoot()->AddChild(obj);
						}
						else
						{
							SceneHut::CreateNew("Scene", 1000);
							SceneHut::GetCurrScene()->GetRoot()->AddChild(obj);
						}
					}
					});
				
			}

			if (ImGui::BeginMenu("GameObject"))
			{
				auto& manifest = Mrk::GameObjectFactory::GetManifest();
				for (auto& item : manifest)
				{
					if (ImGui::MenuItem(item.c_str()))
					{
						auto obj = Mrk::GameObjectFactory::CreateNew(item);
						if (auto selection = Mrk::PluginObjectSelecter::GetInstance()->GetSelection())
						{
							selection->AddChild(obj);
						}
						else if (auto scene = Mrk::SceneHut::GetCurrScene())
						{
							scene->GetRoot()->AddChild(obj);
						}
					}
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Test"))
		{
			if (ImGui::MenuItem("Hello"))
			{
				PluginMessageBoxDlg::GetInstance()->Pop("Hello");
			}

			if (ImGui::MenuItem("World"))
			{
				PluginMessageBoxDlg::GetInstance()->Pop("World");
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Material(Temp)"))
		{
			if (auto selection = PluginObjectSelecter::GetInstance()->GetSelection())
			{
				if (auto meshRenderer = selection->GetComponent<MeshRenderer>())
				{
					auto& matSlots = meshRenderer->GetMatSlots();
					for (auto& slot : matSlots)
					{
						
					}
				}
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}
