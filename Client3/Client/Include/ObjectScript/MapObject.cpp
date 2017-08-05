#include "MapObject.h"
#include "GameObject/Dx11GameObject.h"
#include "Component/Dx11Transform.h"
#include "Component/Dx11Collider.h"
#include "../../../Engine/Include/Core/Dx11Debug.h"
#include "Scene/Dx11Layer.h"
#include "PlayerCollision.h"
#include "Scene/Dx11Scene.h"
#include "Scene/Dx11SceneManager.h"
#include "Component/Dx11ColliderRay.h"
#include "Component/Dx11UIBackground.h"
#include "Player.h"
#include "Laser.h"
#include "Core/Dx11SoundManager.h"

CMapObject::CMapObject() :
	ObjPos(0, 0, 0),
	ObjSize(0, 0, 0),
	ObjRot(0, 0, 0),
	ObjHalfSize(0, 0, 0),
	PlayerPos(0, 0, 0),
	PlayerSize(0, 0, 0),
	PlayerHalfSize(0, 0, 0),
	My_front(0),
	My_back(0),
	My_Right(0),
	My_Left(0),
	My_up(0),
	My_down(0),
	Obj_Front(0),
	Obj_Back(0),
	Obj_Up(0),
	Obj_Down(0),
	Obj_Right(0),
	Obj_Left(0),
	pPlayerColl(NULL),
	pPlayer(NULL),
	m_fNomalX(0.f),
	m_fNomalZ(0.f),
	m_Count(0)
{
}
CMapObject::CMapObject(const CMapObject & map)	:
	CDx11Script(map)
{
}
CMapObject::~CMapObject()
{
	SAFE_RELEASE(pPlayerColl);
	SAFE_RELEASE(pPlayer);	
	
}
bool CMapObject::Init()
{

	return true;
}


bool CMapObject::left_collision(CDx11GameObject *pPlayer) // ���� ���ʰ� ���� �浹
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_front = PlayerPos.z - PlayerHalfSize.z;
	My_back = PlayerPos.z + PlayerHalfSize.z;
	My_up = PlayerPos.y + PlayerHalfSize.y;
	My_down = PlayerPos.y - PlayerHalfSize.y;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = ObjSize / 2.f;

	Obj_Front = ObjPos.z - ObjHalfSize.z;
	Obj_Back = ObjPos.z + ObjHalfSize.z;
	Obj_Up = ObjPos.y + (ObjHalfSize.y*0.75f);
	Obj_Down = ObjPos.y - (ObjHalfSize.y);

	//------------------------�浹�˻�--------------------------
	if (PlayerPos.x >(ObjPos.x + ObjHalfSize.x)) {
		if (My_front<Obj_Back && My_back>Obj_Front && My_up > Obj_Down && My_down < Obj_Up)
			return true;
	}
	else
		return false;
}

bool CMapObject::right_collision(CDx11GameObject *pPlayer)
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_front = PlayerPos.z - PlayerHalfSize.z;
	My_back = PlayerPos.z + PlayerHalfSize.z;
	My_up = PlayerPos.y + PlayerHalfSize.y;
	My_down = PlayerPos.y - PlayerHalfSize.y;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = ObjSize / 2.f;

	Obj_Front = ObjPos.z - ObjHalfSize.z;
	Obj_Back = ObjPos.z + ObjHalfSize.z;
	Obj_Up = ObjPos.y + (ObjHalfSize.y*0.75f);
	Obj_Down = ObjPos.y - (ObjHalfSize.y);

	//------------------------�浹�˻�--------------------------
	if (PlayerPos.x < (ObjPos.x - ObjHalfSize.x)) {
		if (My_front<Obj_Back && My_back>Obj_Front && My_up > Obj_Down && My_down < Obj_Up)
			return true;
	}
	else
		return false;
}

