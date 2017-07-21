#include "Dx11FbxLoader.h"

DX11_USING

CDx11FbxLoader::CDx11FbxLoader()	:
	m_pManager(NULL),
	m_pScene(NULL),
	m_iMeshCount(0)
{
}


CDx11FbxLoader::~CDx11FbxLoader()
{
	// �Ž� �������� ũ�⸸ŭ ������ ����,
	// ������ �Ž� �����̳ʾ��� ���� ���͸� Safe_Delete
	for (size_t i = 0; i < m_vecMeshData.size(); ++i)
	{
		SAFE_DELETE(m_vecMeshData[i]->pAnimationClip);
		Safe_Delete_VecList(m_vecMeshData[i]->vecMaterial);
	}
	
	for (int i = 0; i < m_AnimNameArray.Size(); ++i)
	{
		m_AnimNameArray[i]->Clear();
	}
	m_AnimNameArray.Clear();

	Safe_Delete_VecList(m_vecMeshData);
	Safe_Delete_VecList(m_vecAnimationClip);
	m_pScene->Destroy();
	m_pManager->Destroy();

}

vector<PMESHDATA>& CDx11FbxLoader::GetMeshContainer()
{
	return m_vecMeshData;
}

vector<FBXBONE>& CDx11FbxLoader::GetBones()
{
	return m_vecBone;
}

vector<PFBXANIMATIONCLIP>& CDx11FbxLoader::GetAnimationClip()
{
	return m_vecAnimationClip;
}

int CDx11FbxLoader::GetContainerCount()
{
	// �Ž� �����̳��� ũ��
	return m_vecMeshData.size();
}

int CDx11FbxLoader::GetSubsetCount(int iContainer)
{
	// i��° �Ž� �����̳��� �ε��� ������ ũ��
	return m_vecMeshData[iContainer]->vecIndex.size();
}

bool CDx11FbxLoader::LoadMesh(char * pFileName)
{
	m_iMeshCount = 0;

	// Fbx �Ŵ��� ����
	m_pManager = FbxManager::Create();

	if (!m_pManager)
		return false;

	// ����� ������ ����� �Ŵ����� �����Ѵ�.
	FbxIOSettings* ios = FbxIOSettings::Create(m_pManager, IOSROOT);
	m_pManager->SetIOSettings(ios);

	FbxString IPath = FbxGetApplicationDirectory();
	m_pManager->LoadPluginsDirectory(IPath.Buffer());

	FbxImporter* pImporter = FbxImporter::Create(m_pManager, "");

	// Importer�� Initialize�� ���ָ� ���ڷ� �� ���ϸ��� fbx��
	// �о�� �� �ֵ��� �غ��Ų��.
	if (!pImporter->Initialize(pFileName, -1, m_pManager->GetIOSettings()))
		return false;

	m_pScene = FbxScene::Create(m_pManager, "FbxScene");
	pImporter->Import(m_pScene);

	// Fbx ���� �����ش�.
	FbxAxisSystem SceneAxisSystem = m_pScene->GetGlobalSettings().GetAxisSystem();

	// DirectX�� �޼� ��ǥ��� ���װ� �����Ƿ� 3dMax ��ǥ�踦 �̿��Ѵ�.
	// 3dMax�� �޼� ��ǥ�迡���� Y�� Z�� ���� �ݴ��̴�.
	if (SceneAxisSystem != FbxAxisSystem::Max)
		FbxAxisSystem::Max.ConvertScene(m_pScene);
	
	//Triangulate ���ش�.
	Triangulate(m_pScene->GetRootNode());

	// �ִϸ��̼� �̸��� ���´�.
	m_pScene->FillAnimStackNameArray(m_AnimNameArray);

	// �� ������ �о�´�.
	LoadSkeleton(m_pScene->GetRootNode());

	// �ִϸ��̼� Ŭ�� ������ �о�´�.
	LoadAnimationClipData();

	// ����� �����͸� �ε��Ѵ�.
	if (!LoadNodeData(m_pScene->GetRootNode()))
		return false;

	pImporter->Destroy();

	return true;
}

