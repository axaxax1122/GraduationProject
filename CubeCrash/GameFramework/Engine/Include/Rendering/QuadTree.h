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
	// ���� ��ǥ
	Vec3	m_vLeftTop;
	Vec3	m_vRightTop;
	Vec3	m_vLeftBot;
	Vec3	m_vRightBot;
	Vec3	m_vCenter;

	// �ø� ����
	bool	m_bCulling;

	// ���ϵ�
	QuadTree*	m_pChild[4];
	
	// �θ�
	QuadTree*	m_pParent;

	// ���� ������Ʈ ����Ʈ
	vector<class CDx11GameObject*> m_StaticObjList;

	// ���� ������Ʈ ����Ʈ
	vector<class CDx11GameObject*> m_DynamicObjList;

	// ��� �� ������
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
