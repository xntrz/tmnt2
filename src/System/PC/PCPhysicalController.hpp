#pragma once


class IPhysicalController;


class CPCPhysicalController
{
public:
	static int32 PHYSICALCONTROLLER_MAX;

public:
	static bool Initialize(void* hWnd);
	static void Terminate(void);
	static IPhysicalController* New(int32 iController);
	static void MapDigital(uint32 btn, int32 iDIKey);
	static void MapDigitalFixed(uint32 btn, int32 iDIKey);
	static void MapAnalog(int32 no, int32 iDIKeyX, int32 iDIKeyY);
	static bool IsKeyDown(int32 iDIKey);
	static bool IsKeyValid(int32 iDIKey);
	static bool IsKeyNotFixed(int32 iDIKey);
	static int32 GetDownKey(void);
	static int32 GetPort(void);
	static const char* GetKeyName(int32 iDIKey);
};