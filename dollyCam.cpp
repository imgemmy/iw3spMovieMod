#include <Windows.h>
#include <cmath>
#include <algorithm>
#include <thread>


#include "dollyCam.h"
#include "offsets.h"
#include "drawing.h"

DollyCamera DollyCam;

void DollyCamera::Run()
{
	this->LoadPosition();
	this->SavePosition();
	this->Reset();
	this->Play();
}

void DollyCamera::LoadPosition()
{
	//load first position on keypress 'f'
	if (GetAsyncKeyState(0x46) & 1 && camVector.size() > 0)
	{
		P_Position()->Position = camVector[0][0];
		P_Angles()->Angles = camVector[0][1];
	}
}

void DollyCamera::SavePosition()
{
	//load first position on keypress 'v'
	if (GetAsyncKeyState(0x56) & 1)
	{
		camPosition.x = P_Position()->Position.x;
		camPosition.y = P_Position()->Position.y;
		camPosition.z = P_Position()->Position.z;

		camRotation.x = P_Angles()->Angles.x;
		camRotation.y = P_Angles()->Angles.y;
		camRotation.z = P_Angles()->Angles.z;

		camVector.push_back({ camPosition, camRotation });
	}
}

void DollyCamera::Reset()
{
	//reset vector on keypress 'c'
	if (GetAsyncKeyState(0x43) & 1)
	{
		camVector.clear();
	}
}


void DollyCamera::Play()
{
	float deltaTime = 1.0f / (*(int*)P_Dvar.frameTime);
	printf("ran: %f\n", deltaTime);
	static bool shouldPlay = false;

	//play on keypress 'space'
	if (GetAsyncKeyState(VK_SPACE) & 1)
	{
		shouldPlay = true;
	}
	
	if (shouldPlay && camVector.size() > 1)
	{
		Vector3 start = camVector[0][0];
		Vector3 end = camVector[1][0];



		shouldPlay = false;
	}

}

void DollyCamera::PlotPoints()
{
	auto draw = ImGui::GetBackgroundDrawList();
	Vector3 Screen;
	if (camVector.size() > 0)
	{
		for (int i = 0; i < camVector.size(); i++)
		{
			if (Overlay.WorldToScreen(camVector[i][0], Screen, Overlay.matrix))
			{
				draw->AddCircleFilled(ImVec2(Screen.x, Screen.y), 10, ImColor(0, 0, 0, 255), 10);
				draw->AddCircleFilled(ImVec2(Screen.x, Screen.y), 8, ImColor(255, 255, 255, 255), 10);
				draw->AddCircleFilled(ImVec2(Screen.x, Screen.y), 4, ImColor(0, 0, 0, 255), 10);
			}
		}
	}

}

void DollyCamera::PlotLines()
{
	auto draw = ImGui::GetBackgroundDrawList();
	Vector3 Screen;
	Vector3 Screen2;
	if (camVector.size() > 1)
	{
		for (int i = 0; i < camVector.size() - 1; i++)
		{
			if (Overlay.WorldToScreen(camVector[i][0], Screen, Overlay.matrix) && Overlay.WorldToScreen(camVector[i + 1][0], Screen2, Overlay.matrix))
			{
				draw->AddLine(ImVec2(Screen.x, Screen.y), ImVec2(Screen2.x, Screen2.y), ImColor(37, 255, 227, 255), 3);
			}
		}
	}

}

