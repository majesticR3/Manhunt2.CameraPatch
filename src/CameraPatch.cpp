#include "pch.h"
#include "CameraPatch.h"

typedef unsigned int uint;

enum eWeaponType
{
	WEAPONTYPE_NONE = 0,
	WEAPONTYPE_2HFIREARM,
	WEAPONTYPE_1HFIREARM,
	WEAPONTYPE_MELEE,
	WEAPONTYPE_LURE
};

enum ePedState
{
	PEDSTATE_NORMAL			= 3,
	PEDSTATE_CRAWL			= 5,
	PEDSTATE_AIM			= 8,
	PEDSTATE_BLOCK			= 9,
	PEDSTATE_WALLSQASH		= 10,
	PEDSTATE_CROUCH			= 14,
	PEDSTATE_DRAGDEADBODY	= 18,
	PEDSTATE_CLIMB			= 20
};


void*	TheCamera				= (void*)0x76F240;
CVector* CurrentCamLookAtOffsetFromPlayer = (CVector*)((char*)TheCamera + 1536);
CVector* CurrentCamOffsetFromPlayer = (CVector*)((char*)TheCamera + 1520);
CVector* CurrentRevCamOffsetFromPlayer = (CVector*)((char*)TheCamera + 1504);

/* Camera */
float CamMinYAngle = -60.0f;
float CamMaxYAngle = 60.0f;
float CamCurrYAngle = 18.0f;
bool  CamInSyncWithPlrYAngle = false;

CVector* __fastcall GetGlobalCameraPosFromPlayer(void* This, void* edx, CVector* camPos, CVector* plrPos, CMatrix* camRot, CMatrix* plrRot, CVector* gCamPosFromPlr)
{	
	*(float*)0x6CE0DC = 0.0f;
	*(float*)0x6CE0E0 = 0.0f;

	float* PlrCurrYAngle		= (float*)((char*)This + 4400);
	bool*  PlrActionLookBack	= (bool*)((char*)This + 1232);
	bool*  PlrActionLockOn		= (bool*)((char*)This + 1352);
	int*   PlrCurrState			= (int*)((char*)This + 956);
	int*   PlrCurrWpnType		= (int*)((char*)This + 968);
	int*   PlrLockOnHunter		= (int*)((char*)This + 4500);
	CVector2d* RightStickMoveDefault = (CVector2d*)((char*)This + 1184);

	if (fabs(RightStickMoveDefault->y))
	{
		CamCurrYAngle += RightStickMoveDefault->y * *(float*)0x6ECE68 * 30.0f;
		if (CamCurrYAngle >= CamMaxYAngle)
			CamCurrYAngle = CamMaxYAngle;
		else
			if (CamCurrYAngle <= CamMinYAngle)
				CamCurrYAngle = CamMinYAngle;
	}

	CVector CamPosOffsetFromPlayer;
	float AxisYAngle = CamCurrYAngle;

	((CVector * (__thiscall*)(void*, CVector*, CMatrix*, bool))0x5AF950)(This, plrPos, plrRot, false);

	if (!*(bool*)0x789819 && *PlrActionLookBack)
	{
		*(int*)0x76F244 = 0;
		*(int*)0x76F24C = 0;

		if (*(bool*)0x76F4FD && *(bool*)0x76F4FE)
		{
			CamPosOffsetFromPlayer = *CurrentRevCamOffsetFromPlayer;
			AxisYAngle = -AxisYAngle;
		}
		else
		{
			*(bool*)0x76F4FD = true;
			CamPosOffsetFromPlayer = *CurrentCamOffsetFromPlayer;
		}
	}
	else
		if (*PlrCurrState == PEDSTATE_CRAWL || ((bool(__cdecl*)(void*))0x515260)(This))
		{
			RslElementGroup* elementGroup = (RslElementGroup*)((char*)This + 184);
			CamPosOffsetFromPlayer.x = 0.0f;
			CamPosOffsetFromPlayer.y = fabs(sin(elementGroup->parentNode->ltm.pos.x * 4.0f + elementGroup->parentNode->ltm.pos.z) * 0.05f);
			CamPosOffsetFromPlayer.z = -0.001f;
		}
		else
			CamPosOffsetFromPlayer = *CurrentCamOffsetFromPlayer;

	if ((*PlrCurrState == PEDSTATE_AIM && *PlrCurrWpnType == WEAPONTYPE_LURE) || *(bool*)0x75B348 || *PlrCurrState == PEDSTATE_CRAWL || *PlrCurrState == PEDSTATE_CLIMB || (*PlrLockOnHunter && *PlrActionLockOn) || *(bool*)0x76F860)
	{
		if (CamInSyncWithPlrYAngle || *PlrCurrState == PEDSTATE_CLIMB || *PlrCurrState == PEDSTATE_CRAWL || (*PlrLockOnHunter && *PlrActionLockOn) || *(bool*)0x76F860)
		{
			CamCurrYAngle = *PlrCurrYAngle;
			AxisYAngle = CamCurrYAngle;
		}
		else
		{
			CamInSyncWithPlrYAngle = true;
			*PlrCurrYAngle = CamCurrYAngle;
		}
	}
	else
	{
		CamInSyncWithPlrYAngle = false;
		CurrentCamLookAtOffsetFromPlayer->x = 0.0f;
		CurrentCamLookAtOffsetFromPlayer->y = 0.0f;
		CurrentCamLookAtOffsetFromPlayer->z = 0.0f;
	}

	CMatrix CamYRotation;
	((CMatrix * (__thiscall*)(CMatrix*, CVector*, float))0x57E440)(&CamYRotation, (CVector*)0x6B3218, AxisYAngle);
	((CVector * (__thiscall*)(CVector*, CVector*, CMatrix*))0x580390)(&CamPosOffsetFromPlayer, &CamPosOffsetFromPlayer, &CamYRotation);
	((CVector * (__thiscall*)(CVector*, CVector*, CMatrix*))0x580390)(&CamPosOffsetFromPlayer, &CamPosOffsetFromPlayer, plrRot);
	((CVector * (__cdecl*)(CVector*, CVector*, CVector*))0x60D800)(gCamPosFromPlr, plrPos, &CamPosOffsetFromPlayer);

	return camPos;
}

