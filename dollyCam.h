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
	void PlotPoints();
	void PlotLines();

	//bezier
	Vector3 CalculateBezierPoint(float t, const std::vector<Vector3>& points);
	std::vector<Vector3> GenerateBezierCurve(const std::vector<Vector3>& controlPoints, int numSteps);

	Vector3 camPosition;
	Vector3 camRotation;
	std::vector<std::vector<Vector3>> camVector;
	int numSteps = 10;
};

extern DollyCamera DollyCam;