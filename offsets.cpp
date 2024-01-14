#include "offsets.h"
#include "mathHelpers.h"

ViewAngles* P_Angles()
{
	return (ViewAngles*)0x00868728;
}

Position* P_Position()
{
	return (Position*)0x00e0da1c;
}

Dvars P_Dvar;
Screen P_Screen;