#include "PluginMessageBoxDlg.h"

void Mrk::PluginMessageBoxDlg::Pop(std::string_view message)
{
	if (!messages.contains(message.data()))
	{
		messages.emplace(message.data());
	}
}

void Mrk::PluginMessageBoxDlg::Draw()
{
	if (!messages.empty())
	{
		auto& message = *messages.begin();

		ImGui::OpenPopup(message.c_str());

		if (ImGui::BeginPopupModal(message.c_str()))
		{
			ImGui::Text(message.c_str());

			if (ImGui::Button("OK"))
			{
				messages.erase(message);
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
