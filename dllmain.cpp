// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "src/CameraPatch.h"
#include "src/MemoryMgr.h"


extern "C"
{
	__declspec(dllexport) void InitializeASI()
	{
		Memory::VP::InjectHook(0x513DEA, InterpolateAimAngle);
		Memory::VP::InjectHook(0x5B0510, GetGlobalCameraPosFromPlayer);
		Memory::VP::InjectHook(0x5455E6, ProcessCrosshair);
	}
}

