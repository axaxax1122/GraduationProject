#include "Dx11FilePathManager.h"

DX11_USING

DX11_DEFINITION_SINGLE(CDx11FilePathManager)

CDx11FilePathManager::CDx11FilePathManager()
{
	SetTypeName<CDx11FilePathManager>();
}


CDx11FilePathManager::~CDx11FilePathManager()
{
}

bool CDx11FilePathManager::Init()
{
	wchar_t	strPath[MAX_PATH] = {};

	// ���� �����ϰ� �ִ� ���丮 ��θ� ���´�.
	// .exe���Ϸ� ����������, �������ؼ� �������� �� ��ΰ� �ٸ�.
	GetCurrentDirectory(MAX_PATH, strPath);
	
	int iLength = lstrlen(strPath);

	for (int i = iLength - 1; i >= 0; --i)
	{
		// '/' �� '\' ���ö����� �� ĭ�� ������ �̵�
		if (strPath[i] == '/' || strPath[i] == '\\')
		{
			memset(&strPath[i + 1], 0, sizeof(wchar_t) * (MAX_PATH - (i + 1)));
			break;
		}
	}

#ifdef _DEBUG
	lstrcat(strPath, L"Bin_Debug\\");
#else
	lstrcat(strPath, L"Bin\\");
#endif // _DEBUG

	//��θ� �ʿ� �����Ѵ�.
	m_mapPath.insert(make_pair(ROOT_PATH, strPath));

	memset(strPath, 0, sizeof(wchar_t) * MAX_PATH);
#ifdef _DEBUG
	lstrcpy(strPath, L"../Bin_Debug\\");
#else
	lstrcpy(strPath, L"../Bin\\");
#endif
	m_mapPath.insert(make_pair(ENGINE_PATH, strPath));

	lstrcat(strPath, L"../../Resources/Textures\\");
	m_mapPath.insert(make_pair(ENGINE_TEXTURE_PATH, strPath));


	//���̴��� ����ִ� ������ ��θ� �����Ѵ�.
	CreatePath(SHADER_PATH, L"Shader\\");
	
	//�ؽ�ó�� ����ִ� ������ ��θ� �����Ѵ�.
	CreatePath(TEXTURE_PATH, L"../../Resources/Textures\\");

	//�Ž��� ����ִ� ������ ��θ� �����Ѵ�.
	CreatePath(MESH_PATH, L"../../Resources/Mesh\\");

	return true;
}

const wchar_t * CDx11FilePathManager::CreatePath(const string & strKey, wchar_t * pPath, const string & strBaseKey)
{
	if (FindPath(strKey))
		return NULL;

	//������ ��θ� �ҷ��´�.
	wstring strPath = FindPath(strBaseKey);

	//��ο� pPath�� ���Ͽ� ���� ��θ� �����.
	strPath += pPath;

	//�ʿ� �߰��Ѵ�.
	m_mapPath.insert(make_pair(strKey, strPath));

	return strPath.c_str();
}

const wchar_t * CDx11FilePathManager::FindPath(const string & strKey)
{
	unordered_map<string, wstring>::iterator iter = m_mapPath.find(strKey);

	if (iter == m_mapPath.end())
		return NULL;

	return iter->second.c_str();
}
