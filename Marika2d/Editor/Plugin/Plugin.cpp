#include "Plugin.h"

#include "Editor/Asset/AssetSys.h"

#include "Core/Scene/Scene.h"
#include "Core/GameObject/GameObject.h"
#include "Core/Camera/Camera.h"

#include "Third/imnodes/imnodes.h"
#include "Third/imgui/imgui_internal.h"

#include <format>

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
	ImGui::Begin("SceneTree", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

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

		if (ImGui::TreeNodeEx(node->GetName().c_str(), node->GetChildren().empty() ? ImGuiTreeNodeFlags_Leaf : 0))
		{
			DragDropNode(node);

			if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				PluginObjectSelecter::GetInstance()->SetSelection(node);
			}

			auto& children = node->GetChildren();
			for (auto& child : children)
			{
				CreateTreeNode(child);
			}

			ImGui::TreePop();
		}
		else if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNone) && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			PluginObjectSelecter::GetInstance()->SetSelection(node);
		}

		ImGui::PopID();
	}
}

void Mrk::PluginSceneTreeUI::DragDropNode(std::shared_ptr<GameObject> node)
{
	static std::weak_ptr<GameObject> dragged;

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		dragged = node;
		ImGui::SetDragDropPayload("GameObject", nullptr, 0);
		ImGui::Text(node->GetName().c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject"))
		{
			if (!dragged.expired())
			{
				auto dropped = dragged.lock();

				//TODO : fix : 父节点拖拽到子节点，循环引用内存泄露
				if (dropped != node)	// 确保不能拖到自身
				{
					node->AddChild(dropped);
				}
			}
			dragged.reset();
		}
		ImGui::EndDragDropTarget();
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
				if (!ImGui::IsMouseDragging(0))
				{
					output->SetResolution({ wndSize.x , wndSize.y });
				}
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

void Mrk::PluginPropertiesInspectUI::Draw()
{
	ImGui::Begin("Inspecter", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

	if (auto selection = PluginObjectSelecter::GetInstance()->GetSelection())
	{
		auto& components = selection->GetComponents();
		for (auto& [name, component] : components)
		{
			(void)RecurObject(*component, name);
		}

		auto& manifest = ComponentFactory::GetManifest();
		std::vector<const char*> cstrManifest;
		std::transform(manifest.begin(), manifest.end(), std::back_inserter(cstrManifest), [](const std::string& str) {
			return str.c_str();
			});

		int currItem = 0;
		if (ImGui::Combo("Add Component", &currItem, cstrManifest.data(), (int)(cstrManifest.size())))
		{
			selection->AddComponent(cstrManifest.at(currItem));
		}
	}

	ImGui::End();
}

bool Mrk::PluginPropertiesInspectUI::RecurArithmetic(rttr::variant& arithmetic, std::string_view name)
{
	auto type = arithmetic.get_type();

	if (type == rttr::type::get<bool>())
	{
		auto value = arithmetic.get_value<bool>();
		if (ImGui::Checkbox(name.data(), &value))
		{
			arithmetic = value;
			return true;
		}
	}
	else if (type == rttr::type::get<int16_t>())
	{
		auto value = (int)arithmetic.get_value<int16_t>();
		if (ImGui::InputInt(name.data(), &value))
		{
			arithmetic = (int16_t)value;
			return true;
		}
	}
	else if (type == rttr::type::get<int32_t>())
	{
		auto value = arithmetic.get_value<int32_t>();
		if (ImGui::InputInt(name.data(), &value))
		{
			arithmetic = (int32_t)value;
			return true;
		}
	}
	else if (type == rttr::type::get<int64_t>())
	{
		auto value = (int)arithmetic.get_value<int64_t>();
		if (ImGui::InputInt(name.data(), &value))
		{
			arithmetic = (int64_t)value;
			return true;
		}
	}
	else if (type == rttr::type::get<uint32_t>())
	{
		auto value = (int)arithmetic.get_value<uint32_t>();
		if (ImGui::InputInt(name.data(), &value))
		{
			arithmetic = (uint32_t)value;
			return true;
		}
	}
	else if (type == rttr::type::get<uint64_t>())
	{
		auto value = (int)arithmetic.get_value<uint64_t>();
		if (ImGui::InputInt(name.data(), &value))
		{
			arithmetic = (uint64_t)value;
			return true;
		}
	}
	else if (type == rttr::type::get<float>())
	{
		auto value = arithmetic.get_value<float>();
		if (ImGui::InputFloat(name.data(), &value))
		{
			arithmetic = value;
			return true;
		}
	}
	else if (type == rttr::type::get<double>())
	{
		auto value = arithmetic.get_value<double>();
		if (ImGui::InputDouble(name.data(), &value))
		{
			arithmetic = value;
			return true;
		}
	}

	return false;
}

bool Mrk::PluginPropertiesInspectUI::RecurSeqContainer(rttr::variant& array, std::string_view name)
{
	auto seq = array.create_sequential_view();

	bool modified = false;

	if (ImGui::TreeNode(name.data()))
	{
		for (size_t i = 0; i < seq.get_size(); ++i)
		{
			auto item = seq.get_value(i).extract_wrapped_value();

			if (RecurVariant(item, std::format("element {}", i)))
			{
				seq.set_value(i, item);
				modified = true;
			}
		}

		ImGui::TreePop();
	}

	return modified;
}

bool Mrk::PluginPropertiesInspectUI::RecurAssContainer(rttr::variant& array, std::string_view name)
{
	return false;
}

bool Mrk::PluginPropertiesInspectUI::RecurString(rttr::variant& str, std::string_view name)
{
	std::string value = Utility::GBKToUTF8(str.get_value<std::string>());
	char buffer[256];
	strncpy_s(buffer, value.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';

	if (ImGui::InputText(name.data(), buffer, sizeof(buffer)))
	{
		str = Utility::UFT8ToGBK(std::string(buffer));
		return true;
	}
	else if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILEPATH"))
		{
			if (payload->DataSize > 0)
			{
				strncpy_s(buffer, static_cast<const char*>(payload->Data), sizeof(buffer) - 1);
				buffer[sizeof(buffer) - 1] = '\0';
				str = buffer;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}

	return false;
}

bool Mrk::PluginPropertiesInspectUI::RecurVariant(rttr::variant& variant, std::string_view name)
{
	auto type = variant.get_type();

	if (type.is_valid())
	{
		if (IsSepcializedType(type))
		{
			return RecurSpecialized(variant, name);
		}
		if (type.is_arithmetic())
		{
			return RecurArithmetic(variant, name);
		}
		else if (type == rttr::type::get<std::string>())
		{
			return RecurString(variant, name);
		}
		else if (type.is_associative_container())
		{
			return RecurAssContainer(variant, name);
		}
		else if (type.is_sequential_container())
		{
			return RecurSeqContainer(variant, name);
		}
		else if (type.is_class())
		{
			return RecurObject(variant, name);
		}
		else
		{
			ImGui::Text("%s: [Unsupported Type]", name.data());
		}
	}

	return false;
}

bool Mrk::PluginPropertiesInspectUI::RecurSpecialized(rttr::variant& variant, std::string_view name)
{
	auto type = variant.get_type();

	if (type == rttr::type::get<Mrk::Vector2>())
	{
		auto value = variant.get_value<Vector2>();
		if (ImGui::InputFloat2(name.data(), (float*)&value))
		{
			variant = value;
			return true;
		}
	}
	else if (type == rttr::type::get<Mrk::Vector3>())
	{
		auto value = variant.get_value<Vector3>();
		if (ImGui::InputFloat3(name.data(), (float*)&value))
		{
			variant = value;
			return true;
		}
	}
	else if (type == rttr::type::get<Mrk::Vector4>())
	{
		auto value = variant.get_value<Vector4>();
		if (ImGui::InputFloat4(name.data(), (float*)&value))
		{
			variant = value;
			return true;
		}
	}
	else if (type == rttr::type::get<Mrk::Quaternion>())
	{
		auto value = variant.get_value<Quaternion>();
		if (ImGui::InputFloat4(name.data(), (float*)&value))
		{
			variant = value;
			return true;
		}
	}
	else if (type == rttr::type::get<Mrk::Vector2i>())
	{
		auto value = variant.get_value<Vector2i>();
		if (ImGui::InputInt2(name.data(), (int*)&value))
		{
			variant = value;
			return true;
		}
	}
	else if (type == rttr::type::get<Mrk::Vector3i>())
	{
		auto value = variant.get_value<Vector3i>();
		if (ImGui::InputInt3(name.data(), (int*)&value))
		{
			variant = value;
			return true;
		}
	}
	else if (type == rttr::type::get<Mrk::Vector4i>())
	{
		auto value = variant.get_value<Vector4i>();
		if (ImGui::InputInt4(name.data(), (int*)&value))
		{
			variant = value;
			return true;
		}
	}

	return false;
}

bool Mrk::PluginPropertiesInspectUI::IsSepcializedType(const rttr::type& type)
{
	return type == rttr::type::get<Vector2>()
		|| type == rttr::type::get<Vector3>()
		|| type == rttr::type::get<Vector4>()
		|| type == rttr::type::get<Quaternion>()
		|| type == rttr::type::get<Vector2i>()
		|| type == rttr::type::get<Vector3i>()
		|| type == rttr::type::get<Vector4i>();
}

bool Mrk::PluginPropertiesInspectUI::RecurProperties(rttr::instance obj)
{
	auto type = obj.get_derived_type();
	auto props = type.get_properties();

	bool modified = false;

	for (auto& prop : props)
	{
		auto propName = prop.get_name();
		auto propValue = prop.get_value(obj);

		if (RecurVariant(propValue, propName.data()))
		{
			if (propValue.is_valid())
			{
				try
				{
					prop.set_value(obj, propValue);
					modified = true;
				}
				catch (const std::exception& ex)
				{
					if (ImGui::BeginPopupModal("Setter Error"))
					{
						ImGui::Text(ex.what());
					}
				}
			}
		}
	}

	return modified;
}

bool  Mrk::PluginPropertiesInspectUI::RecurObject(rttr::instance obj, std::string_view name)
{
	auto type = obj.get_type();

	if (type.is_derived_from<Mrk::Component>())
	{
		if (ImGui::CollapsingHeader(name.data()))
		{
			return RecurProperties(obj);
		}
	}
	else
	{
		if (ImGui::TreeNode(name.data()))
		{
			auto res = RecurProperties(obj);
			ImGui::TreePop();
			return res;
		}
	}

	return false;
}

void Mrk::PluginMaterialEditUI::SetSelectedMaterialFile(std::string_view file)
{
	selectedMaterialFile = file;
}

void Mrk::PluginMaterialEditUI::Draw()
{
	ImGui::Begin("Material", nullptr, ImGuiWindowFlags_HorizontalScrollbar);

	std::filesystem::path matPath = selectedMaterialFile;
	if (std::filesystem::exists(matPath))
	{
		if (auto material = MaterialHut::GetMaterial(matPath.string()))
		{
			auto& uniforms = material->GetUniforms();
			for (auto& uniform : uniforms)
			{
				if (ImGui::TreeNode(uniform->GetName().c_str()))
				{
					auto props = uniform->get_type().get_properties();
					for (auto& prop : props)
					{
						auto propType = prop.get_type();
						auto propName = prop.get_name();
						auto propValue = prop.get_value(*uniform);

						if (propName == "name")
						{
							continue;
						}

						if (propType == rttr::type::get<float>())
						{
							auto value = propValue.get_value<float>();
							if (ImGui::InputFloat(propName.data(), &value))
							{
								prop.set_value(*uniform, value);
							}
						}
						else if (propType == rttr::type::get<std::string>())
						{
							auto value = Utility::GBKToUTF8(propValue.get_value<std::string>());
							char buffer[256];
							strncpy_s(buffer, value.c_str(), sizeof(buffer));
							buffer[sizeof(buffer) - 1] = '\0';

							if (ImGui::InputText(propName.data(), buffer, sizeof(buffer)))
							{
								value = Utility::UFT8ToGBK(std::string(buffer));
								prop.set_value(*uniform, value);
							}
							else if (ImGui::BeginDragDropTarget())
							{
								if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILEPATH"))
								{
									if (payload->DataSize > 0)
									{
										strncpy_s(buffer, static_cast<const char*>(payload->Data), sizeof(buffer) - 1);
										buffer[sizeof(buffer) - 1] = '\0';
										propValue = buffer;
										prop.set_value(*uniform, propValue);
									}
								}
								ImGui::EndDragDropTarget();
							}
						}
					}

					ImGui::TreePop();
				}
			}
		}
	}

	ImGui::End();
}

void Mrk::PluginImNodesTest::Draw()
{
	const int hardcoded_node_id = 1;

	ImGui::Begin("node editor");

	ImNodes::BeginNodeEditor();

	ImNodes::BeginNode(hardcoded_node_id);
	ImGui::Dummy(ImVec2(80.0f, 45.0f));
	ImNodes::EndNode();

	ImNodes::EndNodeEditor();

	ImGui::End();
}