void CDx11FbxLoader::Triangulate(FbxNode * pNode)
{
	// ������ Ÿ���� �ﰢ���� �ƴ� ��� �ﰢ������ ��ȯ���ش�.
	// ����� �Ӽ��� �޾ƿͼ� Ÿ���� �˻��Ѵ�.
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr)
	{
		FbxNodeAttribute::EType eType = pAttr->GetAttributeType();
		if (eType == FbxNodeAttribute::eMesh ||
			eType == FbxNodeAttribute::eNurbs ||
			eType == FbxNodeAttribute::eNurbsSurface ||
			eType == FbxNodeAttribute::ePatch)
		{
			FbxGeometryConverter	converter(m_pManager);
			converter.Triangulate(pAttr, true);
		}
	}

	// ���ϵ� ��嵵 ��ȸ�ؼ� ������ ó���� ���ش�.
	int iChildCount = pNode->GetChildCount();
	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(pNode->GetChild(i));
	}
}

bool CDx11FbxLoader::LoadNodeData(FbxNode * pNode)
{
	// ����� �Ӽ��� �޾ƿ´�.
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr)
	{	
		// �Ž� �Ӽ��� ���´ٸ�
		if (pAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			// �Ž��� �޾ƿ���, �ҷ��´�.
			FbxMesh*	pMesh = pNode->GetMesh();

			if (pMesh)
			{
				// �Ž��� �����͸� �ε��Ѵ�.
				if (!LoadMeshData(pMesh))
					return false;

				// �Ž��� ���� ������ ī��Ʈ��ŭ ������ ������.
				int iMtrlCount = pNode->GetMaterialCount();
				for (int i = 0; i < iMtrlCount; ++i)
				{
					// i��° ������ �̾ƿ´�.
					FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
					if (!pMaterial)
						continue;

					// �̾ƿ� ������ �ε��Ѵ�.
					LoadMaterialData(pMaterial);
				}
			}
		}
	}

	// ���������� ���� �ִ� ���ϵ带 ��� ��ȸ���ش�.
	int iChildCount = pNode->GetChildCount();
	for (int i = 0; i < iChildCount; ++i)
	{
		if (!LoadNodeData(pNode->GetChild(i)))
			return false;
	}

	return true;
}

