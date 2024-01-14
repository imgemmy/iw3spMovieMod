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
	//reset vector on keypress 'r'
	if (GetAsyncKeyState(0x52) & 1)
	{
		camVector.clear();
		camVectorSmoothSpline.clear();
	}
}

void DollyCamera::Play()
{
	float deltaTime = 1.0f / (*(int*)P_Dvar.frameTime);
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

		camVectorSmoothSpline = this->GenerateBezierCurve(camVector, 20);
		printf("regularSize: %d | splineSize %d\n", camVector.size(), camVectorSmoothSpline.size());

		shouldPlay = false;
	}

}

//Drawing funcs
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

	Vector3 ScreenSpline;
	Vector3 ScreenSpline2;
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

	if (camVectorSmoothSpline.size() > 3) //needs 4 points to smooth
	{
		for (int i = 0; i < camVectorSmoothSpline.size() - 1; i++)
		{
			if (Overlay.WorldToScreen(camVectorSmoothSpline[i][0], ScreenSpline, Overlay.matrix) && Overlay.WorldToScreen(camVectorSmoothSpline[i + 1][0], ScreenSpline2, Overlay.matrix))
			{
				draw->AddLine(ImVec2(ScreenSpline.x, ScreenSpline.y), ImVec2(ScreenSpline2.x, ScreenSpline2.y), ImColor(255, 37, 81, 255), 8);
			}
		}
	}
}
// Cubic Bezier Interpolation Function
Vector3 DollyCamera::CubicBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;
	float uuu = uu * u;
	float ttt = tt * t;

	Vector3 p = p0 * uuu; // first term
	p = p + (p1 * 3 * uu * t); // second term
	p = p + (p2 * 3 * u * tt); // third term
	p = p + (p3 * ttt); // fourth term

	return p;
}

// Bezier Curve Generation Function
std::vector<std::vector<Vector3>> DollyCamera::GenerateBezierCurve(const std::vector<std::vector<Vector3>>& controlPoints, int newPoints) {
	std::vector<std::vector<Vector3>> bezierPoints;

	// Check if there are enough points for the first Bezier curve segment
	if (controlPoints.size() < 4) {
		return bezierPoints;
	}

	// Process each set of four points
	for (size_t i = 0; i < controlPoints.size() - 1; i += 3) {
		// Handle the case where there are not enough points to form a complete set of four
		Vector3 p1 = (i + 1 < controlPoints.size()) ? controlPoints[i + 1][0] : controlPoints[i][0];
		Vector3 p2 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2][0] : p1;
		Vector3 p3 = (i + 3 < controlPoints.size()) ? controlPoints[i + 3][0] : p2;

		// Generate Bezier curve for this segment
		for (int j = 0; j <= newPoints; ++j) {
			float t = static_cast<float>(j) / newPoints;
			bezierPoints.push_back({ this->CubicBezier(controlPoints[i][0], p1, p2, p3, t), camRotation });
		}
	}

	return bezierPoints;
}


