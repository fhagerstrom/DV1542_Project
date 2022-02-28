#pragma once
#include <iostream>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <stdexcept>
#include <assert.h>
#include <vector>
#include <Windows.h>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <string>
#include "Image.h"

using namespace DirectX;

// For aligning 16 bits
#define ALIGN16 _declspec(align(16))

// Custom fail check define for throwing messageboxes at what the problem is.
#define FAILCHECK(check, message) if((check == 0)) { MessageBox(NULL, message, L"ERROR", MB_OK); }

// Window size
#define WINWIDTH 1024
#define WINHEIGHT 768

// Camera related values
//#define FOV 90.0f
#define NEAR_Z 0.1f
#define FAR_Z 1000.0f

// STRUCTS FOR BUFFERS
ALIGN16
struct MatrixBuffer
{
	XMFLOAT4X4 wvp;
	XMFLOAT4X4 worldSpace;
};

ALIGN16
struct TessellationBuffer
{
	float tessellAmount;
	XMFLOAT3 padding;
};