bool CDx11FbxLoader::LoadMeshData(FbxMesh * pMesh)
{
	if (!pMesh->GetNode())
		return false;

	PMESHDATA	pMeshData = new MESHDATA;
	pMeshData->pAnimationClip = NULL;
	pMeshData->bAnimation = false;
	pMeshData->bBump = false;
	m_vecMeshData.push_back(pMeshData);

	// ������ ������ ���´�.
	// ��Ʈ�� ����Ʈ�� ���´�. ControlPoint == ���� ������ ��
	int iPolygonCount = pMesh->GetPolygonCount();
	FbxVector4* pControlPoints = pMesh->GetControlPoints();
	int iControlPointsCount = pMesh->GetControlPointsCount();

	// ������ ������ �̸� Ȯ���س��´�.
	pMeshData->vecPos.reserve(iControlPointsCount);
	pMeshData->vecNormal.reserve(iControlPointsCount);
	pMeshData->vecUV.reserve(iControlPointsCount);
	pMeshData->vecTangent.reserve(iControlPointsCount);
	pMeshData->vecBinormal.reserve(iControlPointsCount);
	pMeshData->vecWeights.reserve(iControlPointsCount);
	pMeshData->vecIndices.reserve(iControlPointsCount);

	// ������ ��ġ ������ �о�´�.
	for (int i = 0; i < iControlPointsCount; ++i)
	{
		Vec3	vPos;
		vPos.x = (float)pControlPoints[i].mData[0];
		vPos.y = (float)pControlPoints[i].mData[2];
		vPos.z = (float)pControlPoints[i].mData[1];

		pMeshData->vecPos.push_back(vPos);

		//������ ������ �ʱ�ȭ�� ����
		pMeshData->vecNormal.push_back(Vec3Zero);
		pMeshData->vecUV.push_back(Vec2(0.f, 0.f));
		pMeshData->vecTangent.push_back(Vec3Zero);
		pMeshData->vecBinormal.push_back(Vec3Zero);
		pMeshData->vecWeights.push_back(Vec4(0.f, 0.f, 0.f, 0.f));
		pMeshData->vecIndices.push_back(Vec4(0.f, 0.f, 0.f, 0.f));
	}

	pMeshData->iPolygonCount = iPolygonCount;

	UINT	iVertexIdx = 0;
	
	int iMtrlElementCount = pMesh->GetElementMaterialCount();
	// ������ ���� ���´�.
	int iMtrlCount = pMesh->GetNode()->GetMaterialCount();
	for (int i = 0; i < iMtrlCount; ++i)
	{
		vector<UINT>	vecIndex;
		pMeshData->vecIndex.push_back(vecIndex);
	}

	// ������ ����ŭ �ݺ��� ���� �� �������� ���� ������ ���´�.
	for (int i = 0; i < iPolygonCount; ++i)
	{
		// �ش� �ε����� ������ ũ��(������ ����)�� ���´�.
		int iPolygonSize = pMesh->GetPolygonSize(i);
		int iMaterialID = 0;

		if (iMtrlElementCount > 0)
		{
			FbxGeometryElementMaterial*	pMaterial = pMesh->GetElementMaterial(0);
			iMaterialID = pMaterial->GetIndexArray().GetAt(i);
		}

		int iIndex[3] = {};

		for (int j = 0; j < iPolygonSize; ++j)
		{
			// �ﰢ���� �����ϱ� ���� ������ �ε��� ������ ���´�.
			int iControlIndex = pMesh->GetPolygonVertex(i, j);
			
			iIndex[j] = iControlIndex;
			
			// �븻 ������ ���´�.
			// ���� �ϳ��� �븻�� � ���� ���� �������̹Ƿ� ������ ���� ó���Ѵ�.
			int iNormalCount = pMesh->GetElementNormalCount();

			for (int k = 0; k < iNormalCount; ++k)
			{
				FbxGeometryElementNormal* pNormal = pMesh->GetElementNormal(k);
				if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					int iNormalIndex = 0;
					FbxVector4	vFbxNormal;
					Vec3		vNormal;
					switch (pNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						vFbxNormal = pNormal->GetDirectArray().GetAt(iVertexIdx);
						vNormal.x = vFbxNormal.mData[0];
						vNormal.y = vFbxNormal.mData[2];
						vNormal.z = vFbxNormal.mData[1];
						pMeshData->vecNormal[iControlIndex] = vNormal;
					//	pMeshData->vecNormal.push_back(vNormal);
						break;
					case FbxGeometryElement::eIndexToDirect:
						iNormalIndex = pNormal->GetIndexArray().GetAt(iVertexIdx);
						vFbxNormal = pNormal->GetDirectArray().GetAt(iNormalIndex);
						vNormal.x = vFbxNormal.mData[0];
						vNormal.y = vFbxNormal.mData[2];
						vNormal.z = vFbxNormal.mData[1];
						pMeshData->vecNormal[iControlIndex] = vNormal;
						break;
					}
				}
			}


			// �ؽ��� UV ������ �о�´�.
			for (int k = 0; k < pMesh->GetElementUVCount(); ++k)
			{
				FbxGeometryElementUV*	pUV = pMesh->GetElementUV(k);

				if (pUV)
				{
					FbxVector2	vFbxUV;
					Vec2		vUV;
					int			iUVIndex = 0;
					switch (pUV->GetMappingMode())
					{
					case FbxGeometryElement::eByControlPoint:
						switch (pUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							vFbxUV = pUV->GetDirectArray().GetAt(iControlIndex);
							vUV.x = vFbxUV.mData[0];
							vUV.y = 1.f - vFbxUV.mData[1];
							pMeshData->vecUV[iControlIndex] = vUV;
							break;
						case FbxGeometryElement::eIndexToDirect:
							iUVIndex = pUV->GetIndexArray().GetAt(iControlIndex);
							vFbxUV = pUV->GetDirectArray().GetAt(iUVIndex);
							vUV.x = vFbxUV.mData[0];
							vUV.y = 1.f - vFbxUV.mData[1];
							pMeshData->vecUV[iControlIndex] = vUV;
							break;
						}
						break;
					case FbxGeometryElement::eByPolygonVertex:
						iUVIndex = pMesh->GetTextureUVIndex(i, j);
						switch (pUV->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						case FbxGeometryElement::eIndexToDirect:
							vFbxUV = pUV->GetDirectArray().GetAt(iUVIndex);
							vUV.x = vFbxUV.mData[0];
							vUV.y = 1.f - vFbxUV.mData[1];
							pMeshData->vecUV[iControlIndex] = vUV;
							break;
						}
						break;
					}
				}
			}


			// Tangent ������ �о�´�.
			int iTangentCount = pMesh->GetElementTangentCount();
			for (int k = 0; k < iTangentCount; ++k)
			{
				FbxGeometryElementTangent*	pTangent = pMesh->GetElementTangent(k);
				if (pTangent)
				{
					int iTangentIndex = 0;
					FbxVector4	vFbxTangent;
					Vec3		vTangent;
					switch (pTangent->GetMappingMode())
					{
					case FbxGeometryElement::eByPolygonVertex:
						switch (pTangent->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							vFbxTangent = pTangent->GetDirectArray().GetAt(iVertexIdx);
							vTangent.x = vFbxTangent.mData[0];
							vTangent.y = vFbxTangent.mData[2];
							vTangent.z = vFbxTangent.mData[1];
							
							pMeshData->vecTangent[iControlIndex] = vTangent;
							pMeshData->bBump = true;
							break;
						case FbxGeometryElement::eIndexToDirect:
							iTangentIndex = pTangent->GetIndexArray().GetAt(iVertexIdx);
							vFbxTangent = pTangent->GetDirectArray().GetAt(iTangentIndex);
							vTangent.x = vFbxTangent.mData[0];
							vTangent.y = vFbxTangent.mData[2];
							vTangent.z = vFbxTangent.mData[1];

							pMeshData->vecTangent[iControlIndex] = vTangent;
							pMeshData->bBump = true;
							break;
						}
						break;
					}
				}
			}

			// ���̳븻 ������ �о�´�.
			int iBinormalCount = pMesh->GetElementBinormalCount();
			for (int k = 0; k < iBinormalCount; ++k)
			{
				FbxGeometryElementBinormal* pBinormal = pMesh->GetElementBinormal(k);
				if (pBinormal)
				{
					int iBinormalIndex = 0;
					FbxVector4	vFbxBinormal;
					Vec3		vBinormal;
					switch (pBinormal->GetMappingMode())
					{
					case FbxGeometryElement::eByPolygonVertex:
						switch (pBinormal->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
							vFbxBinormal = pBinormal->GetDirectArray().GetAt(iVertexIdx);
							vBinormal.x = vFbxBinormal.mData[0];
							vBinormal.y = vFbxBinormal.mData[2];
							vBinormal.z = vFbxBinormal.mData[1];
							pMeshData->vecBinormal[iControlIndex] = vBinormal;
							break;
						case FbxGeometryElement::eIndexToDirect:
							iBinormalIndex = pBinormal->GetIndexArray().GetAt(iVertexIdx);
							vFbxBinormal = pBinormal->GetDirectArray().GetAt(iBinormalIndex);
							vBinormal.x = vFbxBinormal.mData[0];
							vBinormal.y = vFbxBinormal.mData[2];
							vBinormal.z = vFbxBinormal.mData[1];
							pMeshData->vecBinormal[iControlIndex] = vBinormal;
							break;
						}
						break;
					}
				}
			}// Binormal

			++iVertexIdx;
		}// PolygonSize

		pMeshData->vecIndex[iMaterialID].push_back(iIndex[0]);
		pMeshData->vecIndex[iMaterialID].push_back(iIndex[2]);
		pMeshData->vecIndex[iMaterialID].push_back(iIndex[1]);

	}// PolygonCount

	// �ִϸ��̼� ������ ���´�.
	LoadAnimationData(pMesh, pMeshData);
	++m_iMeshCount;

	return true;
}

