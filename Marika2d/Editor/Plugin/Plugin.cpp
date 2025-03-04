#include "Plugin.h"

#include "Editor/Asset/AssetSys.h"

#include "Core/Scene/Scene.h"
#include "Core/GameObject/GameObject.h"
#include "Core/Camera/Camera.h"

#include "Third/imgui/imgui_internal.h"

void Mrk::PluginSys::Init()
{
	Instance().plugins.clear();
	for (auto& creator : Instance().creators)
	{
		auto plugin = creator();
		plugin->Init();
		Instance().plugins.push_back(plugin);
	}
}

void Mrk::PluginSys::Update()
{
	for (auto& plugin : Instance().plugins)
	{
		plugin->Update();
	}
}

void Mrk::PluginSys::Draw()
{
	for (auto& plugin : Instance().plugins)
	{
		plugin->Draw();
	}
}

void Mrk::PluginAssetImport::SelectFiles()
{
	fileDlg.Open();
}

void Mrk::PluginAssetImport::Init()
{
	fileDlg = ImGui::FileBrowser(ImGuiFileBrowserFlags_MultipleSelection | ImGuiFileBrowserFlags_ConfirmOnEnter | ImGuiFileBrowserFlags_CloseOnEsc);
	fileDlg.SetTitle("Import Asset");
	fileDlg.SetTypeFilters(AssetSys::GetImportSupports());

	saveDlg = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_HideRegularFiles | ImGuiFileBrowserFlags_CreateNewDir, ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
	saveDlg.SetTitle("Save Asset To");
}

void Mrk::PluginAssetImport::Update()
{
	fileDlg.Display();
	if (fileDlg.HasSelected())
	{
		fromPathes = fileDlg.GetMultiSelected();
		fileDlg.ClearSelected();
		saveDlg.Open();
	}

	saveDlg.Display();
	if (saveDlg.HasSelected())
	{
		toPath = saveDlg.GetDirectory();
		saveDlg.ClearSelected();
	}

	if (!fromPathes.empty() && !toPath.empty())
	{
		for (auto& fromPath : fromPathes)
		{
			AssetSys::Import(fromPath, toPath);
		}
		fromPathes.clear();
		toPath.clear();
	}
}

void Mrk::PluginProjectCreater::SelectFile()
{
	pathSelectDlg.Open();
}

void Mrk::PluginProjectCreater::CreateNewProj(const std::filesystem::path& path)
{
	std::filesystem::create_directory(path);
	//TODO
}

void Mrk::PluginProjectCreater::Init()
{
	pathSelectDlg = ImGui::FileBrowser(ImGuiFileBrowserFlags_SelectDirectory);
	pathSelectDlg.SetTitle("Create Project");
}

void Mrk::PluginProjectCreater::Update()
{
	pathSelectDlg.Display();
	if (pathSelectDlg.HasSelected())
	{
		projectPath = pathSelectDlg.GetSelected();
		pathSelectDlg.ClearSelected();

		ImGui::OpenPopup((char*)u8"工程名称");
	}

	if (ImGui::BeginPopupModal((char*)u8"工程名称"))
	{
		ImGui::InputText((char*)u8"工程名称", projectName.data(), projectName.size());
		if (ImGui::Button("OK") && projectName != empty)
		{
			CreateNewProj(projectPath / projectName);
			projectName = empty;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void Mrk::PluginSceneTreeUI::Draw()
{
	ImGui::Begin("SceneTree");

	if (auto mainScene = SceneHut::GetCurrScene())
	{
		CreateTreeNode(mainScene->GetRoot());
	};


	ImGui::End();
}

void Mrk::PluginSceneTreeUI::CreateTreeNode(std::shared_ptr<GameObject> node)
{
	if (node)
	{
		ImGui::PushID(std::to_string(node->GetID().total64).c_str());
		if (ImGui::TreeNode(node->GetName().c_str()))
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				PluginObjectSelecter::GetInstance()->SetSelection(node);
			}
			for (auto& child : node->GetChildren())
			{
				CreateTreeNode(child);
			}
			ImGui::TreePop();
		}
		else
		{
			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				PluginObjectSelecter::GetInstance()->SetSelection(node);
			}
		}
		ImGui::PopID();
	}
}

std::shared_ptr<Mrk::GameObject> Mrk::PluginObjectSelecter::GetSelection()
{
	if (!selection.expired())
	{
		return selection.lock();
	}
	return std::shared_ptr<GameObject>();
}

void Mrk::PluginObjectSelecter::SetSelection(std::shared_ptr<GameObject> selection)
{
	if (selection)
	{
		this->selection = selection;
	}
}

void Mrk::PluginViewportUI::Draw()
{
	ImGui::Begin("Viewport");

	auto wndSize = ImGui::GetContentRegionAvail();
	auto wndPos = ImGui::GetCurrentWindow()->WorkRect.Min;

	if (auto mainCamera = Mrk::CameraHut::GetMainCamera())
	{
		if (auto output = mainCamera->GetComponent<CameraOutput>())
		{
			auto& resolution = output->GetResolution();
			if (resolution.x != wndSize.x || resolution.y != wndSize.y)
			{
				output->SetResolution({ wndSize.x , wndSize.y });
			}

			ImVec2 uv0 = ImVec2(0.0f, 1.0f);  // 左下角
			ImVec2 uv1 = ImVec2(1.0f, 0.0f);  // 右上角
			ImGui::Image((ImTextureID)(output->GetBackBuffertexture()), ImVec2(wndSize.x, wndSize.y), uv0, uv1);
		}
	}

	ImGui::End();
}

void Mrk::PluginSceneLoader::SelectFile()
{
	pathSelectDlg.Open();
}

void Mrk::PluginSceneLoader::Init()
{
	pathSelectDlg = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter, ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
	pathSelectDlg.SetTitle("Load Scene");
}

void Mrk::PluginSceneLoader::Update()
{
	pathSelectDlg.Display();
	if (pathSelectDlg.HasSelected())
	{
		auto path = pathSelectDlg.GetSelected();
		if (std::filesystem::exists(path) && path.extension() == ".msce")
		{
			SceneHut::FromFile(path.string());
		}
		pathSelectDlg.ClearSelected();
	}
}

void Mrk::PluginSceneSaver::SelectFile()
{
	pathSelectDlg.Open();
}

void Mrk::PluginSceneSaver::Init()
{
	pathSelectDlg = ImGui::FileBrowser(ImGuiFileBrowserFlags_CloseOnEsc | ImGuiFileBrowserFlags_ConfirmOnEnter | ImGuiFileBrowserFlags_EnterNewFilename, ConfigSys::GetConfigItem<std::string>("AppConfig", "projDir"));
	pathSelectDlg.SetTitle("Load Scene");
}

void Mrk::PluginSceneSaver::Update()
{
	pathSelectDlg.Display();
	if (pathSelectDlg.HasSelected())
	{
		auto path = pathSelectDlg.GetSelected();
		if (path.extension() == ".msce")
		{
			SceneHut::ToFile(path.string());
		}
		pathSelectDlg.ClearSelected();
	}
}