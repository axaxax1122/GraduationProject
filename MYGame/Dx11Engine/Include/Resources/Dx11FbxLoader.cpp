#include "Dx11FbxLoader.h"

DX11_USING

CDx11FbxLoader::CDx11FbxLoader() :
	m_pManager(NULL),
	m_pScene(NULL)
{
}

CDx11FbxLoader::~CDx11FbxLoader()
{
	//Safe_Delete_VecList(m_vecMaterial);
	m_pScene->Destroy();
	for (size_t i = 0; i < m_vecMeshData.size(); ++i)
	{
		Safe_Delete_VecList(m_vecMeshData[i]->vecMaterial);
	}

	Safe_Delete_VecList(m_vecMeshData);
	m_pManager->Destroy();
}

vector<PMESHDATA>& CDx11FbxLoader::GetMeshContainer()
{
	return m_vecMeshData;
}

int CDx11FbxLoader::GetContainerCount()
{
	return m_vecMeshData.size();
}

int CDx11FbxLoader::GetSubsetCount(int iContainer)
{
	return m_vecMeshData[iContainer]->vecIndex.size();
}

bool CDx11FbxLoader::LoadMesh(char * pFileName)
{
	// Fbx Manager Create
	m_pManager = FbxManager::Create();//1

	if (!m_pManager)
		return false;

	// ����� ������ �����.
	FbxIOSettings* ios = FbxIOSettings::Create(m_pManager, IOSROOT);//2.
																	// �Ŵ����� ������� ����� ������ �����Ѵ�.
	m_pManager->SetIOSettings(ios);

	FbxString lPath = FbxGetApplicationDirectory();//3.
	m_pManager->LoadPluginsDirectory(lPath.Buffer());

	FbxImporter* pImporter = FbxImporter::Create(m_pManager, "");//4.

																 // Importer�� Initialize�� ���ָ� ���ڷ� �� ���ϸ��� fbx�� �о�� �� �ֵ���
																 // �غ��Ų��.
	if (!pImporter->Initialize(pFileName, -1, m_pManager->GetIOSettings()))//5.
		return false;

	m_pScene = FbxScene::Create(m_pManager, "FbxScene");

	pImporter->Import(m_pScene);//7.

								// Fbx ���� �����ش�.
	FbxAxisSystem SceneAxisSystem = m_pScene->GetGlobalSettings().GetAxisSystem();

	if (SceneAxisSystem != FbxAxisSystem::Max)
		FbxAxisSystem::Max.ConvertScene(m_pScene);

	Triangulate(m_pScene->GetRootNode());

	if (!LoadNodeData(m_pScene->GetRootNode()))
		return false;

	pImporter->Destroy();

	return true;
}

void CDx11FbxLoader::Triangulate(FbxNode * pNode)
{
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr)
	{
		if (pAttr->GetAttributeType() == FbxNodeAttribute::eMesh ||
			pAttr->GetAttributeType() == FbxNodeAttribute::eNurbs ||
			pAttr->GetAttributeType() == FbxNodeAttribute::eNurbsSurface)
		{
			FbxGeometryConverter	converter(m_pManager);

			converter.Triangulate(pAttr, true);
		}
	}

	int	iChildCount = pNode->GetChildCount();

	for (int i = 0; i < iChildCount; ++i)
	{
		Triangulate(pNode->GetChild(i));
	}
}