bool CDx11FbxLoader::LoadMaterialData(FbxSurfaceMaterial * pMaterial)
{
	if (!pMaterial)
		return false;

	// ���� ���� ������ �������� �Ҵ�
	// ���� �Ž� �����̳ʴ� ������ ���� ������ ����
	PFBXMATERIALDATA	pMtrlData = new FBXMATERIALDATA;
	PMESHDATA			pContainer = m_vecMeshData.back();

	pContainer->vecMaterial.push_back(pMtrlData);

	// ���� ��� Ÿ�� ����
	if (pMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		pMtrlData->eNode = FMN_LAMBERT;

	else if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		pMtrlData->eNode = FMN_PHONG;


	// Diffuse
	pMtrlData->vDif = GetMaterialProperty(pMaterial, pMtrlData,
		FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	// Diffuse Texture
	GetMaterialTextureName(pMaterial, pMtrlData, FbxSurfaceMaterial::sDiffuse);

	// Ambient
	pMtrlData->vAmb = GetMaterialProperty(pMaterial, pMtrlData,
		FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);

	// Specular
	pMtrlData->vSpc = GetMaterialProperty(pMaterial, pMtrlData,
		FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
	// Specular Texture
	GetMaterialTextureName(pMaterial, pMtrlData, FbxSurfaceMaterial::sSpecular);

	// Emissive
	pMtrlData->vEmv = GetMaterialProperty(pMaterial, pMtrlData,
		FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);

	// Specular Power
	FbxProperty fbxPower = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
	if (fbxPower.IsValid())
	{
		double dPower = fbxPower.Get<FbxDouble>();
		pMtrlData->fSpcPower = (float)dPower;
	}

	// TransparencyFactor
	FbxProperty	fbxTransparency = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (fbxTransparency.IsValid())
	{
		double dTransparencyFactor = fbxTransparency.Get<FbxDouble>();
		pMtrlData->fTransparencyFactor = (float)dTransparencyFactor;
	}

	// NormalMap Texture
	GetMaterialTextureName(pMaterial, pMtrlData, FbxSurfaceMaterial::sNormalMap);

	return true;
}

Vec3 CDx11FbxLoader::GetMaterialProperty(FbxSurfaceMaterial * pMaterial,
	PFBXMATERIALDATA pMtrlData, const char * pPropertyName, 
	const char * pPropertyFactorName)
{
	// ������ �Ӽ��� �о� �����ϴ� �Լ��̴�.
	// ����Ʈ�� Ÿ���� None���� ����.
	pMtrlData->eType = FMT_NONE;

	Vec3	vMtrl = Vec3Zero;

	// ������ Property, PropertyFactor�� ������,
	// ��ȿ���� üũ�Ѵ�.
	FbxProperty fbxProperty = pMaterial->FindProperty(pPropertyName);
	FbxProperty fbxPropertyFactor = pMaterial->FindProperty(pPropertyFactorName);
	if (fbxProperty.IsValid() && fbxPropertyFactor.IsValid())
	{
		FbxDouble3	vResult = fbxProperty.Get<FbxDouble3>();
		double	dFactor = fbxPropertyFactor.Get<FbxDouble>();

		if (dFactor != 0)
		{
			vResult[0] *= dFactor;
			vResult[1] *= dFactor;
			vResult[2] *= dFactor;
		}

		// ���� �����ϰ� Ÿ���� COLOR�� ��
		vMtrl.x = vResult[0];
		vMtrl.y = vResult[1];
		vMtrl.z = vResult[2];
		pMtrlData->eType = FMT_COLOR;
	}

	return vMtrl;
}

bool CDx11FbxLoader::GetMaterialTextureName(FbxSurfaceMaterial * pMaterial, PFBXMATERIALDATA pMtrlData, const char * pPropertyName)
{
	// ������ �ؽ�ó�� �о� �����ϴ� �Լ��̴�.

	// ������ Property�� ������,
	// ��ȿ���� üũ.
	FbxProperty fbxProperty = pMaterial->FindProperty(pPropertyName);
	if (fbxProperty.IsValid())
	{
		// �����ϴ� �ؽ��� ���� �����Ѵ�.
		int iExistTextureCount = 0;
		// GetSrcObjectCount�� Template�� ������ Ÿ���� ��ü ���� �������ش�.
		const int iTextureCount = fbxProperty.GetSrcObjectCount<FbxFileTexture>();

		for (int i = 0; i < iTextureCount; ++i)
		{
			// TextureCount ��ŭ ������ ���鼭
			// �ش� �ε����� FbxFileTexture�� ���´�.
			FbxFileTexture* pFileTexture = fbxProperty.GetSrcObject<FbxFileTexture>(i);
			if (!pFileTexture)
				continue;

			string strPath = pFileTexture->GetFileName();
			if (strcmp(pPropertyName, FbxSurfaceMaterial::sDiffuse) == 0)
				pMtrlData->strDiffuseTex = strPath;

			else if (strcmp(pPropertyName, FbxSurfaceMaterial::sSpecular) == 0)
				pMtrlData->strSpecularTex = strPath;

			else if (strcmp(pPropertyName, FbxSurfaceMaterial::sNormalMap) == 0)
				pMtrlData->strNormalMapTex = strPath;

			++iExistTextureCount;
		}

		// ���̾�� �Ǿ����� ��쵵 ó���Ѵ�.
		const int iLayeredTextureCount = fbxProperty.GetSrcObjectCount<FbxLayeredTexture>();
		for (int i = 0; i < iLayeredTextureCount; ++i)
		{
			FbxLayeredTexture*	pLayeredTexture = fbxProperty.GetSrcObject<FbxLayeredTexture>(i);
			const int	iTextureFileCount = pLayeredTexture->GetSrcObjectCount<FbxFileTexture>();
			for (int j = 0; j < iTextureFileCount; ++j)
			{
				FbxFileTexture*	pFileTexture = fbxProperty.GetSrcObject<FbxFileTexture>(j);
				if (!pFileTexture)
					continue;

				string strPath = pFileTexture->GetFileName();
				if (strcmp(pPropertyName, FbxSurfaceMaterial::sDiffuse) == 0)
					pMtrlData->strDiffuseTex = strPath;

				else if (strcmp(pPropertyName, FbxSurfaceMaterial::sSpecular) == 0)
					pMtrlData->strSpecularTex = strPath;

				else if (strcmp(pPropertyName, FbxSurfaceMaterial::sNormalMap) == 0)
					pMtrlData->strNormalMapTex = strPath;

				++iExistTextureCount;

			}
		}// LayeredTexture

		if (iExistTextureCount > 0)
		{
			if (pMtrlData->eType == FMT_COLOR)
				pMtrlData->eType = FMT_BOTH;

			else
				pMtrlData->eType = FMT_TEXTURE;
		}

	}
	return true;
}

void CDx11FbxLoader::LoadAnimationData(FbxMesh * pMesh, PMESHDATA pMeshData)
{
	bool	bVertexCache = pMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
		((FbxVertexCacheDeformer*)pMesh->GetDeformer(0, FbxDeformer::eVertexCache))->Active.Get();
	bool	bShape = pMesh->GetShapeCount() > 0;
	bool	bSkin = pMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
	bool	bDeformer = bSkin;

	pMeshData->bAnimation = bDeformer;

	if (bDeformer)
	{
		// Skin ������ �ε��Ѵ�.
		if (!LoadSkinningData(pMesh, pMeshData))
			return;
	}
}

bool CDx11FbxLoader::LoadAnimationClipData()
{
	int iStackCount = m_AnimNameArray.GetCount();
	for (int i = 0; i < iStackCount ; ++i)
	{
		// �ִϸ��̼� �̸��� �̿��ؿ� ������ ���´�.
		FbxAnimStack*	pAnimStack = m_pScene->FindMember<FbxAnimStack>(m_AnimNameArray[i]->Buffer());
		if (!pAnimStack)
			return false;

		PFBXANIMATIONCLIP	pClip = new FBXANIMATIONCLIP;
		//m_vecMeshData[m_iMeshCount]->pAnimationClip = pClip;
		m_vecAnimationClip.push_back(pClip);

		FbxString	strAnimStackName = pAnimStack->GetName();
		pClip->strName = strAnimStackName.Buffer();

		FbxTakeInfo*	pInfo = m_pScene->GetTakeInfo(strAnimStackName);
		FbxTime			startTime = pInfo->mLocalTimeSpan.GetStart();
		FbxTime			endTime = pInfo->mLocalTimeSpan.GetStop();
		FbxTime::EMode	eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();

		pClip->startTime = startTime;
		pClip->endTime = endTime;
		pClip->lTimeLength = endTime.GetFrameCount(eTimeMode) -
			startTime.GetFrameCount(eTimeMode);
	}
	return true;
}

void CDx11FbxLoader::LoadSkeleton(FbxNode * pNode)
{
	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		LoadSkeleton(pNode->GetChild(i), 0, 0, -1);
	}
}

void CDx11FbxLoader::LoadSkeleton(FbxNode * pNode, 
	int iDepth, int iIndex, int iParentIndex)
{
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr && pAttr->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FBXBONE	tBone = {};

		tBone.strName = pNode->GetName();
		tBone.iDepth = iDepth;
		tBone.iParentIndex = iParentIndex;

		m_vecBone.push_back(tBone);
	}

	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		LoadSkeleton(pNode->GetChild(i), iDepth + 1, m_vecBone.size(), iIndex);
	}
}