bool CMapObject::front_collision(CDx11GameObject *pPlayer)
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_Right = PlayerPos.x + PlayerHalfSize.x;
	My_Left = PlayerPos.x - PlayerHalfSize.x;
	My_up = PlayerPos.y + PlayerHalfSize.y;
	My_down = PlayerPos.y - PlayerHalfSize.y;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = ObjSize / 2.f;

	Obj_Right = ObjPos.x + ObjHalfSize.x;
	Obj_Left = ObjPos.x - ObjHalfSize.x;
	Obj_Up = ObjPos.y + (ObjHalfSize.y*0.75f);
	Obj_Down = ObjPos.y - (ObjHalfSize.y);

	//------------------------�浹�˻�--------------------------
	if (PlayerPos.z > (ObjPos.z + ObjHalfSize.z)) {
		if (My_Right > Obj_Left && My_Left<Obj_Right && My_up>Obj_Down && My_down < Obj_Up)
			return true;
	}
	else
		return false;
}

bool CMapObject::back_collision(CDx11GameObject *pPlayer)
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_Right = PlayerPos.x + PlayerHalfSize.x;
	My_Left = PlayerPos.x - PlayerHalfSize.x;
	My_up = PlayerPos.y + PlayerHalfSize.y;
	My_down = PlayerPos.y - PlayerHalfSize.y;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = (ObjSize) / 2.f;

	Obj_Right = ObjPos.x + ObjHalfSize.x;
	Obj_Left = ObjPos.x - ObjHalfSize.x;
	Obj_Up = ObjPos.y + (ObjHalfSize.y*0.75f);
	Obj_Down = ObjPos.y - (ObjHalfSize.y);

	//------------------------�浹�˻�--------------------------
	if (PlayerPos.z < (ObjPos.z - ObjHalfSize.z)) {
		if (My_Right > Obj_Left && My_Left<Obj_Right && My_up>Obj_Down && My_down < Obj_Up)
			return true;
	}
	else
		return false;
}

bool CMapObject::Down_collision(CDx11GameObject *pPlayer)
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_Right = PlayerPos.x + PlayerHalfSize.x;
	My_Left = PlayerPos.x - PlayerHalfSize.x;
	My_front = PlayerPos.z - PlayerHalfSize.z;
	My_back = PlayerPos.z + PlayerHalfSize.z;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = ObjSize / 2.f;

	Obj_Right = ObjPos.x + ObjHalfSize.x;
	Obj_Left = ObjPos.x - ObjHalfSize.x;
	Obj_Front = ObjPos.z - (ObjHalfSize.z);
	Obj_Back = ObjPos.z + (ObjHalfSize.z);

	//------------------------�浹�˻�--------------------------
	if (PlayerPos.y > (ObjPos.y + ObjHalfSize.y)) {
		return true;
	}
	else
		return false;
}

bool CMapObject::Back_Left_collision(CDx11GameObject * pPlayer)
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_Right = PlayerPos.x + PlayerHalfSize.x;
	My_Left = PlayerPos.x - PlayerHalfSize.x;
	My_up = PlayerPos.y + PlayerHalfSize.y;
	My_down = PlayerPos.y - PlayerHalfSize.y;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = (ObjSize) / 2.f;

	Obj_Right = ObjPos.x + ObjHalfSize.x;
	Obj_Left = ObjPos.x - ObjHalfSize.x;
	Obj_Up = ObjPos.y + (ObjHalfSize.y*0.75f);
	Obj_Down = ObjPos.y - (ObjHalfSize.y);

	//------------------------�浹�˻�--------------------------
	if ((m_pGameObject->GetTag() != "J1") && (m_pGameObject->GetTag() != "J2") && (m_pGameObject->GetTag() != "J3") && (m_pGameObject->GetTag() != "J4"))
		if (PlayerPos.z < (ObjPos.z - ObjHalfSize.z / 2.0f)) //3��(������2�� �𼭸��� ���̵���)
			if (My_up > Obj_Down && My_down < Obj_Up) //���Ʒ��� �⺻������ �ϰ�~
				if (My_Left<Obj_Left &&My_Right >Obj_Left) //1��&&2��
					return true;
				else
					return false;
}

