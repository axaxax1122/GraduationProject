#pragma once
#include "Dx11Component.h"

DX11_BEGIN

typedef struct _tagCBuffer
{
	string		strKey;
	void*		pData;
	int			iSize;
	int			iShaderType;
}CBUFFER, *PCBUFFER;


class DX11_DLL CDx11Renderer :
	public CDx11Component
{
private:
	friend class CDx11GameObject;

private:
	CDx11Renderer();
	CDx11Renderer(const CDx11Renderer& renderer);
	~CDx11Renderer();

private:
	class CDx11Mesh*		m_pMesh;
	class CDx11Shader*		m_pShader;
	vector<vector<class CDx11Material*>>	m_vecMaterial;
	class CDx11RenderState* m_pRenderState[RST_END];
	RENDER_TYPE_FLAG		m_eRenderType;
	unordered_map<string, PCBUFFER>		m_mapCBuffer;
	class CDx11Texture*		m_pBoneTexture;

public:
	RENDER_TYPE_FLAG GetRenderType() const;
	void SetRenderType(RENDER_TYPE_FLAG eRenderType);
	CDx11Material*	GetMaterial(int iContainer = 0, int iSubset = 0) const;

public:
	bool AddCBuffer(const string& strKey, const string& strCBufferKey,
		int iSize, int iShaderType);
	PCBUFFER FindCBuffer(const string& strKey);
	bool UpdateCBuffer(const string& strKey, void* pData, int iSize);
	void SetAnimCBuffer();
	void SetBoneTexture(class CDx11Texture* pBoneTex);

public:
	virtual bool Init();
	virtual int Update(float fTime);
	virtual int LateUpdate(float fTime);
	virtual void Render(float fTime);
	virtual CDx11Renderer* Clone();


public:
	void SetMesh(const string& strKey);
	void SetShader(const string& strKey);
	void SetMaterial(CDx11Material* pMtrl, int iContainer = 0);
	void CreateMaterial(const string& strDiffuseTexKey, int iContainer = 0,
		const string& strDiffuseSampKey = DEFAULT_SAMPLER, const string& strPointSampKey = POINT_SAMPLER);
	void CreateMaterial(const string& strDiffuseTexKey,
		wchar_t* pFileName, int iContainer = 0 ,const string& strPathKey = TEXTURE_PATH,
		const string& strDiffuseSampKey = DEFAULT_SAMPLER, const string& strPointSampKey = POINT_SAMPLER);
	void SetRenderState(const string& strKey);

private:
	void SetTransformCBuffer();
	void SetLightCBuffer(CDx11GameObject*);
	void SetMaterialCBuffer();
	void SetAnimationCBuffer();
};

DX11_END
