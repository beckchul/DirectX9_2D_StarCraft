#pragma once

template<class T>
static inline void Safe_Delete(T& rData)
{
	if(rData)
	{
		delete rData;
		rData = NULL;
	}
}

struct DEL_OBJ
{
	template<typename T>
	void operator () (T& rTemp)
	{
		if (rTemp)
		{
			delete rTemp;
			rTemp = NULL;
		}
	}
};

inline wstring CorverRelativePath(wstring szFullPath)
{
	TCHAR szRelativePath[MAX_STR] = L"";
	TCHAR szDirectoryPath[MAX_STR] = L"";

	GetCurrentDirectory(sizeof(szDirectoryPath), szDirectoryPath);

	PathRelativePathTo((LPWSTR)szRelativePath,
		szDirectoryPath,
		FILE_ATTRIBUTE_DIRECTORY,
		szFullPath.c_str(),
		FILE_ATTRIBUTE_NORMAL);

	return szRelativePath;
}

#ifdef __MFC_PROJECT__

static int DirectoryFileCount(const wstring& wstrPath)
{

	wstring wstrTempName = wstrPath + L"\\*.png";

	CFileFind Find;
	Find.FindFile(wstrTempName.c_str());

	int iContinue = 1;
	int iFileCount = 0;

	while (iContinue)
	{
		iContinue = Find.FindNextFile();


		if (!Find.IsDirectory())
			++iFileCount;
	}

	return iFileCount;
}

static void DirectoryInfo(const wstring& wstrPath,
	list<IMAGE_PATH*>&	rListImagePath)
{
	CFileFind Find;

	Find.FindFile(wstrPath.c_str());

	int iContinue = 1;

	while (iContinue)
	{
		iContinue = Find.FindNextFile();

		if (Find.IsDots())
			continue;
		if (Find.IsDirectory())
		{
			DirectoryInfo(wstring(Find.GetFilePath()) + L"/*.*", rListImagePath);
		}
		else
		{
			if (Find.IsSystem())
				continue;

			IMAGE_PATH* pImagePath = new IMAGE_PATH;
			TCHAR	szPath[MAX_STR] = L"";

			lstrcpy(szPath, Find.GetFilePath().operator LPCWSTR());
			PathRemoveFileSpec(szPath);

			pImagePath->iCount = DirectoryFileCount(szPath);

			wstring wstrTextureName = Find.GetFileTitle().operator LPCWSTR();

			wstrTextureName
				= wstrTextureName.substr(0
					, wstrTextureName.size() - 1) + L"%d.png";

			TCHAR szBuf[MAX_STR] = L"";

			lstrcpy(szBuf, Find.GetFilePath().operator LPCWSTR());
			PathRemoveFileSpec(szBuf);

			PathCombine(szBuf, szBuf, wstrTextureName.c_str());
			lstrcpy(pImagePath->szPath, CorverRelativePath(szBuf).c_str());

			PathRemoveFileSpec(szBuf);
			lstrcpy(pImagePath->szStateKey, PathFindFileName(szBuf));

			PathRemoveFileSpec(szBuf);
			lstrcpy(pImagePath->szObjKey, PathFindFileName(szBuf));

			rListImagePath.push_back(pImagePath);
			Find.FindFile(Find.GetFilePath());
			Find.FindNextFile();
		}
	}
}

#endif