bool CMapObject::Back_Right_collision(CDx11GameObject * pPlayer)
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_Right = PlayerPos.x + PlayerHalfSize.x;
	My_Left = PlayerPos.x - PlayerHalfSize.x;
	My_up = PlayerPos.y + PlayerHalfSize.y;
	My_down = PlayerPos.y - PlayerHalfSize.y;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = (ObjSize) / 2.f;

	Obj_Right = ObjPos.x + ObjHalfSize.x;
	Obj_Left = ObjPos.x - ObjHalfSize.x;
	Obj_Up = ObjPos.y + (ObjHalfSize.y*0.75f);
	Obj_Down = ObjPos.y - (ObjHalfSize.y);

	//------------------------�浹�˻�--------------------------
	if ((m_pGameObject->GetTag() != "J1") && (m_pGameObject->GetTag() != "J2") && (m_pGameObject->GetTag() != "J3") && (m_pGameObject->GetTag() != "J4"))
		if (PlayerPos.z < (ObjPos.z - ObjHalfSize.z / 2.0f))//3��(������2�� �𼭸��� ���̵���)
			if (My_up > Obj_Down && My_down < Obj_Up)//���Ʒ��� �⺻������ �ϰ�~
				if (My_Left<Obj_Right &&My_Right >Obj_Right)//1��&&2��
					return true;
				else
					return false;
}

bool CMapObject::Front_Left_collision(CDx11GameObject * pPlayer)
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_Right = PlayerPos.x + PlayerHalfSize.x;
	My_Left = PlayerPos.x - PlayerHalfSize.x;
	My_up = PlayerPos.y + PlayerHalfSize.y;
	My_down = PlayerPos.y - PlayerHalfSize.y;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = ObjSize / 2.f;

	Obj_Right = ObjPos.x + ObjHalfSize.x;
	Obj_Left = ObjPos.x - ObjHalfSize.x;
	Obj_Up = ObjPos.y + (ObjHalfSize.y*0.75f);
	Obj_Down = ObjPos.y - (ObjHalfSize.y);

	//------------------------�浹�˻�--------------------------
	if ((m_pGameObject->GetTag() != "J1") && (m_pGameObject->GetTag() != "J2") && (m_pGameObject->GetTag() != "J3") && (m_pGameObject->GetTag() != "J4"))
		if (PlayerPos.z > (ObjPos.z + ObjHalfSize.z / 2.f))//3��(������2�� �𼭸��� ���̵���)
			if (My_up>Obj_Down && My_down < Obj_Up)//���Ʒ��� �⺻������ �ϰ�~
				if (My_Left<Obj_Left &&My_Right >Obj_Left) //1��&&2��
					return true;
				else
					return false;
}

bool CMapObject::Front_Right_collision(CDx11GameObject * pPlayer)
{
	//�÷��̾�
	CDx11Transform*	pTr = pPlayer->GetTransform();
	PlayerPos = pTr->GetWorldPos();
	PlayerSize = pTr->GetWorldScale();
	PlayerHalfSize = (PlayerSize / 2)*27.f;
	SAFE_RELEASE(pTr);

	My_Right = PlayerPos.x + PlayerHalfSize.x;
	My_Left = PlayerPos.x - PlayerHalfSize.x;
	My_up = PlayerPos.y + PlayerHalfSize.y;
	My_down = PlayerPos.y - PlayerHalfSize.y;

	//������Ʈ 
	ObjPos = m_pTransform->GetWorldPos();
	ObjHalfSize = ObjSize / 2.f;

	Obj_Right = ObjPos.x + ObjHalfSize.x;
	Obj_Left = ObjPos.x - ObjHalfSize.x;
	Obj_Up = ObjPos.y + (ObjHalfSize.y*0.75f);
	Obj_Down = ObjPos.y - (ObjHalfSize.y);

	//------------------------�浹�˻�--------------------------
	if ((m_pGameObject->GetTag() != "J1") && (m_pGameObject->GetTag() != "J2") && (m_pGameObject->GetTag() != "J3") && (m_pGameObject->GetTag() != "J4"))
		if (PlayerPos.z > (ObjPos.z + ObjHalfSize.z / 2.f))//3��(������2�� �𼭸��� ���̵���)
			if (My_up>Obj_Down && My_down < Obj_Up)//���Ʒ��� �⺻������ �ϰ�~
				if (My_Left<Obj_Right &&My_Right >Obj_Right)//1��&&2��
					return true;
				else
					return false;
}




