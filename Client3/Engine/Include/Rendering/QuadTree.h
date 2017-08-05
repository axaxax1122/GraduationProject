#pragma once
#include "../Dx11Object.h"

DX11_BEGIN

class DX11_DLL QuadTree	: 
	public CDx11Object
{
private:
	friend class CDx11RenderManager;

public:
	enum CORNER_TYPE
	{
		LEFT_TOP,
		RIGHT_TOP,
		LEFT_BOT,
		RIGHT_BOT
	};

	enum QUAD_RELATION_FRUSTRUM
	{
		FRUSTRUM_OUT,
		FRUSTRUM_PARTIALLY_IN,
		FRUSTRUM_COMPLETELY_IN
	};

public:
	QuadTree();
	QuadTree(QuadTree* pParent);
	~QuadTree();

private:
	// 공간 좌표
	Vec3	m_vLeftTop;
	Vec3	m_vRightTop;
	Vec3	m_vLeftBot;
	Vec3	m_vRightBot;
	Vec3	m_vCenter;

	// 컬링 여부
	bool	m_bCulling;

	// 차일드
	QuadTree*	m_pChild[4];
	
	// 부모
	QuadTree*	m_pParent;

	// 정적 오브젝트 리스트
	vector<class CDx11GameObject*> m_StaticObjList;

	// 동적 오브젝트 리스트
	vector<class CDx11GameObject*> m_DynamicObjList;

	// 경계 구 반지름
	float	m_fRadius;

public:
	void SetNodeInfo(const Vec3& vLeftTop, const Vec3& vRightTop, const Vec3& vLeftBot, const Vec3& vRightBot);
	void SetParent(const Vec3& vLeftTop, const Vec3& vRightTop, const Vec3& vLeftBot, const Vec3& vRightBot);
	QuadTree* AddChild(const Vec3& vLeftTop, const Vec3& vRightTop, const Vec3& vLeftBot, const Vec3& vRightBot);
	bool SubDivide();
	bool BuildQuadTree();
	int IsInFrustrum();
	void FrustrumCulling();
	void PrintNode();
	bool IsInSquare(const Vec3& vMin, const Vec3& vMax);
	void AddStaticObj(class CDx11GameObject* pObj);
	void AddDynamicObj(class CDx11GameObject* pObj);
	void _StaticCheckNode(QuadTree* pNode, CDx11GameObject* pObj, const Vec3& vMin, const Vec3& vMax);
	void _DynamicCheckNode(QuadTree* pNode, CDx11GameObject* pObj, const Vec3& vMin, const Vec3& vMax);

public:
	Frustrum GetFrustrum() const;

};

DX11_END
