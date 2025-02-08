#pragma once

#include "System/Common/Controller.hpp"


class CPCPhysicalController
{
public:
	static int32 PHYSICALCONTROLLER_MAX;

public:
	static bool Initialize(void);
	static void Terminate(void);
	static IPhysicalController* Open(int32 iController);
	static void MapDigital(uint32 btn, int32 iDIKey);
	static void MapDigitalFixed(uint32 btn, int32 iDIKey);
	static void MapAnalog(CController::ANALOG analog, int32 iDIKeyX, int32 iDIKeyY);
    static bool IsKeyDown(int32 iDIKey);
    static bool IsKeyTrigger(int32 iDIKey);
    static bool IsKeyNotFixed(int32 iDIKey);
	static int32 GetDownKey(void);
	static int32 GetPort(void);
};