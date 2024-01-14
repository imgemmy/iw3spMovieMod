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

	//spline smoothing
	Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
	std::vector<std::vector<Vector3>> cubicSpline(const std::vector<std::vector<Vector3>>& points, int newPoints);

	Vector3 CubicBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
	std::vector<std::vector<Vector3>> GenerateBezierCurve(const std::vector<std::vector<Vector3>>& controlPoints, int newPoints);

	//vars
	Vector3 camPosition;
	Vector3 camRotation;
	std::vector<std::vector<Vector3>> camVector;
	std::vector<std::vector<Vector3>> camVectorSmoothSpline;
	int numSteps = 10;
};

extern DollyCamera DollyCam;