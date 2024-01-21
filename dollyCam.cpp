#include <Windows.h>
#include <cmath>
#include <algorithm>
#include <thread>

#include "dollyCam.h"
#include "offsets.h"
#include "drawing.h"

#include <string>
#include <map>

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
	if (GetAsyncKeyState(0x46) & 1 && this->camVector.size() > 0)
	{
		P_Position()->Position = this->camVector[0][0];
		P_Angles()->Angles = this->camVector[0][1];
	}
}

void DollyCamera::SavePosition()
{
	//load first position on keypress 'v'
	if (GetAsyncKeyState(0x56) & 1)
	{
		if (this->camVector.size() < 4) //only 4 nodes should be present
		{
			this->camPosition.x = P_Position()->Position.x;
			this->camPosition.y = P_Position()->Position.y;
			this->camPosition.z = P_Position()->Position.z;

			this->camRotation.x = P_Angles()->Angles.x;
			this->camRotation.y = P_Angles()->Angles.y;
			this->camRotation.z = P_Angles()->Angles.z;

			this->camVector.push_back({ camPosition, camRotation });
		}

	}
}

void DollyCamera::Reset()
{
	//reset vector on keypress 'r'
	if (GetAsyncKeyState(0x52) & 1)
	{
		this->camVector.clear();
		this->camVectorSmoothSpline.clear();
		this->tick = 0;
		this->shouldPlay = false;
	}
}

void DollyCamera::Play()
{
	float deltaTime = 1.0f / (*(int*)P_Dvar.frameTime);
	
	//play on keypress 'space'
	if (GetAsyncKeyState(VK_SPACE) & 1)
	{
		if (this->tick < 1) //make sure dont play while playing
		{
			if (this->camVector.size() > 3) //avoid crash
			{
				this->shouldPlay = true;
				this->camVectorSmoothSpline = this->GenerateBezierCurve(this->camVector, this->numSteps);
				this->BuildFlightPathVizualiser();
			}
		}
		else if (this->tick < this->camVectorSmoothSpline.size() - 1) //stoping replay whilst its already playing
		{
			this->tick = 0;
			this->shouldPlay = false;
		}

	}

	if (this->shouldPlay && this->camVector.size() > 3)
	{
		if (this->tick >= this->camVectorSmoothSpline.size() - 1) //counter/camera done
		{
			this->tick = 0;
			this->shouldPlay = false;
		}

		else if (this->tick < camVectorSmoothSpline.size() - 1)
		{
			this->UpdateCameraPosition(deltaTime, this->speed);	
		}		
	}
}

