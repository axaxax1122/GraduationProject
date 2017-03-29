
#pragma once
#include <fbxsdk.h>
#include "Dx11Types.h"

#define	CRTDBG_MAP_ALLOC
#define	new	new(_CLIENT_BLOCK, __FILE__, __LINE__)

#ifdef DX11_IMPORT
#define	DX11_DLL	__declspec(dllimport)
#else
#define	DX11_DLL	__declspec(dllexport)
#endif // DX11_IMPORT

// File Path Key
#define	ROOT_PATH		"RootPath"
#define	SHADER_PATH		"ShaderPath"
#define	TEXTURE_PATH		"TexturePath"
#define	ENGINE_PATH			"EnginePath"
#define	ENGINE_TEXTURE_PATH	"EngineTexturePath"
#define	MESH_PATH		"MeshPath"

// Shader Key
#define	DEFAULT_SHADER	"DefaultShader"
#define	DEFAULT_TEX_SHADER	"DefaultTexShader"
#define	PIXELCUT_SHADER	"PixelCutShader"
#define	FOG_SHADER	"FogShader"
#define	SKY_SHADER	"SkyShader"
#define	COLLIDER_SHADER	"ColliderShader"
#define	UI_SHADER		"UIShader"
#define EffectShader	"EffectShader"
#define EffectAtlasShader	"EffectAtlasShader"
#define DEFAULT_BUMP_SHADER	"DEFAULT_BUMP_SHADER"

// Default Resource Key
#define	TRIANGLE_MESH		"TriangleMesh"
#define	RECT_TEX_MESH		"RectTexMesh"
#define	SPHERE_MESH			"SphereMesh"
#define PYRAMID_MESH		"PyramidMesh"
#define HEXAGON_MESH		"HexagonMesh"
#define HEXAGON_TEX_MESH	"HexagonTexMesh"
#define	RECT_UI_MESH		"RectUIMesh"
#define EffectCenterMesh	"EffectCenterMesh"
#define FBX_MESH			"FBX_MESH"
#define FBX_MESH2			"FBX_MESH"

// Sampler Keys
#define	DEFAULT_SAMPLER	"DefaultSampler"

// PI
#define	DX11_PI		3.141592f

// State Key
#define	DX11_RS_DEPTH_LESSEQUAL	"DepthLessEqual"
#define	DX11_RS_WIREFRAME		"WireFrame"
#define	DX11_RS_CULL_FRONTFACE	"CullFrontFace"
#define	DX11_RS_NON_FACECULL		"NONFACECULL"
#define	DX11_RS_ALPHABLEND		"AlphaBlend"
#define	DX11_RS_DEPTH_DISABLE	"DepthDisable"

template <typename T>
void Safe_Delete_Map(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		SAFE_DELETE(iter->second);
		++iter;
	}

	p.clear();
}

template <typename T>
void Safe_Release_Map(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		SAFE_RELEASE(iter->second);
		++iter;
	}

	p.clear();
}


template <typename T>
void Safe_Delete_VecList(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		SAFE_DELETE((*iter));
		++iter;
	}

	p.clear();
}

template <typename T>
void Safe_Release_VecList(T& p)
{
	T::iterator	iter = p.begin();

	while (iter != p.end())
	{
		SAFE_RELEASE((*iter));
		++iter;
	}

	p.clear();
}

