#include "PluginInputDlg.h"

void Mrk::PluginInputDlg::Draw()
{
	if (intCallback)
	{
		ImGui::OpenPopup("Input Int");
	}
	
	if (floatCallback)
	{
		ImGui::OpenPopup("Input Float");
	}

	if (stringCallback)
	{
		ImGui::OpenPopup("Input String");
	}

	if (ImGui::BeginPopupModal("Input Int"))
	{
		static int val = 0;

		(void)ImGui::InputInt("Input Int", &val);
		if (ImGui::Button("OK") && intCallback)
		{
			intCallback(val);
			intCallback = nullptr;
			val = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Input float"))
	{
		static float val = 0;

		(void)ImGui::InputFloat("Input Float", &val);
		if (ImGui::Button("OK") && floatCallback)
		{
			floatCallback(val);
			floatCallback = nullptr;
			val = 0;
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Input String"))
	{
		static char buffer[256] = { 0 };

		(void)ImGui::InputText((char*)u8"Input String", buffer, sizeof(buffer));

		if (ImGui::Button("OK") && stringCallback)
		{
			buffer[sizeof(buffer) - 1] = '\0';
			stringCallback(std::string(buffer));
			stringCallback = nullptr;
			std::memset(buffer, 0, sizeof(buffer));
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();

		if (ImGui::Button("Cancel"))
		{
			stringCallback = nullptr;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void Mrk::PluginInputDlg::GetInt(std::function<void(int)> callback)
{
	intCallback = callback;
}

void Mrk::PluginInputDlg::GetFloat(std::function<void(float)> callback)
{
	floatCallback = callback;
}

void Mrk::PluginInputDlg::GetString(std::function<void(const std::string&)> callback)
{
	stringCallback = callback;
}