void CMapObject::MyLocation(CDx11GameObject *pObject)
{
	//�浹 x -> exit �ϸ� ó�� �س�.
	//if (!left_collision(pObject) && !right_collision(pObject) && !front_collision(pObject)
	//	&& !back_collision(pObject) && !Up_collision(pObject))
	//{
	//	pPlayerColl->SetPS(NoColl);
	//}
	////�÷��̾� ���� �浹
	if (left_collision(pObject) == true)
	{
		//cout << "�÷��̾� ���� �浹" << endl;
		pPlayerColl->SetPS(LeftColl, m_pGameObject);
	}
	////�÷��̾� ������ �浹
	if (right_collision(pObject) == true)
	{
		//cout << "�÷��̾� ������ �浹" << endl;
		pPlayerColl->SetPS(RightColl, m_pGameObject);
	}
	////�÷��̾� �� �浹
	if (front_collision(pObject) == true)
	{
		//cout << "�÷��̾� �� �浹" << endl;
		pPlayerColl->SetPS(FrontColl, m_pGameObject);
	}
	////�÷��̾� �� �浹
	if (back_collision(pObject) == true)
	{
		//cout << "�÷��̾� �� �浹" << endl;
		pPlayerColl->SetPS(BackColl, m_pGameObject);
	}
	//�÷��̾� �ٴ� �浹
	if (Down_collision(pObject) == true)
	{
		//cout << "�÷��̾� �ٴ� �浹" << endl;
		pPlayerColl->SetPS(DownColl, m_pGameObject);
	}
	//------------------�𼭸� 4���浹 ----------------------
	//����_����
	if (Back_Left_collision(pObject) == true)
	{
		//cout << "�÷��̾� ��_���� �𼭸� �浹" << endl;
		pPlayerColl->SetPS(Eege_Back_LeftColl, m_pGameObject);
	}
	//����_������
	if (Back_Right_collision(pObject) == true)
	{
		//cout << "�÷��̾� ��_������ �𼭸� �浹" << endl;
		pPlayerColl->SetPS(Eege_Back_RightColl, m_pGameObject);
	}
	//����_����
	if (Front_Left_collision(pObject) == true)
	{
		//cout << "�÷��̾� ��_���� �𼭸� �浹" << endl;
		pPlayerColl->SetPS(Eege_Front_LeftColl, m_pGameObject);
	}
	//����_������
	if (Front_Right_collision(pObject) == true)
	{
		//cout << "�÷��̾� ��_������ �𼭸� �浹" << endl;
		pPlayerColl->SetPS(Eege_Front_RightColl, m_pGameObject);
	}
	////�÷��̾� �� �浹
	//if (Up_collision(pObject) == true)
	//{
	//	pPlayerColl->SetPS(UpColl, m_pGameObject);
	//}
	//�׿� ����ó��(�����浹,������ ���°��)
	//else {
	//	cout << "�׿� ���� �浹" << endl;
	//}

}


void CMapObject::SetSize(Vec3 vSize)
{	
	ObjSize = vSize;
}

void CMapObject::Input(float fTime)
{
}

int CMapObject::Update(float fTime)
{
	return 0;
}

int CMapObject::LateUpdate(float fTime)
{
	if (!pPlayer) 
	{
		CDx11Scene*	pScene = DX11_GET_SINGLE(CDx11SceneManager)->GetCurrentScene();
		CDx11Layer*	pLayer = pScene->FindLayer("DefaultLayer");
		if (!pLayer)
			return false;
		pPlayer = pLayer->FindGameObjectFromTag("PlayerObject");
		pPlayerColl = (CPlayerCollision*)pPlayer->FindComponentFromTag("PlayerCollisionGo");	
		
		SAFE_RELEASE(pLayer);
		SAFE_RELEASE(pScene);
	}
	return 0;
}

void CMapObject::Render(float fTime)
{
}

CMapObject * CMapObject::Clone()
{
	return nullptr;
}

