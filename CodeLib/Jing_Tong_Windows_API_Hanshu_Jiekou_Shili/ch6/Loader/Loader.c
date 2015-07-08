/* ************************************
*����ͨWindows API��
* ʾ������
* Loader.c
* 6.5  ��̬���ӿ�
**************************************/
/* ͷ�ļ���*/ 
#include <stdio.h> 
#include <windows.h> 
/* ���Ͷ��塡*/

// ָ������ָ������
typedef DWORD (*MY_EXPORT_PROC)(LPSTR, DWORD);

// main
VOID main(VOID) 
{ 
	HINSTANCE hinstLib; 
	MY_EXPORT_PROC lpProcAdd; 
	BOOL fFreeResult, fRunTimeLinkSuccess = FALSE; 

	// ���Dllģ����
	hinstLib = LoadLibrary(TEXT("msg.dll")); 

	// �Ƿ���سɹ�
	if (hinstLib != NULL) 
	{ 
		// ���ָ�����������ĵ�ַ
		lpProcAdd = (MY_EXPORT_PROC) GetProcAddress(hinstLib, "ExportExample"); 

		// �ж��Ƿ�ɹ�
		if (NULL != lpProcAdd) 
		{
			fRunTimeLinkSuccess = TRUE;
			(lpProcAdd) ("hello",123); 
		} 
		// �ͷ�DLL 
		fFreeResult = FreeLibrary(hinstLib); 
	} 

	// ����
	if (! fRunTimeLinkSuccess) 
		printf("error; %u\n",GetLastError()); 
}