void __cdecl InterpolateAimAngle(float* curAngle, float* angleDelta, float targetAngle, float speed, float minDelta)
{
	*curAngle = targetAngle;
}

void ProcessCrosshair()
{
	*(char*)0x79D0E4 = 0;
	*(float*)0x79D0E8 = *(float*)0x76DEE0;
	*(float*)0x79D0EC = *(float*)0x75B124;

	if (*(int*)0x76DECC)
	{
		void* PlayerInst			= *(void**)0x789490;
		float fDefaultSpriteScale	= 0.05f;
		float fLineLength			= 0.2f * fDefaultSpriteScale;
		float fDefaultSpread		= 0.1f * fDefaultSpriteScale;
		float fMaxSpread			= 0.5f * fDefaultSpriteScale;
		float fLineOffset			= 0.01f * fDefaultSpriteScale;

		if (PlayerInst && ((bool(__thiscall*)(void*))0x599290)(PlayerInst))
		{
			CVector2d* RightStickMoveDefault = (CVector2d*)((char*)PlayerInst + 1184);
			CVector2d* LeftStickMoveDefault = (CVector2d*)((char*)PlayerInst + 1168);
			float fSpreadAimMove = RightStickMoveDefault->Magnitude() * 0.005f + LeftStickMoveDefault->Magnitude()* 0.01f + fDefaultSpread;
			float fSpread = fSpreadAimMove <= fMaxSpread ? fSpreadAimMove : fMaxSpread;

			((int(__cdecl*)(float, float, float, float, int, int, int, int, int))0x60AF80)(-fLineOffset, fSpread, -fLineOffset, fSpread + fLineLength, 160, 160, 160, 255, 1);
			((int(__cdecl*)(float, float, float, float, int, int, int, int, int))0x60AF80)(fLineOffset, fSpread, fLineOffset, fSpread + fLineLength, 160, 160, 160, 255, 1);
			
			((int(__cdecl*)(float, float, float, float, int, int, int, int, int))0x60AF80)(-fLineOffset, -fSpread, -fLineOffset, -fSpread - fLineLength, 160, 160, 160, 255, 1);
			((int(__cdecl*)(float, float, float, float, int, int, int, int, int))0x60AF80)(fLineOffset, -fSpread, fLineOffset, -fSpread - fLineLength, 160, 160, 160, 255, 1);

			((int(__cdecl*)(float, float, float, float, int, int, int, int, int))0x60AF80)(-fSpread, -fLineOffset, -fSpread - fLineLength, -fLineOffset, 160, 160, 160, 255, 1);
			((int(__cdecl*)(float, float, float, float, int, int, int, int, int))0x60AF80)(-fSpread, fLineOffset, -fSpread - fLineLength, fLineOffset, 160, 160, 160, 255, 1);

			((int(__cdecl*)(float, float, float, float, int, int, int, int, int))0x60AF80)(fSpread, -fLineOffset, fSpread + fLineLength, -fLineOffset, 160, 160, 160, 255, 1);
			((int(__cdecl*)(float, float, float, float, int, int, int, int, int))0x60AF80)(fSpread, fLineOffset, fSpread + fLineLength, fLineOffset, 160, 160, 160, 255, 1);
		}
	}

	*(float*)0x79D0EC	= 0.0f;
	*(char*)0x79D0E4	= 0;
	*(float*)0x79D0E8	= 0.0f;
}