bool CDx11FbxLoader::LoadSkinningData(FbxMesh * pMesh, PMESHDATA pMeshData)
{
	FbxSkin*		pSkin = (FbxSkin*)pMesh->GetDeformer(0, FbxDeformer::eSkin);
	FbxSkin::EType	eSkinningType = pSkin->GetSkinningType();

	if (eSkinningType != FbxSkin::eRigid && eSkinningType != FbxSkin::eLinear)
		return false;

	int iSkinCount = pMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int i = 0; i < iSkinCount; ++i)
	{
		pSkin = (FbxSkin*)pMesh->GetDeformer(i, FbxDeformer::eSkin);

		int iClusterCount = pSkin->GetClusterCount();
		for (int j = 0; j < iClusterCount; ++j)
		{
			FbxCluster* pCluster = pSkin->GetCluster(j);
			if (!pCluster->GetLink())
				continue;

			string strBoneName = pCluster->GetLink()->GetName();
			int iBoneIndex = FindBoneFromName(strBoneName);
			FbxAMatrix	matGeometryTransform = GetTransform(pMesh->GetNode());

			// ����ġ�� �ε��� ������ �ε��Ѵ�.
			LoadWeightIndicesData(pCluster, iBoneIndex, pMeshData);

			// ������ ����� �ε��Ѵ�.
			LoadOffsetMatrixData(pCluster, matGeometryTransform, iBoneIndex, pMeshData);
			LoadTimeTransform(pMesh->GetNode(), pCluster, matGeometryTransform, iBoneIndex, pMeshData);
		}
	}

	ChangeVertexWeightAndIndices(pMesh, pMeshData);

	return true;
}

