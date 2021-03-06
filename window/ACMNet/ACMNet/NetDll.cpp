#include "stdafx.h"
#include "NetDll.h"


NetDll::NetDll()
{
	Hinst = NULL;
}


NetDll::~NetDll()
{
	if (Hinst != NULL){
		FreeLibrary(Hinst);
		Hinst = NULL;
	}
}

void NetDll:: LoadDll(LPCTSTR path){
	Hinst = LoadLibrary(path); //获取链接库实例句柄
	if (Hinst == NULL){
		AfxMessageBox(_T("Load.Dll file failed"));
		return;
	}
}

void NetDll::OpenDll(){
	TCHAR  format[100] = _T("Files (*.dll)|*.dll||");
	CFileDialog file((int)TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, format);//打开文件对话框
	file.DoModal();
	CString path;
	path = file.GetPathName();//获取链接库路径
	if (path.IsEmpty())
		return;
	Hinst = LoadLibrary(path); //获取链接库实例句柄
	if (Hinst == NULL){
		AfxMessageBox(_T("Load.Dll file failed"));
		return;
	}
}