//Drawing funcs
void DollyCamera::PlotPoints()
{
	auto draw = ImGui::GetBackgroundDrawList();
	Vector3 Screen;
	if (this->camVector.size() > 0)
	{
		for (int i = 0; i < this->camVector.size(); i++)
		{
			if (Overlay.WorldToScreen(this->camVector[i][0], Screen, Overlay.matrix))
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

	if (this->camVector.size() > 1) //draw 
	{
		for (int i = 0; i < this->camVector.size() - 1; i++)
		{
			if (Overlay.WorldToScreen(this->camVector[i][0], Screen, Overlay.matrix) && Overlay.WorldToScreen(this->camVector[i + 1][0], Screen2, Overlay.matrix))
			{
				draw->AddLine(ImVec2(Screen.x, Screen.y), ImVec2(Screen2.x, Screen2.y), ImColor(37, 255, 227, 255), 3);
			}
		}
	}

	if (this->camVectorSmoothSpline.size() > 3) //needs 4 points to smooth
	{
		for (int i = 0; i < this->visualizePathNodes.size() - 1; i++)
		{
			if (Overlay.WorldToScreen(this->visualizePathNodes[i], ScreenSpline, Overlay.matrix) && Overlay.WorldToScreen(this->visualizePathNodes[i + 1], ScreenSpline2, Overlay.matrix))
			{
				draw->AddLine(ImVec2(ScreenSpline.x, ScreenSpline.y), ImVec2(ScreenSpline2.x, ScreenSpline2.y), ImColor(255, 37, 81, 255), 8);
			}
		}
	}
}

// Cubic Bezier Interpolation Function
Vector3 DollyCamera::CubicBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) 
{
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

std::vector<std::vector<Vector3>> DollyCamera::GenerateBezierCurve(const std::vector<std::vector<Vector3>>& controlPoints, int newPoints) 
{
	std::vector<std::vector<Vector3>> bezierPoints;

	if (controlPoints.size() < 4) {
		return bezierPoints;
	}

	for (size_t i = 0; i < controlPoints.size() - 1; i += 3) {
		// Position control points
		Vector3 p1 = (i + 1 < controlPoints.size()) ? controlPoints[i + 1][0] : controlPoints[i][0];
		Vector3 p2 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2][0] : p1;
		Vector3 p3 = (i + 3 < controlPoints.size()) ? controlPoints[i + 3][0] : p2;

		// Rotation control points
		Vector3 r1 = (i + 1 < controlPoints.size()) ? controlPoints[i + 1][1] : controlPoints[i][1];
		Vector3 r2 = (i + 2 < controlPoints.size()) ? controlPoints[i + 2][1] : r1;
		Vector3 r3 = (i + 3 < controlPoints.size()) ? controlPoints[i + 3][1] : r2;

		for (int j = 0; j <= newPoints; ++j) {
			float t = static_cast<float>(j) / newPoints;
			Vector3 position = this->CubicBezier(controlPoints[i][0], p1, p2, p3, t);
			Vector3 rotation = this->CubicBezier(controlPoints[i][1], r1, r2, r3, t);

			bezierPoints.push_back({ position, rotation });
		}
	}

	return bezierPoints;
}


void DollyCamera::UpdateCameraPosition(float deltaTime, float &speed) 
{
	static float t = 0.0f;
	t += deltaTime * speed;

	if (t > 1.f)
	{
		this->tick++;
		P_Position()->Position = this->camVectorSmoothSpline[this->tick][0];
		P_Angles()->Angles = this->camVectorSmoothSpline[this->tick][1];
		t = 0.f;
	}
}

//Play with no vector but interpolate live
//void DollyCamera::UpdateCameraPosition(float deltaTime, float& speed)
//{
//	static float t = 0.0f;
//
//	// Adjust the rate of increment of 't'. The divisor here controls the overall speed.
//	t += deltaTime * speed / 100000.0f; // Adjust the divisor as needed for speed scaling
//
//	// Ensure 't' stays in the [0, 1] range for looping, or clamp for non-looping
//	if (t > 1.0f) {
//		t = fmod(t, 1.0f); // Use this for looping
//		// t = 1.0f; // Use this instead for non-looping
//	}
//
//	// Assuming controlPoints is a vector of Vector3, where each group of four points forms a Bezier segment
//	std::vector<std::vector<Vector3>> controlPoints = this->camVector; // Replace with actual function to get control points
//
//	// Check if there are enough points for at least one Bezier segment
//	if (controlPoints.size() >= 4) {
//		// Calculate which segment of the curve 't' is currently in
//		int segment = std::min<int>(static_cast<int>(t * (controlPoints.size() / 4)), static_cast<int>(controlPoints.size() / 4) - 1);
//
//		// Calculate local t for the current segment
//		float localT = (t * (controlPoints.size() / 4)) - segment;
//
//		// Calculate indices for control points of the current segment
//		int idx = segment * 4;
//
//		Vector3 position = this->CubicBezier(controlPoints[idx][0], controlPoints[idx + 1][0], controlPoints[idx + 2][0], controlPoints[idx + 3][0], localT);
//		Vector3 rotation = this->CubicBezier(controlPoints[idx][1], controlPoints[idx + 1][1], controlPoints[idx + 2][1], controlPoints[idx + 3][1], localT);
//		// Assuming a similar approach for rotation if needed
//
//		P_Position()->Position = position;
//		P_Angles()->Angles = rotation;
//		// Update rotation similarly
//	}
//}

float DollyCamera::DistanceBetweenPoints(Vector3& vec1, Vector3& vec2)
{
	float distance = sqrt(pow(vec1.x - vec2.x, 2.0) + pow(vec1.y - vec2.y, 2.0) + pow(vec1.z - vec2.z, 2.0));
	return distance;
}

void DollyCamera::BuildFlightPathVizualiser()
{
	this->visualizePathNodes.clear();

	if (this->camVectorSmoothSpline.size() > 4)
	{
		//bezier curves
		for (int i = 1, k = 0; i < this->camVectorSmoothSpline.size() - 1; i++)
		{
			//Line from each frame to another
			Vector3 iOrigin = this->camVectorSmoothSpline[i][0];
			Vector3 kOrigin = this->camVectorSmoothSpline[k][0];

			if (this->DistanceBetweenPoints(iOrigin, kOrigin) > 10.f)
			{
				this->visualizePathNodes.push_back(kOrigin);
				k = i;
			}
			else if (i == this->camVectorSmoothSpline.size() - 2)
				this->visualizePathNodes.push_back(iOrigin);
		}
	}

}

