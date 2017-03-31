#pragma once
#include "../Dx11Object.h"

DX11_BEGIN

// 재질 노드 종류
enum FBX_MATERIAL_NODE
{
	FMN_LAMBERT,	// Diffuse만 처리
	FMN_PHONG		// 일반적으로 퐁 방식. (픽셀 단위의 처리)
};

// 재질 타입
enum FBX_MATERIAL_TYPE
{
	FMT_NONE,
	FMT_COLOR,		// Dif, Amb, Spc 만 존재
	FMT_TEXTURE,	// Diffuse Texture 만 존재
	FMT_BOTH,		// 둘다 존재
	FMT_MAX
};


// 재질 데이터 구조체
typedef struct _tagMaterialData
{
	FBX_MATERIAL_NODE	eNode;
	FBX_MATERIAL_TYPE	eType;
	Vec3	vDif;
	Vec3	vAmb;
	Vec3	vSpc;
	Vec3	vEmv;
	float	fSpcPower;
	float	fTransparencyFactor;
	string	strDiffuseTex;
	string	strSpecularTex;
	string	strNormalMapTex;
}FBXMATERIALDATA, *PFBXMATERIALDATA;

// 가중치 인덱스
typedef struct _tagFbxWeightIndices
{
	UINT	iBoneIndex;
	double	dWeight;
}FBXWEIGHTINDICES, *PFBXWEIGHTINDICES;

// 키 프레임
typedef struct _tagFbxKeyFrame
{
	FbxMatrix	matTransform;
	double		dTime;
}FBXKEYFRAME, *PFBXKEYFRAME;

// 본
typedef struct _tagFbxBone
{
	string		strName;
	int			iParentIndex;
	int			iDepth;
	FbxMatrix	matTransform;
	FbxMatrix	matOffset;
	vector<FBXKEYFRAME> vecKeyFrame;
}FBXBONE, *PFBXBONE;

// 애니메이션 클립
typedef struct _tagFbxAnimationClip
{
	string		strName;
	FbxTime		startTime;
	FbxTime		endTime;
	FbxLongLong	lTimeLength;
}FBXANIMATIONCLIP, *PFBXANIMATIONCLIP;

// 매쉬 데이터 구조체
typedef struct _tagMeshData
{
	bool			bAnimation;
	bool			bBump;
	int				iPolygonCount;
	vector<Vec3>	vecPos;
	vector<Vec3>	vecNormal;
	vector<Vec2>	vecUV;
	vector<Vec3>	vecTangent;
	vector<Vec3>	vecBinormal;
	vector<Vec4>	vecWeights;
	vector<Vec4>	vecIndices;
	vector<vector<UINT>>		vecIndex;
	vector<PFBXMATERIALDATA>	vecMaterial;
	PFBXANIMATIONCLIP		pAnimationClip;
	unordered_map<int, vector<FBXWEIGHTINDICES>>	mapWI;
}MESHDATA, *PMESHDATA;


class DX11_DLL CDx11FbxLoader :
	public CDx11Object
{
private:
	friend class CDx11Mesh;

private:
	CDx11FbxLoader();
	~CDx11FbxLoader();

private:
	FbxManager*	m_pManager;
	FbxScene*	m_pScene;
	vector<PMESHDATA> m_vecMeshData;
	vector<FBXBONE>			m_vecBone;
	FbxArray<FbxString*>	m_AnimNameArray;
	UINT					m_iMeshCount;

public:
	vector<PMESHDATA>& GetMeshContainer();
	int GetContainerCount();
	int GetSubsetCount(int iContainer);
	bool LoadMesh(char* pFileName);

private:
	void Triangulate(FbxNode* pNode);
	bool LoadNodeData(FbxNode* pNode);
	bool LoadMeshData(FbxMesh* pMesh);
	bool LoadMaterialData(FbxSurfaceMaterial* pMaterial);
	Vec3 GetMaterialProperty(FbxSurfaceMaterial* pMaterial, PFBXMATERIALDATA pMtrlData,
		const char* pPropertyName, const char* pPropertyFactorName);
	bool GetMaterialTextureName(FbxSurfaceMaterial* pMaterial, PFBXMATERIALDATA pMtrlData,
		const char* pPropertyName);

	void LoadAnimationData(FbxMesh*	pMesh, PMESHDATA pMeshData);
	bool LoadAnimationClipData(FbxMesh* pMesh, PMESHDATA pMeshData);
	void LoadSkeleton(FbxNode* pNode);
	void LoadSkeleton(FbxNode* pNode, int iDepth, int iIndex, int iParentIndex);
	bool LoadSkinningData(FbxMesh* pMesh, PMESHDATA pMeshData);
	int FindBoneFromName(const string& strName);
	FbxAMatrix GetTransform(FbxNode* pNode);
	void LoadWeightIndicesData(FbxCluster* pCluster, int iBoneIndex, PMESHDATA pMeshData);
	void LoadOffsetMatrixData(FbxCluster* pCluster, const FbxAMatrix& matGeometryTransform,
		int iBoneIndex, PMESHDATA pMeshData);
	void LoadTimeTransform(FbxNode* pNode, FbxCluster* pCluster, const FbxAMatrix& matGeometryTransform,
		int iBoneIndex, PMESHDATA pMeshData);
	void ChangeVertexWeightAndIndices(FbxMesh* pMesh, PMESHDATA pMeshData);

};

DX11_END