#pragma once
#include <vector>

#include "mathHelpers.h"

struct DollyCamera
{
	void Run();
	void LoadPosition();
	void SavePosition();
	void Reset();
	void Play();

	//Drawing
	void PlotPoints();
	void PlotLines();

	//Spline Smoothing
	Vector3 CubicBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
	std::vector<std::vector<Vector3>> GenerateBezierCurve(const std::vector<std::vector<Vector3>>& controlPoints, int newPoints);

	//Camera Movement
	void UpdateCameraPosition(float deltaTime, float& speed);


	//vars
	Vector3 camPosition;
	Vector3 camRotation;
	std::vector<std::vector<Vector3>> camVector;
	std::vector<std::vector<Vector3>> camVectorSmoothSpline;
	int tick = 0;
	bool shouldPlay = false;

	//changable vars
	int numSteps = 2000;
	float speed = 50.f;
};

extern DollyCamera DollyCam;