int CDx11FbxLoader::FindBoneFromName(const string & strName)
{
	for (size_t i = 0; i < m_vecBone.size(); ++i)
	{
		if (m_vecBone[i].strName == strName)
			return i;
	}

	return -2;
}

FbxAMatrix CDx11FbxLoader::GetTransform(FbxNode * pNode)
{
	const FbxVector4	vT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4	vR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4	vS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(vT, vR, vS);
}

void CDx11FbxLoader::LoadWeightIndicesData(FbxCluster * pCluster, int iBoneIndex, PMESHDATA pMeshData)
{
	int iWeightCount = pCluster->GetControlPointIndicesCount();
	int iClusterIndex = 0;

	for (int i = 0; i < iWeightCount; ++i)
	{
		FBXWEIGHTINDICES	tWI = {};

		tWI.iBoneIndex = iBoneIndex;
		tWI.dWeight = pCluster->GetControlPointWeights()[i];

		iClusterIndex = pCluster->GetControlPointIndices()[i];
		pMeshData->mapWI[iClusterIndex].push_back(tWI);
	}
}

void CDx11FbxLoader::LoadOffsetMatrixData(FbxCluster * pCluster, 
	const FbxAMatrix & matGeometryTransform, int iBoneIndex, 
	PMESHDATA pMeshData)
{
	FbxAMatrix	matTransform;
	FbxAMatrix	matTransformLink;

	pCluster->GetTransformMatrix(matTransform);
	pCluster->GetTransformLinkMatrix(matTransformLink);

	FbxVector4	v1 = { 1,0,0,0 };
	FbxVector4	v2 = { 0,0,1,0 };
	FbxVector4	v3 = { 0,1,0,0 };
	FbxVector4	v4 = { 0,0,0,1 };
	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	FbxAMatrix	matOffset;
	matOffset = matTransformLink.Inverse() * matTransform * matGeometryTransform;
	matOffset = matReflect * matOffset * matReflect;

	m_vecBone[iBoneIndex].matOffset = matOffset;
}

