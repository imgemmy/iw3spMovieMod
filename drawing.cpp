#include "drawing.h"
#include "offsets.h"
#include "dollyCam.h"
#include <iostream>

Drawing Overlay;
D3DXVECTOR3 point = { 0,0,0 };

void Drawing::StaticOverlay()
{
	ImGui::Begin("BG-Overlay", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoBackground);
	{
		//Text helpers/binds
		auto draw = ImGui::GetBackgroundDrawList();
		draw->AddText(ImVec2(40, 10), ImColor(255, 255, 255), "Press Insert to open the menu.");
		draw->AddText(ImVec2(40, 50), ImColor(255, 255, 255), "- Controls -");
		draw->AddText(ImVec2(40, 70), ImColor(255, 255, 255), "Press N to activate noclip.");
		draw->AddText(ImVec2(40, 90), ImColor(255, 255, 255), "Press V to add a camera node.");
		draw->AddText(ImVec2(40, 110), ImColor(255, 255, 255), "Press F to load the first node.");
		draw->AddText(ImVec2(40, 130), ImColor(255, 255, 255), "Press R to clear all nodes.");
		draw->AddText(ImVec2(40, 150), ImColor(255, 255, 255), "Press Space to play the sequence.");
		draw->AddText(ImVec2(40, 170), ImColor(255, 255, 255), "Arrow Keys to adjust rotation.");

		//dolly camera drawings
		DollyCam.PlotLines();
		DollyCam.PlotPoints();
		

	}
	ImGui::End();

}

void Drawing::Menu()
{
	ImGui::SetNextWindowSize(ImVec2(300, 500));
	ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize);
	{
		//pos and angles
		ImGui::Text("Position:");
		ImGui::Text("X"); ImGui::SameLine(); ImGui::DragFloat("##pos_x", &P_Position()->Position.x, 1, -FLT_MAX - 1, FLT_MAX, "%.0f");
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::DragFloat("##pos_y", &P_Position()->Position.y, 1, -FLT_MAX - 1, FLT_MAX, "%.0f");
		ImGui::Text("Z"); ImGui::SameLine(); ImGui::DragFloat("##pos_z", &P_Position()->Position.z, 1, -FLT_MAX - 1, FLT_MAX, "%.0f");

		ImGui::Text("View Angles:");
		ImGui::Text("X"); ImGui::SameLine(); ImGui::DragFloat("##view_x", &P_Angles()->Angles.x, 1, -85, 85, "%.0f");
		ImGui::Text("Y"); ImGui::SameLine(); ImGui::DragFloat("##view_y", &P_Angles()->Angles.y, 1, 0, 360, "%.0f");
		ImGui::Text("Z"); ImGui::SameLine(); ImGui::DragFloat("##view_z", &P_Angles()->Angles.z, 1, -180, 180, "%.0f");

		//Change speed of cam
		//bump up speed cause lol slow
		ImGui::DragFloat("Camera Speed", &DollyCam.speed, 1.f, 1.f, 200.f, "%0.2f");
		ImGui::DragInt("Camera Steps", &DollyCam.numSteps, 10, 1000, 100000, "%d");

	}
	ImGui::End();
}

bool Drawing::WorldToScreen(const Vector3 &vecOrigin, Vector3 &vecScreen, float* pflViewMatrix)//vecOrigin is the 3d target, vecScreen is 2d conversion, viewmatrix is obviously the matrix array (4x4)
{
	// Read the matrix values from the memory address
	memcpy(this->matrix, (void*)(P_Screen.ViewMatrix), sizeof(matrix));

	vecScreen[0] = pflViewMatrix[0] * vecOrigin[0] + pflViewMatrix[1] * vecOrigin[1] + pflViewMatrix[2] * vecOrigin[2] + pflViewMatrix[3];
	vecScreen[1] = pflViewMatrix[4] * vecOrigin[0] + pflViewMatrix[5] * vecOrigin[1] + pflViewMatrix[6] * vecOrigin[2] + pflViewMatrix[7];

	auto flTemp = pflViewMatrix[12] * vecOrigin[0] + pflViewMatrix[13] * vecOrigin[1] + pflViewMatrix[14] * vecOrigin[2] + pflViewMatrix[15];

	if (flTemp < 0.01f)
		return false;

	auto invFlTemp = 1.f / flTemp;
	vecScreen[0] *= invFlTemp;
	vecScreen[1] *= invFlTemp;

	static float iResolution[2] = { 0 };
	if (!iResolution[0] || !iResolution[1])
	{
		iResolution[0] = *(float*)P_Screen.ScreenX;
		iResolution[1] = *(float*)P_Screen.ScreenY;
	}

	auto x = iResolution[0] / 2.f;
	auto y = iResolution[1] / 2.f;

	x += 0.5f * vecScreen[0] * (float)iResolution[0] + 0.5f;
	y -= 0.5f * vecScreen[1] * (float)iResolution[1] + 0.5f;

	vecScreen[0] = x;
	vecScreen[1] = y;

	return true;
}