void CMapObject::OnCollisionEnter(CDx11Collider * pColl, float fTime)
{
	//�ʿ�����Ʈ �� ĳ����
	if (pColl->GetTag() == "PlayerColl")
	{	
		CDx11GameObject *pObject = pColl->GetGameObject();
		pColl->SetColor(Red);
		MyLocation(pObject);//ĳ���� �浹ü
		SAFE_RELEASE(pObject);
	}

	if ((pColl->GetTag() == "MyLaserColl") ||
		(pColl->GetTag() == "UserLaserColl"))
	{
		if (m_Count == 0) {
			m_Count++;
			DX11_GET_SINGLE(Dx11SoundManager)->PlayWALL();
		}
			
		CDx11GameObject*	pObj = pColl->GetGameObject();
		CDx11Transform*		pTr = pObj->GetTransform();

		Vec3	vPos = pTr->GetWorldPos();
		CLaser*	pLaser = (CLaser*)pObj->FindComponentFromType(CT_SCRIPT);
		pLaser->DeathMotion();
		pObj->Death();
		SAFE_RELEASE(pLaser);
		SAFE_RELEASE(pTr);
		SAFE_RELEASE(pObj);

	}

	if ((pColl->GetTag() == "UserColl"))
	{

	}
}

void CMapObject::OnCollision(CDx11Collider * pColl, float fTime)
{
	if (pColl->GetTag() == "PlayerColl")
	{
		CDx11GameObject *pObject = pColl->GetGameObject();
		CDx11Transform *pObjTr = pObject->GetTransform();
		MyLocation(pObject);
		//cout << pObjTr->GetWorldAxis(AXIS_Z).x << "/" << pObjTr->GetWorldAxis(AXIS_Z).y << "/" << pObjTr->GetWorldAxis(AXIS_Z).z << "/" << endl;

		if (pPlayerColl->GetPS() == LeftColl) {
			float temp = pObjTr->GetWorldAxis(AXIS_Z).x;
			if (m_fNomalX < 0)
				if (temp > 0) 		pObjTr->MoveWorld(60.f, fTime, Vec3(1.f, 0, 0));
			m_fNomalX = pObjTr->GetWorldAxis(AXIS_Z).x;
		}
		if (pPlayerColl->GetPS() == RightColl) {
			float temp = pObjTr->GetWorldAxis(AXIS_Z).x;
			if (m_fNomalX > 0)
				if (temp < 0) 		pObjTr->MoveWorld(60.f, fTime, Vec3(-1.f, 0, 0));
			m_fNomalX = pObjTr->GetWorldAxis(AXIS_Z).x;
		}
		if (pPlayerColl->GetPS() == FrontColl) {
			float temp = pObjTr->GetWorldAxis(AXIS_Z).z;
			if (m_fNomalZ < 0)
				if (temp >= 0) 		pObjTr->MoveWorld(60.f, fTime, Vec3(0.f, 0, 1));
			m_fNomalZ = pObjTr->GetWorldAxis(AXIS_Z).z;
		}
		if (pPlayerColl->GetPS() == BackColl) {
			float temp = pObjTr->GetWorldAxis(AXIS_Z).z;
			if (m_fNomalZ > 0)
				if (temp <= 0)		pObjTr->MoveWorld(60.f, fTime, Vec3(0.f, 0, -1));
			m_fNomalZ = pObjTr->GetWorldAxis(AXIS_Z).z;
		}
		SAFE_RELEASE(pObjTr);
		SAFE_RELEASE(pObject);
	}

	// ũ�ν���� ���� �浹
	if (pColl->GetTag() == "AimColl")
	{
		//RAY tRay = ((CDx11ColliderRay*)pColl)->GetRay();
		//CPlayer* pPlayerObj = (CPlayer*)pPlayer->FindComponentFromType(CT_SCRIPT);
		//pPlayerObj->ShootMissile(tRay.vDir, fTime);
		//SAFE_RELEASE(pPlayerObj);
	}

}

void CMapObject::OnCollisionExit(CDx11Collider * pColl, float fTime)
{
	if (pColl->GetTag() == "PlayerColl")
	{
		pPlayerColl->SetPS(NoColl);
		pColl->SetColor(White);
	}
	m_Count = 0;
}
