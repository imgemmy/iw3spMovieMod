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

	//Spline/Bezier stuff
	Vector3 CubicBezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
	std::vector<std::vector<Vector3>> GenerateBezierCurve(const std::vector<std::vector<Vector3>>& controlPoints, int newPoints);
	void BuildFlightPathVizualiser();

	//Camera Movement
	void UpdateCameraPosition(float deltaTime, float& speed);

	//Maths
	float DistanceBetweenPoints(Vector3& vec1, Vector3& vec2);


	//vars
	Vector3 camPosition;
	Vector3 camRotation;
	std::vector<std::vector<Vector3>> camVector;
	std::vector<std::vector<Vector3>> camVectorSmoothSpline;
	std::vector<Vector3> visualizePathNodes;
	float playback = 0.f; //note this is clamped 0 - 1
	bool shouldPlay = false;

	//changable vars
	int numSteps = 20000;
	float speed = 50.f;
};

extern DollyCamera DollyCam;