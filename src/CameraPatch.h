#pragma once

#include <iostream>

class CVector
{
public:
	float x;
	float y;
	float z;
	float w;
	CVector() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {};
	CVector(float x, float y, float z) :x(x), y(y), z(z), w(0.0f) {};
};

class CVector2d
{
public:
	float x;
	float y;
	CVector2d() : x(0.0f), y(0.0f) {};
	
	float Magnitude() { return sqrtf(x * x + y * y); };
};

class CMatrix
{
public:
	CVector right;
	CVector up;
	CVector at;
	CVector pos;
	CMatrix() { pos.w = 1.0f; };
};

struct RslLLLink
{
	RslLLLink* next;
	RslLLLink* prev;
};

struct RslLinkList
{
	RslLLLink link;
};

struct RslNode
{
	RslNode* nextInstance;
	RslNode* next;
	RslNode* parent;
	RslNode* root;
	RslNode* child;
	void*	animTree;
	char	name[32];
	bool	changed;
	int		padding;
	CMatrix ltm;
	CMatrix modelling;
};

struct RslElementGroup
{
	RslNode*		parentNode;
	void*			parentWorld;
	void*			a;
	void*			b;
	void*			animTree;
	RslLinkList		elementsInList;
};


CVector* __fastcall GetGlobalCameraPosFromPlayer(void* This, void* edx, CVector* camPos, CVector* plrPos, CMatrix* camRot, CMatrix* plrRot, CVector* gCamPosFromPlr);
void __cdecl InterpolateAimAngle(float* curAngle, float* angleDelta, float targetAngle, float speed, float minDelta);
void	ProcessCrosshair();