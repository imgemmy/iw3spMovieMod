#pragma once
#include "imgui/imgui.h"
#include "mathHelpers.h"
#include <d3dx9.h>
#include "offsets.h"

struct Drawing
{
	void StaticOverlay();
	void Menu();

	//W2S stuff
	bool WorldToScreen(const Vector3& vecOrigin, Vector3& vecScreen, float* pflViewMatrix);
	float matrix[16];
};
extern Drawing Overlay;