bool CDx11FbxLoader::LoadNodeData(FbxNode * pNode)
{
	FbxNodeAttribute*	pAttr = pNode->GetNodeAttribute();

	if (pAttr)
	{
		if (pAttr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			FbxMesh*	pMesh = pNode->GetMesh();

			if (pMesh)
			{
				if (!LoadMeshData(pMesh))
					return false;

				int	iMtrlCount = pNode->GetMaterialCount();

				for (int i = 0; i < iMtrlCount; ++i)
				{
					FbxSurfaceMaterial*	pMaterial = pNode->GetMaterial(i);

					if (!pMaterial)
						continue;

					LoadMaterialData(pMaterial);
				}
			}
		}
	}

	int	iChildCount = pNode->GetChildCount();

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

	pMeshData->bAnimation = false;
	pMeshData->bBump = false;

	m_vecMeshData.push_back(pMeshData);

	// ������ ���� ���´�.
	int	iPolygonCount = pMesh->GetPolygonCount();
	FbxVector4*	pControlPoints = pMesh->GetControlPoints();
	int	iControlPointsCount = pMesh->GetControlPointsCount();

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
		pMeshData->vecNormal.push_back(Vec3Zero);
		pMeshData->vecUV.push_back(Vec2(0.f, 0.f));
		pMeshData->vecTangent.push_back(Vec3Zero);
		pMeshData->vecBinormal.push_back(Vec3Zero);
		pMeshData->vecWeights.push_back(Vec4(0.f, 0.f, 0.f, 0.f));
		pMeshData->vecIndices.push_back(Vec4(0.f, 0.f, 0.f, 0.f));
	}

	pMeshData->iPolygonCount = iPolygonCount;

	UINT	iVertexIdx = 0;

	int	iMtrlElementCount = pMesh->GetElementMaterialCount();

	// ���� ������ ���´�.
	int	iMtrlCount = pMesh->GetNode()->GetMaterialCount();

	for (int i = 0; i < iMtrlCount; ++i)
	{
		vector<UINT>	vecIndex;
		pMeshData->vecIndex.push_back(vecIndex);
	}

	// ������ ����ŭ �ݺ��� ���� �� �������� ���������� ���´�.
	for (int i = 0; i < iPolygonCount; ++i)
	{
		int	iPolygonSize = pMesh->GetPolygonSize(i);
		int	iMaterialID = 0;

		if (iMtrlElementCount > 0)
		{
			FbxGeometryElementMaterial*	pMaterial = pMesh->GetElementMaterial(0);
			iMaterialID = pMaterial->GetIndexArray().GetAt(i);
		}

		int	iIndex[3] = {};

		for (int j = 0; j < iPolygonSize; ++j)
		{
			// �ﰢ���� �����ϱ� ���� ������ �ε��� ������ ���´�.
			int	iControlIndex = pMesh->GetPolygonVertex(i, j);

			iIndex[j] = iControlIndex;

			// Normal ������ �о�´�.
			int	iNormalCount = pMesh->GetElementNormalCount();

			for (int k = 0; k < iNormalCount; ++k)
			{
				FbxGeometryElementNormal*	pNormal = pMesh->GetElementNormal(k);

				if (pNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					int	iNormalIndex = 0;
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
						//pMeshData->vecNormal.push_back(vNormal);
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
			}// Load Normal

			 // Texture UV ������ �о�´�.
			for (int k = 0; k < pMesh->GetElementUVCount(); ++k)
			{
				FbxGeometryElementUV*	pUV = pMesh->GetElementUV(k);

				if (pUV)
				{
					FbxVector2	vFbxUV;
					Vec2	vUV;
					int		iUVIndex = 0;
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
			}// Texture UV

			 // Tangent ������ �о�´�.
			int	iTangentCount = pMesh->GetElementTangentCount();
			for (int k = 0; k < iTangentCount; ++k)
			{
				FbxGeometryElementTangent*	pTangent = pMesh->GetElementTangent(k);

				if (pTangent)
				{
					int	iTangentIndex = 0;
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
			}// Tangent

			 // BiNormal ������ �о�´�.
			int	iBinormalCount = pMesh->GetElementBinormalCount();
			for (int k = 0; k < iBinormalCount; ++k)
			{
				FbxGeometryElementBinormal*	pBinormal = pMesh->GetElementBinormal(k);

				if (pBinormal)
				{
					int	iBinormalIndex = 0;
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

	return true;
}

bool CDx11FbxLoader::LoadMaterialData(FbxSurfaceMaterial * pMaterial)
{
	if (!pMaterial)
		return false;

	PFBXMATERIALDATA	pMtrlData = new FBXMATERIALDATA;

	PMESHDATA	pContainer = m_vecMeshData.back();

	pContainer->vecMaterial.push_back(pMtrlData);

	if (pMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
		pMtrlData->eNode = FMN_LAMBERT;

	else if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		pMtrlData->eNode = FMN_PHONG;

	// Diffuse������ �о�´�.
	pMtrlData->vDif = GetMaterialProperty(pMaterial, pMtrlData,
		FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	// DiffuseTexture ������ �о�´�.
	GetMateiralTextureName(pMaterial, pMtrlData, FbxSurfaceMaterial::sDiffuse);

	// Ambient ������ �о�´�.
	pMtrlData->vAmb = GetMaterialProperty(pMaterial, pMtrlData,
		FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);

	// Specular ������ �о�´�.
	pMtrlData->vSpc = GetMaterialProperty(pMaterial, pMtrlData,
		FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
	// SpecularTexture ������ �о�´�.
	GetMateiralTextureName(pMaterial, pMtrlData, FbxSurfaceMaterial::sSpecular);

	// Emissive�� �о�´�.
	pMtrlData->vEmv = GetMaterialProperty(pMaterial, pMtrlData,
		FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);

	// Power�� �о�´�.
	FbxProperty	fbxPower = pMaterial->FindProperty(FbxSurfaceMaterial::sShininess);
	if (fbxPower.IsValid())
	{
		double dPower = fbxPower.Get<FbxDouble>();
		pMtrlData->fSpcPower = (float)dPower;
	}

	// TransparencyFactor�� �о�´�.
	FbxProperty	fbxTransparency = pMaterial->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);

	if (fbxTransparency.IsValid())
	{
		double dTransparencyFactor = fbxTransparency.Get<FbxDouble>();
		pMtrlData->fTransparencyFactor = (float)dTransparencyFactor;
	}

	// NormalMap Texture�� �о�´�.
	GetMateiralTextureName(pMaterial, pMtrlData, FbxSurfaceMaterial::sNormalMap);

	return true;
}

Vec3 CDx11FbxLoader::GetMaterialProperty(FbxSurfaceMaterial * pMaterial,
	PFBXMATERIALDATA pMtrlData, const char * pPropertyName, const char * pPropertyFactorName)
{
	pMtrlData->eType = FMT_NONE;

	Vec3	vMtrl = Vec3Zero;
	// Material Property�� ���´�.
	FbxProperty	fbxProperty = pMaterial->FindProperty(pPropertyName);
	// Material Property Factor�� ���´�.
	FbxProperty	fbxPropertyFactor = pMaterial->FindProperty(pPropertyFactorName);

	// ���� �Ӽ��� �Ӽ�Factor��  ��ȿ���� üũ�Ѵ�.
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

		vMtrl.x = vResult[0];
		vMtrl.y = vResult[1];
		vMtrl.z = vResult[2];

		pMtrlData->eType = FMT_COLOR;
	}

	return vMtrl;
}

bool CDx11FbxLoader::GetMateiralTextureName(FbxSurfaceMaterial * pMaterial,
	PFBXMATERIALDATA pMtrlData, const char * pPropertyName)
{
	// Material Property�� ���´�.
	FbxProperty	fbxProperty = pMaterial->FindProperty(pPropertyName);

	// �Ӽ��� ��ȿ���� üũ�Ѵ�.
	if (fbxProperty.IsValid())
	{
		// �����ϴ� �ؽ��� ���� �����Ѵ�.
		int	iExistTextureCount = 0;
		// GetSrcObjectCount�� Template�� ������ Ÿ���� ��ü ���� �������ش�.
		const int	iTextureCount = fbxProperty.GetSrcObjectCount<FbxFileTexture>();

		for (int i = 0; i < iTextureCount; ++i)
		{
			// FbxFileTexture�� ���´�.
			FbxFileTexture*	pFileTexture = fbxProperty.GetSrcObject<FbxFileTexture>(i);

			if (!pFileTexture)
				continue;

			string	strPath = pFileTexture->GetFileName();

			if (strcmp(pPropertyName, FbxSurfaceMaterial::sDiffuse) == 0)
				pMtrlData->strDiffuseTex = strPath;

			else if (strcmp(pPropertyName, FbxSurfaceMaterial::sSpecular) == 0)
				pMtrlData->strSpecularTex = strPath;

			else if (strcmp(pPropertyName, FbxSurfaceMaterial::sNormalMap) == 0)
				pMtrlData->strNormalMapTex = strPath;

			++iExistTextureCount;
		}

		const int	iLayeredTextureCount = fbxProperty.GetSrcObjectCount<FbxLayeredTexture>();

		for (int i = 0; i < iLayeredTextureCount; ++i)
		{
			FbxLayeredTexture*	pLayeredTexture = fbxProperty.GetSrcObject<FbxLayeredTexture>(i);

			const int	iTextureFileCount = pLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

			for (int j = 0; j < iTextureFileCount; ++j)
			{
				FbxFileTexture*	pFileTexture = pLayeredTexture->GetSrcObject<FbxFileTexture>(j);

				if (!pFileTexture)
					continue;

				string	strPath = pFileTexture->GetFileName();

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
