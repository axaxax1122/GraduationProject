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

	// 현재 동작하고 있는 디렉토리 경로를 얻어온다.
	// .exe파일로 실행했을때, 컴파일해서 실행했을 때 경로가 다름.
	GetCurrentDirectory(MAX_PATH, strPath);
	
	int iLength = lstrlen(strPath);

	for (int i = iLength - 1; i >= 0; --i)
	{
		// '/' 나 '\' 나올때까지 한 칸씩 앞으로 이동
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

	//경로를 맵에 저장한다.
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


	//쉐이더가 들어있는 폴더의 경로를 생성한다.
	CreatePath(SHADER_PATH, L"Shader\\");
	
	//텍스처가 들어있는 폴더의 경로를 생성한다.
	CreatePath(TEXTURE_PATH, L"../../Resources/Textures\\");

	//매쉬가 들어있는 폴더의 경로를 생성한다.
	CreatePath(MESH_PATH, L"../../Resources/Mesh\\");

	return true;
}

const wchar_t * CDx11FilePathManager::CreatePath(const string & strKey, wchar_t * pPath, const string & strBaseKey)
{
	if (FindPath(strKey))
		return NULL;

	//저장한 경로를 불러온다.
	wstring strPath = FindPath(strBaseKey);

	//경로에 pPath를 더하여 최종 경로를 만든다.
	strPath += pPath;

	//맵에 추가한다.
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