void CDx11FbxLoader::LoadTimeTransform(FbxNode * pNode, FbxCluster * pCluster, 
	const FbxAMatrix & matGeometryTransform, int iBoneIndex, PMESHDATA pMeshData)
{
	FbxTime::EMode	eTimeMode = m_pScene->GetGlobalSettings().GetTimeMode();

	FbxVector4	v1 = { 1,0,0,0 };
	FbxVector4	v2 = { 0,0,1,0 };
	FbxVector4	v3 = { 0,1,0,0 };
	FbxVector4	v4 = { 0,0,0,1 };
	FbxAMatrix	matReflect;
	matReflect.mData[0] = v1;
	matReflect.mData[1] = v2;
	matReflect.mData[2] = v3;
	matReflect.mData[3] = v4;

	FbxLongLong lStart =  m_vecAnimationClip[0]->startTime.GetFrameCount(FbxTime::eFrames30);
	FbxLongLong lEnd = m_vecAnimationClip[0]->endTime.GetFrameCount(FbxTime::eFrames30);

	for (FbxLongLong i = lStart; i <= lEnd; ++i)
	{
		FBXKEYFRAME	tKeyFrame;
		FbxTime		tCurTime;

		tCurTime.SetFrame(i, eTimeMode);

		FbxAMatrix	matOffset = pNode->EvaluateGlobalTransform(tCurTime) * matGeometryTransform;
		FbxAMatrix	matCurTransform = matOffset.Inverse() * pCluster->GetLink()->EvaluateGlobalTransform(tCurTime);

		matCurTransform = matReflect * matCurTransform * matReflect;

		tKeyFrame.dTime = tCurTime.GetSecondDouble();
		tKeyFrame.matTransform = matCurTransform;

		m_vecBone[iBoneIndex].vecKeyFrame.push_back(tKeyFrame);
	}
}

