#include "stdafx.h"
#include "Tool.h"


CTool::CTool()
{
}


CTool::~CTool()
{
}

CAtlString CTool::GetAppPath()
{
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, (LPWCH)path, MAX_PATH);
	CAtlString returnStringPath = path;
	int index = returnStringPath.ReverseFind(_T('\\'));
	if (index == -1) returnStringPath.Empty();
	else returnStringPath = returnStringPath.Left(index + 1);
	return returnStringPath;
}

const CAtlString CTool::GetFullPath()
{
	CAtlString path = GetAppPath() + _T("uilibTemplate.exe");
	return path;
}