void CDx11FbxLoader::ChangeVertexWeightAndIndices(FbxMesh * pMesh, PMESHDATA pMeshData)
{
	// ����ġ�� �ε����� ����� map�� �ݺ��Ѵ�.
	unordered_map<int, vector<FBXWEIGHTINDICES>>::iterator	iter;
	unordered_map<int, vector<FBXWEIGHTINDICES>>::iterator	iterEnd = pMeshData->mapWI.end();

	const FbxVector4*	pControlPoints = pMesh->GetControlPoints();

	for (iter = pMeshData->mapWI.begin(); iter != iterEnd; ++iter)
	{
		if (iter->second.size() > 4)
		{
			// ����ġ ���� ���� ������������ �����Ѵ�.
			sort(iter->second.begin(), iter->second.end(), [](const FBXWEIGHTINDICES& lhs, const FBXWEIGHTINDICES& rhs)
			{
				return lhs.dWeight > rhs.dWeight;
			});

			// ����ġ�� ������ ���� �Ŀ� ����ġ�� ������ 1�� �� �� �ֵ��� 1���� ������ ���ش�.
			float	fSumWeight = 0.f, fInterpolateWeight = 0.f;
			for (int i = 0; i < 4; ++i)
			{
				fSumWeight += iter->second[i].dWeight;
			}

			fInterpolateWeight = 1.f - fSumWeight;

			vector<FBXWEIGHTINDICES>::iterator	iterErase = iter->second.begin() + 4;
			iter->second.erase(iterErase, iter->second.end());
			iter->second[0].dWeight += fInterpolateWeight;
		}

		float	fWeight[4] = {};
		int		iIndices[4] = {};

		for (size_t i = 0; i < iter->second.size(); ++i)
		{
			fWeight[i] = iter->second[i].dWeight;
			iIndices[i] = iter->second[i].iBoneIndex;
		}

		Vec4	vWeight = fWeight;
		Vec4	vIndices = iIndices;

		pMeshData->vecWeights[iter->first] = vWeight;
		pMeshData->vecIndices[iter->first] = vIndices;
	}
}
